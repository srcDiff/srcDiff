// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_delta.cpp
 *
 * @copyright Copyright (C) 2014-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <delta_utils.hpp>

#include <srcdiff.h>
#include <srcml.h>

#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

namespace {

const char* const USAGE =
    "Usage: test_delta [options] <suite file>...\n"
    "\n"
    "Options:\n"
    "  -n, --case <number>  only run test case <number>\n"
    "  --dos                convert inputs to DOS (CRLF) line endings\n"
    "  --color              always color output\n"
    "  --no-color           never color output (default when stdout is not a terminal)\n"
    "  -h, --help           show this message\n";

const int FIELD_WIDTH_LANGUAGE = 12;
const int FIELD_WIDTH_URL      = 20;
const int FIELD_WIDTH_CASES    = 60;

const char* const GREEN = "\033[0;32m";
const char* const RED   = "\033[0;31m";
const char* const RESET = "\033[0m";

bool dos_line_endings = false;
bool use_color = false;

struct test_case_data {
    std::string url;
    std::string language;
    std::string filename;
    std::string expected;
};

struct failure {
    std::string suite_file;
    std::string url;
    std::string language;
    int number;
    std::string direction;
    std::string message;
};

std::string colored(const std::string& text, const char* color) {
    if(!use_color) return text;
    return color + text + RESET;
}

std::pair<std::string, std::string> split_versioned(const std::string& value) {
    std::string::size_type pos = value.find('|');
    if(pos == std::string::npos) return { value, value };
    return { value.substr(0, pos), value.substr(pos + 1) };
}

std::string unix_to_dos(const std::string& source) {
    std::string result;
    for(std::string::size_type pos = 0; pos < source.size(); ++pos) {
        if(source[pos] == '\n' && (pos == 0 || source[pos - 1] != '\r')) {
            result += '\r';
        }
        result += source[pos];
    }
    return result;
}

srcml_unit* parse_source(srcml_archive* archive, const std::string& source,
                         const std::string& language, const std::string& filename) {
    srcml_unit* unit = srcml_unit_create(archive);
    srcml_unit_set_language(unit, language.c_str());
    srcml_unit_set_filename(unit, filename.c_str());
    const std::string& text = dos_line_endings ? unix_to_dos(source) : source;
    if(srcml_unit_parse_memory(unit, text.c_str(), text.size()) != SRCML_STATUS_OK) {
        srcml_unit_free(unit);
        throw std::runtime_error("could not parse source with libsrcml");
    }
    return unit;
}

std::string create_delta(const std::string& original_source, const std::string& modified_source,
                         const std::string& language, const std::string& filename) {
    srcml_archive* archive = srcml_archive_create();
    srcml_archive_enable_solitary_unit(archive);
    srcml_archive_disable_hash(archive);
    srcml_archive_register_namespace(archive, "diff", "http://www.srcML.org/srcDiff");

    char* buffer = nullptr;
    size_t size = 0;
    srcml_archive_write_open_memory(archive, &buffer, &size);

    srcml_unit* original_unit = nullptr;
    srcml_unit* modified_unit = nullptr;
    srcdiff_config* config = nullptr;
    srcml_unit* delta = nullptr;
    std::string result;
    std::string error;
    try {
        original_unit = parse_source(archive, original_source, language, filename);
        modified_unit = parse_source(archive, modified_source, language, filename);

        config = srcdiff_config_create();
        delta = srcdiff_create_delta(config, original_unit, modified_unit);
        if(!delta) throw std::runtime_error("srcdiff_create_delta failed");

        result = serialize_unit(delta);
    } catch(const std::exception& e) {
        error = e.what();
    }

    if(delta) srcml_unit_free(delta);
    if(config) srcdiff_config_free(config);
    if(modified_unit) srcml_unit_free(modified_unit);
    if(original_unit) srcml_unit_free(original_unit);
    srcml_archive_close(archive);
    srcml_archive_free(archive);
    srcml_memory_free(buffer);

    if(!error.empty()) throw std::runtime_error(error);
    return result;
}

std::string first_difference(const std::string& actual, const std::string& expected) {
    std::istringstream actual_lines(actual);
    std::istringstream expected_lines(expected);
    std::string actual_line;
    std::string expected_line;
    for(int line = 1; ; ++line) {
        bool has_actual   = bool(std::getline(actual_lines, actual_line));
        bool has_expected = bool(std::getline(expected_lines, expected_line));
        if(!has_actual && !has_expected) return "no line difference";
        if(!has_actual || !has_expected || actual_line != expected_line) {
            return "first difference at line " + std::to_string(line) + ":\n"
                 + "- " + (has_expected ? expected_line : "<eof>") + "\n"
                 + "+ " + (has_actual ? actual_line : "<eof>");
        }
    }
}

std::string check_delta(const std::string& original_source, const std::string& modified_source,
                        const std::string& language, const std::string& filename,
                        const std::string& expected) {
    try {
        std::string actual = create_delta(original_source, modified_source, language, filename);
        if(actual == expected) return "";
        return first_difference(actual, expected);
    } catch(const std::exception& e) {
        return std::string("error: ") + e.what();
    }
}

std::string test_forward(const test_case_data& data) {
    try {
        std::string original_source = extract_revision_source(data.expected, 0);
        std::string modified_source = extract_revision_source(data.expected, 1);
        return check_delta(original_source, modified_source, data.language, data.filename, data.expected);
    } catch(const std::exception& e) {
        return std::string("error: ") + e.what();
    }
}

std::string test_reverse(const test_case_data& data) {
    try {
        std::string original_source = extract_revision_source(data.expected, 1);
        std::string modified_source = extract_revision_source(data.expected, 0);
        std::pair<std::string, std::string> filenames = split_versioned(data.filename);
        return check_delta(original_source, modified_source, data.language,
                           filenames.second + '|' + filenames.first, switch_differences(data.expected));
    } catch(const std::exception& e) {
        return std::string("error: ") + e.what();
    }
}

class test_runner {
public:
    explicit test_runner(int only_case) : only_case(only_case) {}

