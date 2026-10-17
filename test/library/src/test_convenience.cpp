// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_convenience.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 *
 * Tests for the srcDiff() convenience function.
 */

#include <library_test.hpp>

#include <string>

using namespace library_test;

namespace {

const std::string ORIGINAL = "int f() {\n  return 0;\n}\n";
const std::string MODIFIED = "int f() {\n  return 1;\n}\n";

struct archive_contents {
    int units = 0;
    std::string srcml;
    std::string original;
    std::string modified;
};

archive_contents read_output(const std::string& xml) {
    archive_contents contents;
    srcml_archive* in = srcml_archive_create();
    if(srcml_archive_read_open_memory(in, xml.c_str(), xml.size()) == SRCML_STATUS_OK) {
        while(srcml_unit* unit = srcml_archive_read_unit(in)) {
            if(contents.units++ == 0) {
                contents.srcml = srcml_of(unit);
                if(srcml_unit* original = srcdiff_read_unit_original(unit)) {
                    contents.original = source_of(original);
                    srcml_unit_free(original);
                }
                if(srcml_unit* modified = srcdiff_read_unit_modified(unit)) {
                    contents.modified = source_of(modified);
                    srcml_unit_free(modified);
                }
            }
            srcml_unit_free(unit);
        }
        srcml_archive_close(in);
    }
    srcml_archive_free(in);
    return contents;
}

}

TEST_CASE(null_filenames_are_an_error) {
    temp_dir dir;
    CHECK(srcDiff(nullptr, nullptr, dir.path("out.xml").c_str()) != SRCDIFF_STATUS_OK);
}

TEST_CASE(null_output_is_an_error) {
    temp_dir dir;
    write_file(dir.path("a.cpp"), ORIGINAL);
    write_file(dir.path("b.cpp"), MODIFIED);
    CHECK(srcDiff(dir.path("a.cpp").c_str(), dir.path("b.cpp").c_str(), nullptr) != SRCDIFF_STATUS_OK);
}

TEST_CASE(unknown_extension_is_an_error) {
    temp_dir dir;
    write_file(dir.path("a.unknown"), ORIGINAL);
    write_file(dir.path("b.unknown"), MODIFIED);
    CHECK(srcDiff(dir.path("a.unknown").c_str(), dir.path("b.unknown").c_str(), dir.path("out.xml").c_str())
          != SRCDIFF_STATUS_OK);
}

TEST_CASE(missing_input_is_an_error) {
    temp_dir dir;
    write_file(dir.path("b.cpp"), MODIFIED);
    CHECK(srcDiff(dir.path("missing.cpp").c_str(), dir.path("b.cpp").c_str(), dir.path("out.xml").c_str())
          != SRCDIFF_STATUS_OK);
}

TEST_CASE(writes_delta_to_output_file) {
    temp_dir dir;
    write_file(dir.path("a.cpp"), ORIGINAL);
    write_file(dir.path("b.cpp"), MODIFIED);
    CHECK(srcDiff(dir.path("a.cpp").c_str(), dir.path("b.cpp").c_str(), dir.path("out.xml").c_str())
          == SRCDIFF_STATUS_OK);

    std::string output = read_file(dir.path("out.xml"));
    REQUIRE(!output.empty());
    archive_contents contents = read_output(output);
    CHECK_EQUAL(std::to_string(contents.units), "1");
    CHECK(contents.srcml.find("<diff:insert") != std::string::npos);
    CHECK(contents.srcml.find("<diff:delete") != std::string::npos);
    CHECK_EQUAL(contents.original, ORIGINAL);
    CHECK_EQUAL(contents.modified, MODIFIED);
}

TEST_CASE(null_original_writes_pure_insert) {
    temp_dir dir;
    write_file(dir.path("b.cpp"), MODIFIED);
    CHECK(srcDiff(nullptr, dir.path("b.cpp").c_str(), dir.path("out.xml").c_str()) == SRCDIFF_STATUS_OK);

    std::string output = read_file(dir.path("out.xml"));
    REQUIRE(!output.empty());
    archive_contents contents = read_output(output);
    CHECK_EQUAL(std::to_string(contents.units), "1");
    CHECK(contents.srcml.find("<diff:insert") != std::string::npos);
    CHECK(contents.srcml.find("<diff:delete") == std::string::npos);
    CHECK_EQUAL(contents.modified, MODIFIED);
}

TEST_CASE(null_modified_writes_pure_delete) {
    temp_dir dir;
    write_file(dir.path("a.cpp"), ORIGINAL);
    CHECK(srcDiff(dir.path("a.cpp").c_str(), nullptr, dir.path("out.xml").c_str()) == SRCDIFF_STATUS_OK);

    std::string output = read_file(dir.path("out.xml"));
    REQUIRE(!output.empty());
    archive_contents contents = read_output(output);
    CHECK_EQUAL(std::to_string(contents.units), "1");
    CHECK(contents.srcml.find("<diff:delete") != std::string::npos);
    CHECK(contents.srcml.find("<diff:insert") == std::string::npos);
    CHECK_EQUAL(contents.original, ORIGINAL);
}
