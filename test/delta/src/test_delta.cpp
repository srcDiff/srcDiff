// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_delta.cpp
 *
 * @copyright Copyright (C) 2014-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include <delta_utils.hpp>

#include <srcdiff.h>
#include <srcml.h>

#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace utf = boost::unit_test;

namespace {

bool dos_line_endings = false;

struct test_case_data {
    std::string url;
    std::string language;
    std::string filename;
    std::string expected;
};

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
    BOOST_REQUIRE(srcml_unit_parse_memory(unit, text.c_str(), text.size()) == SRCML_STATUS_OK);
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

    // like --filename, so the merged attribute stays X|X
    srcml_unit* original_unit = parse_source(archive, original_source, language, filename);
    srcml_unit* modified_unit = parse_source(archive, modified_source, language, filename);

    srcdiff_config* config = srcdiff_config_create();
    srcml_unit* delta = srcdiff_create_delta(config, original_unit, modified_unit);
    BOOST_REQUIRE(delta);

    std::string result = serialize_unit(delta);

    srcml_unit_free(delta);
    srcdiff_config_free(config);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
    srcml_archive_close(archive);
    srcml_archive_free(archive);
    srcml_memory_free(buffer);

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
            return "line " + std::to_string(line) + ":\n- " + (has_expected ? expected_line : "<eof>")
                 + "\n+ " + (has_actual ? actual_line : "<eof>");
        }
    }
}

void check_delta(const std::string& original_source, const std::string& modified_source,
                 const std::string& language, const std::string& filename,
                 const std::string& expected) {
    std::string actual = create_delta(original_source, modified_source, language, filename);
    BOOST_TEST_INFO("first difference at " << first_difference(actual, expected));
    BOOST_TEST(actual == expected);
}

void test_forward(std::shared_ptr<const test_case_data> data) {
    std::string original_source = extract_revision_source(data->expected, 0);
    std::string modified_source = extract_revision_source(data->expected, 1);
    check_delta(original_source, modified_source, data->language, data->filename, data->expected);
}

void test_reverse(std::shared_ptr<const test_case_data> data) {
    std::string original_source = extract_revision_source(data->expected, 1);
    std::string modified_source = extract_revision_source(data->expected, 0);
    std::pair<std::string, std::string> filenames = split_versioned(data->filename);
    check_delta(original_source, modified_source, data->language,
                filenames.second + '|' + filenames.first, switch_differences(data->expected));
}

void add_test(const std::string& name, std::function<void()> test) {
    utf::framework::master_test_suite().add(utf::make_test_case(test, name, __FILE__, __LINE__));
}

}

bool init_unit_test() {
    utf::master_test_suite_t& master = utf::framework::master_test_suite();

    std::string suite_filename;
    for(int pos = 1; pos < master.argc; ++pos) {
        std::string arg = master.argv[pos];
        if(arg == "--dos") {
            dos_line_endings = true;
        } else {
            suite_filename = arg;
        }
    }

    if(suite_filename.empty()) {
        std::cerr << "Usage: test_delta [boost test options] -- <suite file> [--dos]\n";
        return false;
    }

    srcml_archive* archive = srcml_archive_create();
    if(srcml_archive_read_open_filename(archive, suite_filename.c_str()) != SRCML_STATUS_OK) {
        std::cerr << "Could not read file: " << suite_filename << '\n';
        srcml_archive_free(archive);
        return false;
    }

    const char* url = srcml_archive_get_url(archive);

    test_case_data archive_data;
    archive_data.url = url ? url : "";
    master.p_name.value = archive_data.url;

    bool is_swappable = archive_data.url != "interchange" && archive_data.url != "super_list";
    bool is_interchange = archive_data.url == "interchange";

    int count = 0;
    while(srcml_unit* unit = srcml_archive_read_unit(archive)) {
        ++count;

        std::shared_ptr<test_case_data> data = std::make_shared<test_case_data>(archive_data);
        // libsrcml doesn't report a read archive's language
        const char* language = srcml_unit_get_language(unit);
        data->language = language && *language ? language : "C++";
        const char* filename = srcml_unit_get_filename(unit);
        data->filename = filename ? filename : "";
        data->expected = serialize_unit(unit);
        srcml_unit_free(unit);

        int test_number = is_interchange ? count : count * 2 - 1;
        add_test(std::to_string(test_number), [data]() { test_forward(data); });
        if(is_swappable) {
            add_test(std::to_string(count * 2), [data]() { test_reverse(data); });
        }
    }

    srcml_archive_close(archive);
    srcml_archive_free(archive);

    return true;
}
