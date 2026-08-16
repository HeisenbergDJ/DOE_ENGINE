/********************************************************************
 * "sha256.cc" provides the SHA-256 digest helper.                 *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "sha256.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

namespace doe::utils {

namespace {

std::uint32_t RotateRight(std::uint32_t value, int bits)
{
    return (value >> bits) | (value << (32 - bits));
}

const std::array<std::uint32_t, 64> kConstants = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

}  // namespace

std::string Sha256Hex(const std::string& data)
{
    std::uint32_t hash[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                             0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

    const std::uint64_t bit_length = static_cast<std::uint64_t>(data.size()) * 8;
    std::vector<std::uint8_t> message(data.begin(), data.end());
    message.push_back(0x80);
    while (message.size() % 64 != 56) {
        message.push_back(0);
    }
    for (int shift = 7; shift >= 0; --shift) {
        message.push_back(static_cast<std::uint8_t>((bit_length >> (shift * 8)) & 0xff));
    }

    for (std::size_t offset = 0; offset < message.size(); offset += 64) {
        std::array<std::uint32_t, 64> words{};
        for (int i = 0; i < 16; ++i) {
            const std::size_t base = offset + static_cast<std::size_t>(i) * 4;
            words[i] = (static_cast<std::uint32_t>(message[base]) << 24) |
                       (static_cast<std::uint32_t>(message[base + 1]) << 16) |
                       (static_cast<std::uint32_t>(message[base + 2]) << 8) |
                       static_cast<std::uint32_t>(message[base + 3]);
        }
        for (int i = 16; i < 64; ++i) {
            const std::uint32_t small_sigma0 =
                RotateRight(words[i - 15], 7) ^ RotateRight(words[i - 15], 18) ^
                (words[i - 15] >> 3);
            const std::uint32_t small_sigma1 =
                RotateRight(words[i - 2], 17) ^ RotateRight(words[i - 2], 19) ^
                (words[i - 2] >> 10);
            words[i] = words[i - 16] + small_sigma0 + words[i - 7] + small_sigma1;
        }

        std::uint32_t a = hash[0];
        std::uint32_t b = hash[1];
        std::uint32_t c = hash[2];
        std::uint32_t d = hash[3];
        std::uint32_t e = hash[4];
        std::uint32_t f = hash[5];
        std::uint32_t g = hash[6];
        std::uint32_t h = hash[7];

        for (int i = 0; i < 64; ++i) {
            const std::uint32_t big_sigma1 =
                RotateRight(e, 6) ^ RotateRight(e, 11) ^ RotateRight(e, 25);
            const std::uint32_t choice = (e & f) ^ (~e & g);
            const std::uint32_t temp1 = h + big_sigma1 + choice + kConstants[i] + words[i];
            const std::uint32_t big_sigma0 =
                RotateRight(a, 2) ^ RotateRight(a, 13) ^ RotateRight(a, 22);
            const std::uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
            const std::uint32_t temp2 = big_sigma0 + majority;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    std::string result;
    result.reserve(64);
    char buffer[9];
    for (const std::uint32_t word : hash) {
        std::snprintf(buffer, sizeof buffer, "%08x", word);
        result += buffer;
    }
    return result;
}

}  // namespace doe::utils
