/********************************************************************
 * "mpi_scheduler.cc" provides the MPI point scheduler.           *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "mpi_scheduler.h"

#include <mpi.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

namespace doe::runtime {

namespace {

constexpr int kRunIdBufferSize = 64;

// One MPI tag carries both record batches and the end-of-stream marker; the
// marker is a header whose count field equals kDoneSentinel. Using a single
// tag keeps MPI's per-source, per-destination message ordering guarantee, so
// the root always sees a rank's batches before its marker.
constexpr int kRecordMessageTag = 7;
constexpr std::uint32_t kDoneSentinel = 0xFFFFFFFFu;

struct DeserializedRecords {
    bool done = false;
    std::vector<std::string> records;
};

void AppendU32(std::vector<char>& buffer, std::uint32_t value)
{
    buffer.push_back(static_cast<char>((value >> 0) & 0xFF));
    buffer.push_back(static_cast<char>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<char>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<char>((value >> 24) & 0xFF));
}

std::uint32_t ReadU32(const char* data)
{
    const auto bytes = reinterpret_cast<const unsigned char*>(data);
    return (static_cast<std::uint32_t>(bytes[0]) << 0) |
           (static_cast<std::uint32_t>(bytes[1]) << 8) |
           (static_cast<std::uint32_t>(bytes[2]) << 16) |
           (static_cast<std::uint32_t>(bytes[3]) << 24);
}

// Wire format: uint32 count, then count pairs of (uint32 length, bytes).
std::vector<char> SerializeRecords(const std::vector<std::string>& records)
{
    std::size_t size = sizeof(std::uint32_t);
    for (const std::string& record : records) {
        size += sizeof(std::uint32_t) + record.size();
    }
    std::vector<char> buffer;
    buffer.reserve(size);
    AppendU32(buffer, static_cast<std::uint32_t>(records.size()));
    for (const std::string& record : records) {
        AppendU32(buffer, static_cast<std::uint32_t>(record.size()));
        buffer.insert(buffer.end(), record.begin(), record.end());
    }
    return buffer;
}

DeserializedRecords DeserializeRecords(const char* data, std::size_t size)
{
    DeserializedRecords result;
    if (size < sizeof(std::uint32_t)) {
        return result;
    }
    const std::uint32_t count = ReadU32(data);
    if (count == kDoneSentinel) {
        result.done = true;
        return result;
    }
    std::size_t offset = sizeof(std::uint32_t);
    result.records.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        if (offset + sizeof(std::uint32_t) > size) {
            return result;
        }
        const std::uint32_t length = ReadU32(data + offset);
        offset += sizeof(std::uint32_t);
        if (offset + length > size) {
            return result;
        }
        result.records.emplace_back(data + offset, length);
        offset += length;
    }
    return result;
}

}  // namespace

MpiScheduler::MpiScheduler()
{
    int initialized = 0;
    MPI_Initialized(&initialized);
    if (!initialized) {
        MPI_Init(nullptr, nullptr);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
    MPI_Comm_size(MPI_COMM_WORLD, &size_);
}

MpiScheduler::~MpiScheduler()
{
    int finalized = 0;
    MPI_Finalized(&finalized);
    if (!finalized) {
        MPI_Finalize();
    }
}

std::string MpiScheduler::BeginRun(const std::string& candidate_run_id) const
{
    char buffer[kRunIdBufferSize] = {};
    if (rank_ == 0) {
        std::strncpy(buffer, candidate_run_id.c_str(), kRunIdBufferSize - 1);
    }
    MPI_Bcast(buffer, kRunIdBufferSize, MPI_CHAR, 0, MPI_COMM_WORLD);
    return std::string(buffer);
}

bool MpiScheduler::IsRoot() const
{
    return rank_ == 0;
}

int MpiScheduler::Rank() const
{
    return rank_;
}

RunSummary MpiScheduler::Run(std::size_t point_count,
                             const std::function<bool(std::size_t)>& run_point) const
{
    const std::size_t block =
        (point_count + static_cast<std::size_t>(size_) - 1) / static_cast<std::size_t>(size_);
    const std::size_t begin = static_cast<std::size_t>(rank_) * block;
    const std::size_t end = std::min(begin + block, point_count);

    unsigned long long local_success = 0;
    for (std::size_t index = begin; index < end; ++index) {
        if (run_point(index)) {
            ++local_success;
        }
    }

    unsigned long long global_success = 0;
    MPI_Allreduce(&local_success, &global_success, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM,
                  MPI_COMM_WORLD);

    RunSummary summary;
    summary.point_count = point_count;
    summary.success_count = static_cast<std::size_t>(global_success);
    return summary;
}

ResumeInfo MpiScheduler::BroadcastResumeInfo(const ResumeInfo& local) const
{
    ResumeInfo info;

    int abort_flag = local.abort ? 1 : 0;
    MPI_Bcast(&abort_flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
    info.abort = (abort_flag != 0);

    int message_length = static_cast<int>(local.abort_message.size());
    MPI_Bcast(&message_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    std::string message = local.abort_message;
    message.resize(static_cast<std::size_t>(message_length));
    if (message_length > 0) {
        MPI_Bcast(message.data(), message_length, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    info.abort_message = std::move(message);

    std::vector<std::uint64_t> points;
    if (rank_ == 0) {
        points.reserve(local.terminal_points.size());
        for (const std::size_t point : local.terminal_points) {
            points.push_back(static_cast<std::uint64_t>(point));
        }
    }
    int point_count = static_cast<int>(points.size());
    MPI_Bcast(&point_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    points.resize(static_cast<std::size_t>(point_count));
    if (point_count > 0) {
        MPI_Bcast(points.data(), point_count, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    }
    info.terminal_points.reserve(points.size());
    for (const std::uint64_t point : points) {
        info.terminal_points.push_back(static_cast<std::size_t>(point));
    }

    return info;
}

void MpiScheduler::SendBatch(const std::vector<std::string>& records) const
{
    if (rank_ == 0) {
        return;
    }
    const std::vector<char> buffer = SerializeRecords(records);
    MPI_Send(buffer.data(), static_cast<int>(buffer.size()), MPI_CHAR, 0, kRecordMessageTag,
             MPI_COMM_WORLD);
}

void MpiScheduler::FinishSending() const
{
    if (rank_ == 0) {
        return;
    }
    std::vector<char> buffer;
    AppendU32(buffer, kDoneSentinel);
    MPI_Send(buffer.data(), static_cast<int>(buffer.size()), MPI_CHAR, 0, kRecordMessageTag,
             MPI_COMM_WORLD);
}

void MpiScheduler::DrainAvailable(
    const std::function<void(const std::vector<std::string>&)>& persist) const
{
    if (rank_ != 0) {
        return;
    }
    int probe_flag = 0;
    while (true) {
        MPI_Status status;
        MPI_Iprobe(MPI_ANY_SOURCE, kRecordMessageTag, MPI_COMM_WORLD, &probe_flag, &status);
        if (!probe_flag) {
            break;
        }
        ReceiveOne(persist, status);
    }
}

void MpiScheduler::DrainAll(
    const std::function<void(const std::vector<std::string>&)>& persist) const
{
    if (rank_ != 0) {
        return;
    }
    while (finished_sources_.size() < static_cast<std::size_t>(size_ - 1)) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, kRecordMessageTag, MPI_COMM_WORLD, &status);
        ReceiveOne(persist, status);
    }
}

void MpiScheduler::ReceiveOne(
    const std::function<void(const std::vector<std::string>&)>& persist,
    const MPI_Status& status) const
{
    int count = 0;
    MPI_Get_count(&status, MPI_CHAR, &count);
    std::vector<char> buffer(static_cast<std::size_t>(count));
    MPI_Recv(buffer.data(), count, MPI_CHAR, status.MPI_SOURCE, kRecordMessageTag, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

    const DeserializedRecords deserialized = DeserializeRecords(buffer.data(), buffer.size());
    if (deserialized.done) {
        finished_sources_.insert(status.MPI_SOURCE);
    } else if (!deserialized.records.empty()) {
        persist(deserialized.records);
    }
}

}  // namespace doe::runtime
