// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_create_delta.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 *
 * Tests for srcdiff_create_delta().
 */

#include <library_test.hpp>

#include <string>

using namespace library_test;

namespace {

const std::string ORIGINAL = "int f() {\n  return 0;\n}\n";
const std::string MODIFIED = "int f() {\n  return 1;\n}\n";

bool has_insert(const std::string& srcml) { return srcml.find("<diff:insert") != std::string::npos; }
bool has_delete(const std::string& srcml) { return srcml.find("<diff:delete") != std::string::npos; }

std::string delta_srcml(srcdiff_config* config, const archive& out,
                        const std::string& original, const std::string& modified,
                        const std::string& language = "C++",
                        const std::string& original_filename = "a.cpp",
                        const std::string& modified_filename = "b.cpp") {
    srcml_unit* original_unit = out.parse(original, language, original_filename);
    srcml_unit* modified_unit = out.parse(modified, language, modified_filename);
    srcml_unit* delta = srcdiff_create_delta(config, original_unit, modified_unit);
    std::string result = delta ? srcml_of(delta) : "<NULL>";
    if(delta) srcml_unit_free(delta);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
    return result;
}

}

TEST_CASE(identical_units_have_no_changes) {
    archive out;
    std::string srcml = delta_srcml(nullptr, out, ORIGINAL, ORIGINAL);
    REQUIRE(srcml != "<NULL>");
    CHECK(!has_insert(srcml));
    CHECK(!has_delete(srcml));
    CHECK(srcml.find("return") != std::string::npos);
}

TEST_CASE(modified_units_have_changes) {
    archive out;
    std::string srcml = delta_srcml(nullptr, out, ORIGINAL, MODIFIED);
    REQUIRE(srcml != "<NULL>");
    CHECK(has_insert(srcml));
    CHECK(has_delete(srcml));
}

TEST_CASE(null_config_matches_fresh_config) {
    for(const source_pair& pair : sample_pairs()) {
        archive out;
        std::string filename_original = std::string("a.") + pair.extension;
        std::string filename_modified = std::string("b.") + pair.extension;
        std::string with_null = delta_srcml(nullptr, out, pair.original, pair.modified, pair.language,
                                            filename_original, filename_modified);
        srcdiff_config* config = srcdiff_config_create();
        std::string with_config = delta_srcml(config, out, pair.original, pair.modified, pair.language,
                                              filename_original, filename_modified);
        srcdiff_config_free(config);
        CHECK_MESSAGE(with_null == with_config, std::string(pair.name) + ": NULL config differs from fresh config");
    }
}

TEST_CASE(delta_preserves_language) {
    for(const source_pair& pair : sample_pairs()) {
        archive out;
        srcml_unit* original_unit = out.parse(pair.original, pair.language, std::string("a.") + pair.extension);
        srcml_unit* modified_unit = out.parse(pair.modified, pair.language, std::string("b.") + pair.extension);
        srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
        CHECK_MESSAGE(delta != nullptr, std::string(pair.name) + ": delta is NULL");
        if(delta) {
            CHECK_EQUAL(quoted(srcml_unit_get_language(delta)), quoted(pair.language));
            srcml_unit_free(delta);
        }
        srcml_unit_free(modified_unit);
        srcml_unit_free(original_unit);
    }
}

TEST_CASE(delta_merges_different_filenames) {
    archive out;
    srcml_unit* original_unit = out.parse(ORIGINAL, "C++", "a.cpp");
    srcml_unit* modified_unit = out.parse(MODIFIED, "C++", "b.cpp");
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
    REQUIRE(delta);
    CHECK_EQUAL(quoted(srcml_unit_get_filename(delta)), quoted("a.cpp|b.cpp"));
    srcml_unit_free(delta);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
}

TEST_CASE(delta_keeps_same_filename_unmerged) {
    archive out;
    srcml_unit* original_unit = out.parse(ORIGINAL, "C++", "a.cpp");
    srcml_unit* modified_unit = out.parse(MODIFIED, "C++", "a.cpp");
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
    REQUIRE(delta);
    CHECK_EQUAL(quoted(srcml_unit_get_filename(delta)), quoted("a.cpp"));
    srcml_unit_free(delta);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
}

TEST_CASE(delta_merges_different_versions) {
    archive out;
    srcml_unit* original_unit = out.parse(ORIGINAL, "C++", "a.cpp", "1");
    srcml_unit* modified_unit = out.parse(MODIFIED, "C++", "a.cpp", "2");
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
    REQUIRE(delta);
    CHECK_EQUAL(quoted(srcml_unit_get_version(delta)), quoted("1|2"));
    srcml_unit_free(delta);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
}

