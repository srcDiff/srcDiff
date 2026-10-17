// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file library_test.hpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 *
 * Framework and helpers for libsrcdiff test suite.
 */

#ifndef INCLUDED_LIBRARY_TEST_HPP
#define INCLUDED_LIBRARY_TEST_HPP

#include <srcdiff.h>
#include <srcml.h>

#include <string>
#include <vector>

namespace library_test {

using test_function = void (*)();

struct registrar {
    registrar(const char* name, test_function function);
};

void report_failure(const char* file, int line, const std::string& message);

std::string quoted(const char* value);

class archive {
public:
    explicit archive(bool register_diff_namespace = true, bool solitary = false);
    ~archive();
    archive(const archive&) = delete;
    archive& operator=(const archive&) = delete;

    srcml_archive* get() const { return handle; }

    srcml_unit* parse(const std::string& source, const std::string& language = "C++",
                      const std::string& filename = "a.cpp", const char* version = nullptr) const;

    bool write(srcml_unit* unit) const;

    std::string close();

private:
    srcml_archive* handle;
    char* buffer = nullptr;
    size_t size = 0;
    bool closed = false;
};

std::string source_of(srcml_unit* unit);

std::string srcml_of(srcml_unit* unit);

std::string inner_srcml_of(srcml_unit* unit);

int count(const std::string& haystack, const std::string& needle);

std::string read_file(const std::string& path);

void write_file(const std::string& path, const std::string& contents);

class temp_dir {
public:
    temp_dir();
    ~temp_dir();
    temp_dir(const temp_dir&) = delete;
    temp_dir& operator=(const temp_dir&) = delete;

    std::string path(const std::string& name) const;

private:
    std::string root;
};

struct source_pair {
    const char* name;
    const char* language;
    const char* extension;
    const char* original;
    const char* modified;
};

const std::vector<source_pair>& sample_pairs();

}

#define LIBRARY_TEST_CONCAT_IMPL(a, b) a##b
#define LIBRARY_TEST_CONCAT(a, b) LIBRARY_TEST_CONCAT_IMPL(a, b)

#define TEST_CASE(name) \
    static void name(); \
    static ::library_test::registrar LIBRARY_TEST_CONCAT(name, _registrar)(#name, &name); \
    static void name()

#define CHECK(condition) \
    do { \
        if(!(condition)) ::library_test::report_failure(__FILE__, __LINE__, "CHECK(" #condition ") failed"); \
    } while(false)

#define CHECK_MESSAGE(condition, message) \
    do { \
        if(!(condition)) ::library_test::report_failure(__FILE__, __LINE__, std::string(message)); \
    } while(false)

#define CHECK_EQUAL(actual, expected) \
    do { \
        const std::string library_test_actual   = (actual); \
        const std::string library_test_expected = (expected); \
        if(library_test_actual != library_test_expected) { \
            ::library_test::report_failure(__FILE__, __LINE__, \
                "CHECK_EQUAL(" #actual ", " #expected ") failed\n    actual:   \"" + library_test_actual \
                + "\"\n    expected: \"" + library_test_expected + "\""); \
        } \
    } while(false)

#define REQUIRE(condition) \
    do { \
        if(!(condition)) { \
            ::library_test::report_failure(__FILE__, __LINE__, "REQUIRE(" #condition ") failed"); \
            return; \
        } \
    } while(false)

#endif
