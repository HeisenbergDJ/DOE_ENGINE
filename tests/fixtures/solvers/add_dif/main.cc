// A minimal test solver used by the multiple_extractors fixture. Reads
// "x1 = value" and "x2 = value" from an absolute input file, and writes
// "y1 = x1+x2" and "y2 = x1-x2" to the sibling <input>.out file.
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace {

std::string Trim(std::string_view text) {
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) {
        return {};
    }

    const auto last = text.find_last_not_of(" \t\r\n");
    return std::string(text.substr(first, last - first + 1));
}

bool ParseValue(const std::string& text, long double& value) {
    std::istringstream stream(text);
    if (!(stream >> value)) {
        return false;
    }

    stream >> std::ws;
    return stream.eof();
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: add_dif <absolute-input-file>\n";
        return 1;
    }

    const std::filesystem::path input_file(argv[1]);
    if (!input_file.is_absolute()) {
        std::cerr << "Error: input file path must be absolute: " << input_file
                  << '\n';
        return 1;
    }

    std::filesystem::path output_file = input_file;
    output_file.replace_extension(".out");

    std::ifstream input(input_file);
    if (!input) {
        std::cerr << "Error: cannot open " << input_file << '\n';
        return 1;
    }

    std::optional<long double> x1;
    std::optional<long double> x2;
    std::string line;
    std::size_t line_number = 0;

    while (std::getline(input, line)) {
        ++line_number;

        const std::string trimmed_line = Trim(line);
        if (trimmed_line.empty() || trimmed_line.front() == '#') {
            continue;
        }

        const auto equal_position = trimmed_line.find('=');
        if (equal_position == std::string::npos) {
            std::cerr << "Error: invalid format on line " << line_number
                      << "; expected name = value\n";
            return 1;
        }

        const std::string name = Trim(
            std::string_view(trimmed_line).substr(0, equal_position));
        const std::string value_text = Trim(
            std::string_view(trimmed_line).substr(equal_position + 1));

        long double value = 0.0L;
        if (!ParseValue(value_text, value)) {
            std::cerr << "Error: invalid number on line " << line_number << '\n';
            return 1;
        }

        std::optional<long double>* destination = nullptr;
        if (name == "x1") {
            destination = &x1;
        } else if (name == "x2") {
            destination = &x2;
        } else {
            std::cerr << "Error: unknown variable '" << name << "' on line "
                      << line_number << '\n';
            return 1;
        }

        if (destination->has_value()) {
            std::cerr << "Error: duplicate variable '" << name << "' on line "
                      << line_number << '\n';
            return 1;
        }
        *destination = value;
    }

    if (!x1.has_value() || !x2.has_value()) {
        std::cerr << "Error: both x1 and x2 must be defined in " << input_file
                  << '\n';
        return 1;
    }

    std::ofstream output(output_file);
    if (!output) {
        std::cerr << "Error: cannot create " << output_file << '\n';
        return 1;
    }

    output << std::setprecision(18);
    output << "y1 = " << (*x1 + *x2) << '\n';
    output << "y2 = " << (*x1 - *x2) << '\n';

    std::cout << "Wrote sum and difference to " << output_file << '\n';
    return 0;
}
