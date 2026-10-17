// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_read_unit.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 *
 * Tests for srcdiff_read_unit_original(), srcdiff_read_unit_modified(), and
 * srcdiff_read_unit_revision().
 */

#include <library_test.hpp>
#include <delta_utils.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace library_test;

namespace {

const std::string ORIGINAL = "int f() {\n  return 0;\n}\n";
const std::string MODIFIED = "int f() {\n  return 1;\n}\n";

srcml_unit* create_delta(const archive& out, const std::string& original, const std::string& modified,
                         const std::string& language = "C++",
                         const std::string& original_filename = "a.cpp",
                         const std::string& modified_filename = "b.cpp",
                         const char* original_version = nullptr, const char* modified_version = nullptr) {
    srcml_unit* original_unit = out.parse(original, language, original_filename, original_version);
    srcml_unit* modified_unit = out.parse(modified, language, modified_filename, modified_version);
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
    return delta;
}

class read_archive {
public:
    explicit read_archive(const std::string& xml) : contents(xml), handle(srcml_archive_create()) {
        srcml_archive_read_open_memory(handle, contents.c_str(), contents.size());
    }
    ~read_archive() {
        srcml_archive_close(handle);
        srcml_archive_free(handle);
    }
    srcml_unit* read_unit() { return srcml_archive_read_unit(handle); }

private:
    std::string contents;
    srcml_archive* handle;
};

}

TEST_CASE(read_original_round_trips_source) {
    for(const source_pair& pair : sample_pairs()) {
        archive out;
        srcml_unit* delta = create_delta(out, pair.original, pair.modified, pair.language,
                                         std::string("a.") + pair.extension, std::string("b.") + pair.extension);
        CHECK_MESSAGE(delta != nullptr, std::string(pair.name) + ": delta is NULL");
        if(!delta) continue;
        srcml_unit* original = srcdiff_read_unit_original(delta);
        CHECK_MESSAGE(original != nullptr, std::string(pair.name) + ": original is NULL");
        if(original) {
            std::string source = source_of(original);
            CHECK_MESSAGE(source == pair.original, std::string(pair.name) + ": original source differs\n    actual:   \""
                          + source + "\"\n    expected: \"" + pair.original + "\"");
            srcml_unit_free(original);
        }
        srcml_unit_free(delta);
    }
}

TEST_CASE(read_modified_round_trips_source) {
    for(const source_pair& pair : sample_pairs()) {
        archive out;
        srcml_unit* delta = create_delta(out, pair.original, pair.modified, pair.language,
                                         std::string("a.") + pair.extension, std::string("b.") + pair.extension);
        CHECK_MESSAGE(delta != nullptr, std::string(pair.name) + ": delta is NULL");
        if(!delta) continue;
        srcml_unit* modified = srcdiff_read_unit_modified(delta);
        CHECK_MESSAGE(modified != nullptr, std::string(pair.name) + ": modified is NULL");
        if(modified) {
            std::string source = source_of(modified);
            CHECK_MESSAGE(source == pair.modified, std::string(pair.name) + ": modified source differs\n    actual:   \""
                          + source + "\"\n    expected: \"" + pair.modified + "\"");
            srcml_unit_free(modified);
        }
        srcml_unit_free(delta);
    }
}

TEST_CASE(read_round_trips_markup) {
    for(const source_pair& pair : sample_pairs()) {
        archive out;
        std::string original_filename = std::string("a.") + pair.extension;
        std::string modified_filename = std::string("b.") + pair.extension;
        srcml_unit* original_parsed = out.parse(pair.original, pair.language, original_filename);
        srcml_unit* modified_parsed = out.parse(pair.modified, pair.language, modified_filename);
        srcml_unit* delta = srcdiff_create_delta(nullptr, original_parsed, modified_parsed);
        CHECK_MESSAGE(delta != nullptr, std::string(pair.name) + ": delta is NULL");
        if(delta) {
            srcml_unit* original = srcdiff_read_unit_original(delta);
            srcml_unit* modified = srcdiff_read_unit_modified(delta);
            if(original) {
                CHECK_MESSAGE(inner_srcml_of(original) == inner_srcml_of(original_parsed),
                              std::string(pair.name) + ": original markup differs\n    actual:   "
                              + inner_srcml_of(original) + "\n    expected: " + inner_srcml_of(original_parsed));
                srcml_unit_free(original);
            }
            if(modified) {
                CHECK_MESSAGE(inner_srcml_of(modified) == inner_srcml_of(modified_parsed),
                              std::string(pair.name) + ": modified markup differs\n    actual:   "
                              + inner_srcml_of(modified) + "\n    expected: " + inner_srcml_of(modified_parsed));
                srcml_unit_free(modified);
            }
            srcml_unit_free(delta);
        }
        srcml_unit_free(modified_parsed);
        srcml_unit_free(original_parsed);
    }
}