TEST_CASE(both_units_null_returns_null) {
    CHECK(srcdiff_create_delta(nullptr, nullptr, nullptr) == nullptr);
    srcdiff_config* config = srcdiff_config_create();
    CHECK(srcdiff_create_delta(config, nullptr, nullptr) == nullptr);
    srcdiff_config_free(config);
}

TEST_CASE(null_original_is_pure_insert) {
    archive out;
    srcml_unit* modified_unit = out.parse(MODIFIED, "C++", "b.cpp");
    srcml_unit* delta = srcdiff_create_delta(nullptr, nullptr, modified_unit);
    REQUIRE(delta);
    std::string srcml = srcml_of(delta);
    CHECK(has_insert(srcml));
    CHECK(!has_delete(srcml));
    CHECK_EQUAL(quoted(srcml_unit_get_language(delta)), quoted("C++"));
    srcml_unit_free(delta);
    srcml_unit_free(modified_unit);
}

TEST_CASE(null_modified_is_pure_delete) {
    archive out;
    srcml_unit* original_unit = out.parse(ORIGINAL, "C++", "a.cpp");
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, nullptr);
    REQUIRE(delta);
    std::string srcml = srcml_of(delta);
    CHECK(has_delete(srcml));
    CHECK(!has_insert(srcml));
    CHECK_EQUAL(quoted(srcml_unit_get_language(delta)), quoted("C++"));
    srcml_unit_free(delta);
    srcml_unit_free(original_unit);
}

TEST_CASE(empty_sources_give_unchanged_delta) {
    archive out;
    std::string srcml = delta_srcml(nullptr, out, "", "");
    REQUIRE(srcml != "<NULL>");
    CHECK(!has_insert(srcml));
    CHECK(!has_delete(srcml));
}

TEST_CASE(empty_original_is_pure_insert) {
    archive out;
    std::string srcml = delta_srcml(nullptr, out, "", MODIFIED);
    REQUIRE(srcml != "<NULL>");
    CHECK(has_insert(srcml));
    CHECK(!has_delete(srcml));
}

TEST_CASE(empty_modified_is_pure_delete) {
    archive out;
    std::string srcml = delta_srcml(nullptr, out, ORIGINAL, "");
    REQUIRE(srcml != "<NULL>");
    CHECK(has_delete(srcml));
    CHECK(!has_insert(srcml));
}

TEST_CASE(unparsed_units_return_null) {
    archive out;
    srcml_unit* original_unit = srcml_unit_create(out.get());
    srcml_unit* modified_unit = srcml_unit_create(out.get());
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
    CHECK(delta == nullptr);
    if(delta) srcml_unit_free(delta);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
}

TEST_CASE(config_reuse_matches_fresh_config) {
    archive out;
    srcdiff_config* reused = srcdiff_config_create();
    for(const source_pair& pair : sample_pairs()) {
        std::string filename_original = std::string("a.") + pair.extension;
        std::string filename_modified = std::string("b.") + pair.extension;
        std::string with_reused = delta_srcml(reused, out, pair.original, pair.modified, pair.language,
                                              filename_original, filename_modified);
        srcdiff_config* fresh = srcdiff_config_create();
        std::string with_fresh = delta_srcml(fresh, out, pair.original, pair.modified, pair.language,
                                             filename_original, filename_modified);
        srcdiff_config_free(fresh);
        CHECK_MESSAGE(with_reused == with_fresh, std::string(pair.name) + ": reused config differs from fresh config");
    }
    srcdiff_config_free(reused);
}

TEST_CASE(config_reuse_is_stable) {
    archive out;
    srcdiff_config* config = srcdiff_config_create();
    const source_pair& first  = sample_pairs().at(0);
    const source_pair& second = sample_pairs().at(5);
    std::string before = delta_srcml(config, out, first.original, first.modified);
    delta_srcml(config, out, second.original, second.modified);
    std::string after  = delta_srcml(config, out, first.original, first.modified);
    CHECK(before == after);
    srcdiff_config_free(config);
}

