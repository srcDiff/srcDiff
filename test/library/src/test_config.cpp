// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_config.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 *
 * Tests for srcdiff_config lifetime and independent use of separate configs.
 */

#include <library_test.hpp>

#include <string>
#include <thread>
#include <vector>

using namespace library_test;

TEST_CASE(config_create_returns_config) {
    srcdiff_config* config = srcdiff_config_create();
    CHECK(config != nullptr);
    srcdiff_config_free(config);
}

TEST_CASE(config_free_null_is_safe) {
    srcdiff_config_free(nullptr);
}

TEST_CASE(config_create_free_repeatedly) {
    for(int i = 0; i < 100; ++i) {
        srcdiff_config* config = srcdiff_config_create();
        CHECK(config != nullptr);
        srcdiff_config_free(config);
    }
}

TEST_CASE(config_free_after_deltas) {
    archive out;
    srcdiff_config* config = srcdiff_config_create();
    std::vector<srcml_unit*> deltas;
    for(const source_pair& pair : sample_pairs()) {
        srcml_unit* original_unit = out.parse(pair.original, pair.language, std::string("a.") + pair.extension);
        srcml_unit* modified_unit = out.parse(pair.modified, pair.language, std::string("b.") + pair.extension);
        deltas.push_back(srcdiff_create_delta(config, original_unit, modified_unit));
        srcml_unit_free(modified_unit);
        srcml_unit_free(original_unit);
    }
    srcdiff_config_free(config);

    // deltas must stay valid after the config is freed
    for(srcml_unit* delta : deltas) {
        CHECK(delta != nullptr);
        if(!delta) continue;
        CHECK(srcml_of(delta).find("<diff:") != std::string::npos);
        srcml_unit_free(delta);
    }
}

TEST_CASE(separate_configs_in_parallel_threads) {
    std::vector<std::string> expected;
    for(const source_pair& pair : sample_pairs()) {
        archive out;
        srcml_unit* original_unit = out.parse(pair.original, pair.language, std::string("a.") + pair.extension);
        srcml_unit* modified_unit = out.parse(pair.modified, pair.language, std::string("b.") + pair.extension);
        srcml_unit* delta = srcdiff_create_delta(nullptr, original_unit, modified_unit);
        expected.push_back(delta ? srcml_of(delta) : "<NULL>");
        if(delta) srcml_unit_free(delta);
        srcml_unit_free(modified_unit);
        srcml_unit_free(original_unit);
    }

    const int THREADS = 4;
    const int ROUNDS  = 3;
    std::vector<std::vector<std::string>> actual(THREADS);
    std::vector<std::thread> threads;
    for(int thread_number = 0; thread_number < THREADS; ++thread_number) {
        threads.emplace_back([thread_number, &actual]() {
            archive out;
            srcdiff_config* config = srcdiff_config_create();
            for(int round = 0; round < ROUNDS; ++round) {
                for(const source_pair& pair : sample_pairs()) {
                    srcml_unit* original_unit = out.parse(pair.original, pair.language, std::string("a.") + pair.extension);
                    srcml_unit* modified_unit = out.parse(pair.modified, pair.language, std::string("b.") + pair.extension);
                    srcml_unit* delta = srcdiff_create_delta(config, original_unit, modified_unit);
                    actual[thread_number].push_back(delta ? srcml_of(delta) : "<NULL>");
                    if(delta) srcml_unit_free(delta);
                    srcml_unit_free(modified_unit);
                    srcml_unit_free(original_unit);
                }
            }
            srcdiff_config_free(config);
        });
    }
    for(std::thread& thread : threads) thread.join();

    int mismatches = 0;
    for(int thread_number = 0; thread_number < THREADS; ++thread_number) {
        for(std::vector<std::string>::size_type pos = 0; pos < actual[thread_number].size(); ++pos) {
            if(actual[thread_number][pos] != expected[pos % expected.size()]) ++mismatches;
        }
    }
    CHECK_MESSAGE(mismatches == 0, std::to_string(mismatches) + " of " + std::to_string(THREADS * ROUNDS * expected.size())
                  + " parallel deltas differ from the sequential result");
}