TEST_CASE(read_revision_matches_original_and_modified) {
    archive out;
    srcml_unit* delta = create_delta(out, ORIGINAL, MODIFIED);
    REQUIRE(delta);
    srcml_unit* original  = srcdiff_read_unit_original(delta);
    srcml_unit* revision0 = srcdiff_read_unit_revision(delta, SRCDIFF_ORIGINAL);
    srcml_unit* modified  = srcdiff_read_unit_modified(delta);
    srcml_unit* revision1 = srcdiff_read_unit_revision(delta, SRCDIFF_MODIFIED);
    REQUIRE(original && revision0 && modified && revision1);
    CHECK_EQUAL(srcml_of(revision0), srcml_of(original));
    CHECK_EQUAL(srcml_of(revision1), srcml_of(modified));
    srcml_unit_free(revision1);
    srcml_unit_free(modified);
    srcml_unit_free(revision0);
    srcml_unit_free(original);
    srcml_unit_free(delta);
}

TEST_CASE(read_revision_out_of_range_returns_null) {
    archive out;
    srcml_unit* delta = create_delta(out, ORIGINAL, MODIFIED);
    REQUIRE(delta);
    for(size_t revision : { size_t(2), size_t(100), size_t(-1) }) {
        srcml_unit* unit = srcdiff_read_unit_revision(delta, revision);
        CHECK_MESSAGE(unit == nullptr, "revision " + std::to_string(revision) + " did not return NULL");
        if(unit) srcml_unit_free(unit);
    }
    srcml_unit_free(delta);
}

TEST_CASE(read_null_returns_null) {
    CHECK(srcdiff_read_unit_original(nullptr) == nullptr);
    CHECK(srcdiff_read_unit_modified(nullptr) == nullptr);
    CHECK(srcdiff_read_unit_revision(nullptr, SRCDIFF_ORIGINAL) == nullptr);
    CHECK(srcdiff_read_unit_revision(nullptr, SRCDIFF_MODIFIED) == nullptr);
}

TEST_CASE(read_unparsed_unit_returns_null) {
    archive out;
    srcml_unit* unit = srcml_unit_create(out.get());
    srcml_unit* original = srcdiff_read_unit_original(unit);
    CHECK(original == nullptr);
    if(original) srcml_unit_free(original);
    srcml_unit_free(unit);
}

TEST_CASE(read_plain_srcml_is_unchanged) {
    archive out;
    srcml_unit* plain = out.parse(ORIGINAL);
    for(size_t revision : { size_t(SRCDIFF_ORIGINAL), size_t(SRCDIFF_MODIFIED) }) {
        srcml_unit* unit = srcdiff_read_unit_revision(plain, revision);
        CHECK(unit != nullptr);
        if(!unit) continue;
        CHECK_EQUAL(source_of(unit), ORIGINAL);
        CHECK_EQUAL(inner_srcml_of(unit), inner_srcml_of(plain));
        srcml_unit_free(unit);
    }
    srcml_unit_free(plain);
}

TEST_CASE(read_keeps_language) {
    for(const source_pair& pair : sample_pairs()) {
        archive out;
        srcml_unit* delta = create_delta(out, pair.original, pair.modified, pair.language,
                                         std::string("a.") + pair.extension, std::string("b.") + pair.extension);
        if(!delta) continue;
        srcml_unit* original = srcdiff_read_unit_original(delta);
        srcml_unit* modified = srcdiff_read_unit_modified(delta);
        if(original) CHECK_EQUAL(quoted(srcml_unit_get_language(original)), quoted(pair.language));
        if(modified) CHECK_EQUAL(quoted(srcml_unit_get_language(modified)), quoted(pair.language));
        if(modified) srcml_unit_free(modified);
        if(original) srcml_unit_free(original);
        srcml_unit_free(delta);
    }
}

TEST_CASE(read_splits_filename) {
    archive out;
    srcml_unit* delta = create_delta(out, ORIGINAL, MODIFIED, "C++", "a.cpp", "b.cpp");
    REQUIRE(delta);
    srcml_unit* original = srcdiff_read_unit_original(delta);
    srcml_unit* modified = srcdiff_read_unit_modified(delta);
    REQUIRE(original && modified);
    CHECK_EQUAL(quoted(srcml_unit_get_filename(original)), quoted("a.cpp"));
    CHECK_EQUAL(quoted(srcml_unit_get_filename(modified)), quoted("b.cpp"));
    srcml_unit_free(modified);
    srcml_unit_free(original);
    srcml_unit_free(delta);
}

TEST_CASE(read_splits_version) {
    archive out;
    srcml_unit* delta = create_delta(out, ORIGINAL, MODIFIED, "C++", "a.cpp", "a.cpp", "1", "2");
    REQUIRE(delta);
    srcml_unit* original = srcdiff_read_unit_original(delta);
    srcml_unit* modified = srcdiff_read_unit_modified(delta);
    REQUIRE(original && modified);
    CHECK_EQUAL(quoted(srcml_unit_get_version(original)), quoted("1"));
    CHECK_EQUAL(quoted(srcml_unit_get_version(modified)), quoted("2"));
    srcml_unit_free(modified);
    srcml_unit_free(original);
    srcml_unit_free(delta);
}

