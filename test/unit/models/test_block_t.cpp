// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_block_t.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE if_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <if.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "block";

std::vector<std::tuple<std::string>> test_cases_block_content = {
    {"{}"},
    {"{{}}"},
    {"{{{}}}"},
    {"{a=1;}"},
    {"{a=2;{}}"},
    {"{a=3;{a=4;}}"},
    {"{{a=333;{;}};;;;;;;};;"},
    {"{a=3;{a=5;{a=6;}}}"}
};

// TODO: add expected output
// test here

std::vector<std::tuple<std::string>> test_cases_is_syntax_similar_impl = {
    {"int a = 1;", "int a = 1;"}, //identical
    {"int a = 1;", "int b = 1;"}, //similar
    {"int a = 1;", "if (b==c) {d = true;}"}, //completely different
    //node_type START
    {"",""},
    {"",""}, 
    {"",""},
    {"",""},
    //blocks
    {"int a = 1;","{int a = 1;}"},
    {"{int a = 1}", "int a = 1;"}, 
    {"{if (a==b){for (auto c in d){e = f;}}}", "{if (b==c){d = true;}}"},
    {"{if (b==c){d = true;}}", "{if (a==b){for (auto c in d){e = f;}}}"},
};

std::vector<std::tuple<std::string>> test_cases_is_matchable_imple = {
    //psuedo_original == psuedo_modified
    {"if(true) return false;","if(false) {return true;}"}, 
    {"if(true) {return false;}","if(true) return false;"},
    {"if(false) {return true;}","if(true) {return false;}"},
    {"if(true) return true;","if(false) return false;"},
    //similarity > 0/similarlity < 0
    {"",""},
    {"",""},
    //match_operation == nesting.operation
    {"",""},
    {"",""}
};