    bool run_suite(const std::string& suite_filename) {
        srcml_archive* archive = srcml_archive_create();
        if(srcml_archive_read_open_filename(archive, suite_filename.c_str()) != SRCML_STATUS_OK) {
            std::cerr << "Could not read file: " << suite_filename << '\n';
            srcml_archive_free(archive);
            return false;
        }

        const char* url = srcml_archive_get_url(archive);
        test_case_data archive_data;
        archive_data.url = url ? url : "";

        bool is_swappable = archive_data.url != "interchange" && archive_data.url != "super_list";

        int line_width = 0;
        bool printed_header = false;
        int count = 0;
        while(srcml_unit* unit = srcml_archive_read_unit(archive)) {
            ++count;
            if(only_case != 0 && count != only_case) {
                srcml_unit_free(unit);
                continue;
            }

            test_case_data data = archive_data;
            // libsrcml doesn't report a read archive's language
            const char* language = srcml_unit_get_language(unit);
            data.language = language && *language ? language : "C++";
            const char* filename = srcml_unit_get_filename(unit);
            data.filename = filename ? filename : "";
            data.expected = serialize_unit(unit);
            srcml_unit_free(unit);

            if(!printed_header) {
                std::cout << std::left << std::setw(FIELD_WIDTH_LANGUAGE) << data.language << ' '
                          << std::setw(FIELD_WIDTH_URL) << data.url << ' ' << std::flush;
                printed_header = true;
            }

            std::vector<std::pair<std::string, std::string>> results;
            results.emplace_back("forward", test_forward(data));
            if(is_swappable) {
                results.emplace_back("reverse", test_reverse(data));
            }

            bool passed = true;
            for(const std::pair<std::string, std::string>& result : results) {
                if(result.second.empty()) continue;
                passed = false;
                failures.push_back({ suite_filename, data.url, data.language, count, result.first, result.second });
            }

            ++total_count;
            if(!passed) ++failed_count;

            std::string number = std::to_string(count);
            if(line_width + int(number.size()) + 1 > FIELD_WIDTH_CASES) {
                std::cout << '\n' << std::string(FIELD_WIDTH_LANGUAGE + 1 + FIELD_WIDTH_URL + 1, ' ');
                line_width = 0;
            }
            std::cout << colored(number, passed ? GREEN : RED) << ' ' << std::flush;
            line_width += int(number.size()) + 1;
        }

        if(printed_header) std::cout << '\n';

        srcml_archive_close(archive);
        srcml_archive_free(archive);

        return true;
    }

