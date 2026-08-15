// A minimal test solver used by the delimited_text and placeholder_text
// fixtures. Reads "name = value" lines from an absolute input file, sums all
// values, and writes "y = <sum>" to the sibling <input>.out file.
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: add_solver <absolute-input-file>\n";
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

    long double sum = 0.0L;
    std::string line;
    std::size_t line_number = 0;
    std::size_t value_count = 0;

    while (std::getline(input, line)) {
        ++line_number;

        const auto first_non_space = line.find_first_not_of(" \t\r\n");
        if (first_non_space == std::string::npos || line[first_non_space] == '#') {
            continue;
        }

        const auto equal_position = line.find('=');
        if (equal_position == std::string::npos) {
            std::cerr << "Error: invalid format on line " << line_number
                      << "; expected name = value\n";
            return 1;
        }

        std::istringstream value_stream(line.substr(equal_position + 1));
        long double value = 0.0L;
        if (!(value_stream >> value)) {
            std::cerr << "Error: invalid number on line " << line_number << '\n';
            return 1;
        }

        value_stream >> std::ws;
        if (!value_stream.eof()) {
            std::cerr << "Error: unexpected content after the number on line "
                      << line_number << '\n';
            return 1;
        }

        sum += value;
        ++value_count;
    }

    if (value_count == 0) {
        std::cerr << "Error: no values found in " << input_file << '\n';
        return 1;
    }

    std::ofstream output(output_file);
    if (!output) {
        std::cerr << "Error: cannot create " << output_file << '\n';
        return 1;
    }

    output << "y = " << std::setprecision(18) << sum << '\n';
    std::cout << "Wrote sum of " << value_count << " values to " << output_file
              << '\n';
    return 0;
}