TEST_CASE(read_preserves_attribute_namespaces) {
    const std::string xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
        "<unit xmlns=\"http://www.srcML.org/srcML/src\" xmlns:diff=\"http://www.srcML.org/srcDiff\""
        " xmlns:pos=\"http://www.srcML.org/srcML/position\" revision=\"1.0.0\" language=\"C++\""
        " filename=\"a.cpp\" pos:tabs=\"8\"><expr_stmt><expr><literal type=\"number\" pos:start=\"1:1\""
        " pos:end=\"1:1\">1</literal></expr>;</expr_stmt>\n</unit>\n";
    read_archive in(xml);
    srcml_unit* unit = in.read_unit();
    REQUIRE(unit);
    srcml_unit* original = srcdiff_read_unit_original(unit);
    REQUIRE(original);
    std::string srcml = srcml_of(original);
    CHECK_MESSAGE(srcml.find("<literal type=\"number\" pos:start=\"1:1\" pos:end=\"1:1\">1</literal>") != std::string::npos,
                  "namespaced attributes not preserved:\n" + srcml);
    CHECK_MESSAGE(count(srcml, "xmlns:pos=") <= 1, "pos namespace redeclared:\n" + srcml);
    srcml_unit_free(original);
    srcml_unit_free(unit);
}

TEST_CASE(read_result_outlives_delta) {
    archive out;
    srcml_unit* delta = create_delta(out, ORIGINAL, MODIFIED);
    REQUIRE(delta);
    srcml_unit* original = srcdiff_read_unit_original(delta);
    srcml_unit_free(delta);
    REQUIRE(original);
    CHECK_EQUAL(source_of(original), ORIGINAL);
    srcml_unit_free(original);
}

TEST_CASE(read_from_read_archive) {
    archive out(true, true);
    srcml_unit* delta = create_delta(out, ORIGINAL, MODIFIED);
    REQUIRE(delta);
    CHECK(out.write(delta));
    srcml_unit_free(delta);

    read_archive in(out.close());
    srcml_unit* unit = in.read_unit();
    REQUIRE(unit);
    srcml_unit* original = srcdiff_read_unit_original(unit);
    srcml_unit* modified = srcdiff_read_unit_modified(unit);
    CHECK(original != nullptr);
    CHECK(modified != nullptr);
    if(original) CHECK_EQUAL(source_of(original), ORIGINAL);
    if(modified) CHECK_EQUAL(source_of(modified), MODIFIED);
    if(modified) srcml_unit_free(modified);
    if(original) srcml_unit_free(original);
    srcml_unit_free(unit);
}

TEST_CASE(read_delta_suite_round_trips) {
    std::vector<std::filesystem::path> suite_files;
    for(const auto& entry : std::filesystem::directory_iterator(DELTA_SUITE_DIR)) {
        if(entry.path().extension() == ".xml") suite_files.push_back(entry.path());
    }
    std::sort(suite_files.begin(), suite_files.end());
    REQUIRE(!suite_files.empty());

    const int MAX_REPORTED = 10;
    int total = 0;
    int failed = 0;
    for(const std::filesystem::path& suite_file : suite_files) {
        srcml_archive* in = srcml_archive_create();
        if(srcml_archive_read_open_filename(in, suite_file.string().c_str()) != SRCML_STATUS_OK) {
            CHECK_MESSAGE(false, "could not read " + suite_file.string());
            srcml_archive_free(in);
            continue;
        }
        int number = 0;
        while(srcml_unit* unit = srcml_archive_read_unit(in)) {
            ++number;
            std::string expected_srcml = serialize_unit(unit);
            for(int revision : { 0, 1 }) {
                ++total;
                std::string message;
                try {
                    std::string expected = extract_revision_source(expected_srcml, revision);
                    srcml_unit* read = srcdiff_read_unit_revision(unit, revision);
                    if(!read) {
                        message = "NULL";
                    } else {
                        std::string actual = source_of(read);
                        if(actual != expected) message = "\n    actual:   \"" + actual + "\"\n    expected: \"" + expected + "\"";
                        srcml_unit_free(read);
                    }
                } catch(const std::exception& e) {
                    message = std::string("error: ") + e.what();
                }
                if(message.empty()) continue;
                ++failed;
                if(failed <= MAX_REPORTED) {
                    CHECK_MESSAGE(false, suite_file.filename().string() + " case " + std::to_string(number)
                                  + " revision " + std::to_string(revision) + ": " + message);
                }
            }
            srcml_unit_free(unit);
        }
        srcml_archive_close(in);
        srcml_archive_free(in);
    }
    std::cout << "  checked " << total << " revisions from " << suite_files.size() << " suite files\n";
    CHECK(total > 0);
    CHECK_MESSAGE(failed == 0, std::to_string(failed) + " of " + std::to_string(total) + " revisions differ");
}
