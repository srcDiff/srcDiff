// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file library_test.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <library_test.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace library_test {

namespace {

std::vector<std::pair<std::string, test_function>>& registry() {
    static std::vector<std::pair<std::string, test_function>> tests;
    return tests;
}

int failure_count = 0;

}

registrar::registrar(const char* name, test_function function) {
    registry().emplace_back(name, function);
}

void report_failure(const char* file, int line, const std::string& message) {
    ++failure_count;
    std::cout << "  " << std::filesystem::path(file).filename().string() << ':' << line << ": " << message << '\n';
}

std::string quoted(const char* value) {
    return value ? std::string(value) : std::string("<NULL>");
}

archive::archive(bool register_diff_namespace, bool solitary) : handle(srcml_archive_create()) {
    if(solitary) srcml_archive_enable_solitary_unit(handle);
    srcml_archive_disable_hash(handle);
    if(register_diff_namespace) {
        srcml_archive_register_namespace(handle, "diff", "http://www.srcML.org/srcDiff");
    }
    srcml_archive_write_open_memory(handle, &buffer, &size);
}

archive::~archive() {
    close();
    srcml_archive_free(handle);
    srcml_memory_free(buffer);
}

bool archive::write(srcml_unit* unit) const {
    return srcml_archive_write_unit(handle, unit) == SRCML_STATUS_OK;
}

std::string archive::close() {
    if(!closed) {
        srcml_archive_close(handle);
        closed = true;
    }
    return buffer ? std::string(buffer, size) : std::string();
}

srcml_unit* archive::parse(const std::string& source, const std::string& language,
                           const std::string& filename, const char* version) const {
    srcml_unit* unit = srcml_unit_create(handle);
    srcml_unit_set_language(unit, language.c_str());
    srcml_unit_set_filename(unit, filename.c_str());
    if(version) srcml_unit_set_version(unit, version);
    if(srcml_unit_parse_memory(unit, source.c_str(), source.size()) != SRCML_STATUS_OK) {
        srcml_unit_free(unit);
        throw std::runtime_error("could not parse source with libsrcml");
    }
    return unit;
}

std::string source_of(srcml_unit* unit) {
    if(!unit) throw std::runtime_error("source_of: NULL unit");
    const char* source = srcml_unit_get_src(unit);
    if(!source) throw std::runtime_error("source_of: srcml_unit_get_src failed");
    return source;
}

std::string srcml_of(srcml_unit* unit) {
    const char* srcml = srcml_unit_get_srcml(unit);
    return srcml ? srcml : "";
}

std::string inner_srcml_of(srcml_unit* unit) {
    const char* srcml = srcml_unit_get_srcml_inner(unit);
    return srcml ? srcml : "";
}

int count(const std::string& haystack, const std::string& needle) {
    int total = 0;
    for(std::string::size_type pos = haystack.find(needle); pos != std::string::npos;
        pos = haystack.find(needle, pos + needle.size())) {
        ++total;
    }
    return total;
}

std::string read_file(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if(!in) return "";
    std::ostringstream contents;
    contents << in.rdbuf();
    return contents.str();
}

void write_file(const std::string& path, const std::string& contents) {
    std::ofstream out(path, std::ios::binary);
    out << contents;
}

temp_dir::temp_dir() {
    std::random_device device;
    std::filesystem::path base = std::filesystem::temp_directory_path();
    for(;;) {
        std::filesystem::path candidate = base / ("libsrcdiff-test-" + std::to_string(device()));
        if(std::filesystem::create_directory(candidate)) {
            root = candidate.string();
            return;
        }
    }
}

temp_dir::~temp_dir() {
    std::error_code ignored;
    std::filesystem::remove_all(root, ignored);
}

std::string temp_dir::path(const std::string& name) const {
    return (std::filesystem::path(root) / name).string();
}

const std::vector<source_pair>& sample_pairs() {
    static const std::vector<source_pair> pairs = {
        { "cpp_statement", "C++", "cpp",
          "int f() {\n  return 0;\n}\n",
          "int f() {\n  return 1;\n}\n" },
        { "cpp_insert_statement", "C++", "cpp",
          "void f() {\n  a();\n}\n",
          "void f() {\n  a();\n  b();\n}\n" },
        { "cpp_delete_statement", "C++", "cpp",
          "void f() {\n  a();\n  b();\n}\n",
          "void f() {\n  a();\n}\n" },
        { "cpp_wrap_in_if", "C++", "cpp",
          "void f() {\n  a();\n}\n",
          "void f() {\n  if(x) {\n    a();\n  }\n}\n" },
        { "cpp_for_to_while", "C++", "cpp",
          "void f() {\n  for(;i < n;) { g(); }\n}\n",
          "void f() {\n  while(i < n) { g(); }\n}\n" },
        // from libsrcdiff-method-bug.md
        { "cpp_grouped_changes", "C++", "cpp",
          "int f() {\n  if(a) return 1;\n  return 0;\n}\n",
          "int f() {\n  if(a && b) { log(); return 1; }\n  return 2;\n}\n" },
        { "cpp_whitespace_only", "C++", "cpp",
          "int  x = 1;\n",
          "int x  =  1;\n" },
        { "cpp_string_literal", "C++", "cpp",
          "const char* s = \"hello world\";\n",
          "const char* s = \"hello there world\";\n" },
        { "c_function", "C", "c",
          "int main(void) {\n  return 0;\n}\n",
          "int main(int argc, char** argv) {\n  return argc;\n}\n" },
        { "java_method", "Java", "java",
          "class A {\n  void f() { g(); }\n}\n",
          "class A {\n  void f() { g(); h(); }\n}\n" },
        { "csharp_property", "C#", "cs",
          "class A {\n  int X { get; set; }\n}\n",
          "class A {\n  public int X { get; private set; }\n}\n" },
    };
    return pairs;
}

}

int main(int argc, char* argv[]) {
    using library_test::registry;

    std::vector<std::string> selected;
    for(int pos = 1; pos < argc; ++pos) {
        std::string arg = argv[pos];
        if(arg == "--list") {
            for(const auto& test : registry()) std::cout << test.first << '\n';
            return 0;
        }
        selected.push_back(arg);
    }

    int run = 0;
    int failed = 0;
    for(const auto& test : registry()) {
        if(!selected.empty()) {
            bool found = false;
            for(const std::string& name : selected) found = found || name == test.first;
            if(!found) continue;
        }

        std::cout << test.first << '\n' << std::flush;
        int failures_before = library_test::failure_count;
        try {
            test.second();
        } catch(const std::exception& e) {
            library_test::report_failure("<exception>", 0, e.what());
        }
        ++run;
        bool passed = library_test::failure_count == failures_before;
        if(!passed) ++failed;
        std::cout << "  " << (passed ? "passed" : "FAILED") << '\n';
    }

    if(run == 0) {
        std::cerr << "No matching test cases\n";
        return 2;
    }

    std::cout << '\n' << (run - failed) << '/' << run << " passed\n";
    return failed == 0 ? 0 : 1;
}