TEST_CASE(config_reuse_after_null_result_does_not_return_previous_unit) {
    archive out;
    srcdiff_config* config = srcdiff_config_create();

    srcml_unit* original_unit = out.parse(ORIGINAL);
    srcml_unit* modified_unit = out.parse(MODIFIED);
    srcml_unit* first = srcdiff_create_delta(config, original_unit, modified_unit);
    REQUIRE(first);

    srcml_unit* empty_original = srcml_unit_create(out.get());
    srcml_unit* empty_modified = srcml_unit_create(out.get());
    srcml_unit* second = srcdiff_create_delta(config, empty_original, empty_modified);
    CHECK_MESSAGE(second != first, "second call returned the first call's unit again");
    CHECK(second == nullptr);

    if(second && second != first) srcml_unit_free(second);
    srcml_unit_free(first);
    srcml_unit_free(empty_modified);
    srcml_unit_free(empty_original);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
    srcdiff_config_free(config);
}

TEST_CASE(delta_does_not_modify_inputs) {
    archive out;
    srcml_unit* original_unit = out.parse(ORIGINAL, "C++", "a.cpp", "1");
    srcml_unit* modified_unit = out.parse(MODIFIED, "C++", "b.cpp", "2");
    std::string original_before = srcml_of(original_unit);
    std::string modified_before = srcml_of(modified_unit);
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
    REQUIRE(delta);
    CHECK_EQUAL(srcml_of(original_unit), original_before);
    CHECK_EQUAL(srcml_of(modified_unit), modified_before);
    CHECK_EQUAL(quoted(srcml_unit_get_filename(original_unit)), quoted("a.cpp"));
    CHECK_EQUAL(quoted(srcml_unit_get_filename(modified_unit)), quoted("b.cpp"));
    srcml_unit_free(delta);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
}

TEST_CASE(delta_outlives_inputs) {
    archive out;
    srcml_unit* original_unit = out.parse(ORIGINAL);
    srcml_unit* modified_unit = out.parse(MODIFIED);
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
    REQUIRE(delta);
    std::string srcml = srcml_of(delta);
    CHECK(has_insert(srcml));
    CHECK(has_delete(srcml));
    srcml_unit_free(delta);
}

TEST_CASE(delta_writes_to_solitary_archive) {
    archive out(true, true);
    srcml_unit* original_unit = out.parse(ORIGINAL);
    srcml_unit* modified_unit = out.parse(MODIFIED);
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
    REQUIRE(delta);
    CHECK(out.write(delta));
    srcml_unit_free(delta);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);

    std::string written = out.close();
    CHECK_EQUAL(std::to_string(count(written, "<unit ")), "1");
    CHECK(has_insert(written));
    CHECK(has_delete(written));
}

TEST_CASE(delta_writes_to_multi_unit_archive) {
    archive out;
    int written_units = 0;
    for(const source_pair& pair : sample_pairs()) {
        srcml_unit* original_unit = out.parse(pair.original, pair.language, std::string("a.") + pair.extension);
        srcml_unit* modified_unit = out.parse(pair.modified, pair.language, std::string("b.") + pair.extension);
        srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
        CHECK_MESSAGE(delta != nullptr, std::string(pair.name) + ": delta is NULL");
        if(delta) {
            CHECK_MESSAGE(out.write(delta), std::string(pair.name) + ": could not write delta");
            ++written_units;
            srcml_unit_free(delta);
        }
        srcml_unit_free(modified_unit);
        srcml_unit_free(original_unit);
    }
    std::string written = out.close();

    srcml_archive* in = srcml_archive_create();
    REQUIRE(srcml_archive_read_open_memory(in, written.c_str(), written.size()) == SRCML_STATUS_OK);
    int read_units = 0;
    while(srcml_unit* unit = srcml_archive_read_unit(in)) {
        ++read_units;
        srcml_unit_free(unit);
    }
    srcml_archive_close(in);
    srcml_archive_free(in);
    CHECK_EQUAL(std::to_string(read_units), std::to_string(written_units));
}

TEST_CASE(delta_declares_diff_namespace_without_archive_registration) {
    archive out(false, true);
    srcml_unit* original_unit = out.parse(ORIGINAL);
    srcml_unit* modified_unit = out.parse(MODIFIED);
    srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
    REQUIRE(delta);
    CHECK(out.write(delta));
    srcml_unit_free(delta);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);

    std::string written = out.close();
    CHECK(has_insert(written));
    CHECK_MESSAGE(written.find("xmlns:diff=\"http://www.srcML.org/srcDiff\"") != std::string::npos,
                  "written archive does not declare the diff namespace:\n" + written);
}