    void print_report() const {
        std::cout << "\nReport:\n\n";
        if(failed_count == 0) {
            std::cout << colored("No errors out of " + std::to_string(total_count)
                                 + (total_count == 1 ? " case" : " cases"), GREEN) << '\n';
            return;
        }

        std::cout << colored("Errors: " + std::to_string(failed_count) + " out of " + std::to_string(total_count)
                             + (total_count == 1 ? " case" : " cases"), RED) << "\n\n";

        for(std::vector<failure>::size_type pos = 0; pos < failures.size(); ) {
            const failure& first = failures[pos];

            std::vector<std::vector<failure>::size_type> same_case;
            for(; pos < failures.size()
                  && failures[pos].suite_file == first.suite_file
                  && failures[pos].number == first.number; ++pos) {
                same_case.push_back(pos);
            }

            std::string directions;
            for(std::vector<failure>::size_type index : same_case) {
                if(!directions.empty()) directions += ", ";
                directions += failures[index].direction;
            }

            std::cout << colored(first.language + ' ' + first.url + ' ' + std::to_string(first.number), RED)
                      << " (" << directions << ")  [" << first.suite_file << "]\n";
            for(std::vector<failure>::size_type index : same_case) {
                std::cout << "  " << failures[index].direction << ": " << failures[index].message << '\n';
            }
            std::cout << '\n';
        }
    }

    bool all_passed() const { return failed_count == 0; }

private:
    int only_case;
    int total_count = 0;
    int failed_count = 0;
    std::vector<failure> failures;
};

}

int main(int argc, char* argv[]) {
    use_color = isatty(fileno(stdout));

    int only_case = 0;
    std::vector<std::string> suite_filenames;
    for(int pos = 1; pos < argc; ++pos) {
        std::string arg = argv[pos];
        if(arg == "-h" || arg == "--help") {
            std::cout << USAGE;
            return 0;
        } else if(arg == "--dos") {
            dos_line_endings = true;
        } else if(arg == "--color") {
            use_color = true;
        } else if(arg == "--no-color") {
            use_color = false;
        } else if(arg == "-n" || arg == "--case") {
            if(++pos == argc) {
                std::cerr << "Missing number for " << arg << "\n\n" << USAGE;
                return 2;
            }
            only_case = std::atoi(argv[pos]);
            if(only_case <= 0) {
                std::cerr << "Invalid test case number: " << argv[pos] << '\n';
                return 2;
            }
        } else if(arg == "--") {
            // no-op
        } else if(!arg.empty() && arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n\n" << USAGE;
            return 2;
        } else {
            suite_filenames.push_back(arg);
        }
    }

    if(suite_filenames.empty()) {
        std::cerr << USAGE;
        return 2;
    }

    test_runner runner(only_case);
    bool read_all = true;
    for(const std::string& suite_filename : suite_filenames) {
        if(!runner.run_suite(suite_filename)) read_all = false;
    }

    runner.print_report();

    return runner.all_passed() && read_all ? 0 : 1;
}
