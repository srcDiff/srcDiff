/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#define BOOST_TEST_MODULE test top_level_name_seek

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <test_utils.hpp>
#include <srcdiff_match_internal.hpp>

namespace bu = boost::unit_test::data;

const std::shared_ptr<srcml_nodes> nodes[] = {

    create_nodes("int i;", "C++"),

    create_nodes("int main() { int i; }", "C++"),
    create_nodes("int main() { int i; }", "C++"),

    create_nodes("if(x >= 10) {}", "C++"),

    create_nodes("class class_name {};", "C++"),

    create_nodes("class subclass_name : public base_class_name {};", "C++"),
    create_nodes("class subclass_name : public base_class_name {};", "C++"),

    // Names within names
    create_nodes("a::b", "C++"),
    create_nodes("a::b", "C++"),

    // Start position is not a starting tag
    create_nodes("const auto & n = node->at(i);", "C++"),
    create_nodes("const auto & n = node->at(i);", "C++"),

    // No name tags
    create_nodes("while(true) {}", "C++"),

};

const int start_pos[] = {

    1,      // <decl>:1

    0,      // <function>:0
    20,     // <decl>:20

    5,      // <condition>:5

    0,      // <class>:0

    0,      // <class>:0
    10,     // <super>:10

    // Names within names
    0,      // <expr>:0
    1,      // <name>:1

    // Start position is not a starting tag
    4,      // 'const':4
    5,      // </specifier>:5

    // No name tags
    0,      // <while>:0

};

const int end_pos[] = {

    8,      // <name>:8, name of variable "i"

    7,      // <name>:7, name of function "main"
    27,     // <name>:27, name of variable "i" inside function "main"

    6,      // <name>:6, name of variable "i" inside if statment condition

    3,      // <name>:3, name of class "class_name"

    3,      // <name>:2, name of subclass "subclass_name"
    15,     // <name>:15, name of base class "base_class_name"

    // Names within names
    1,      // <name>:1, outer name tag, "a::b"
    2,      // <name>:2, inner name tag, "a"

    // Start position not a starting tag
    4,      // 'const':4
    5,      // </specifier>:5

    // No name tags
    0,      // <while>:0

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(end_pos), node, start_pos, rhs) {

    int start = start_pos;

    top_level_name_seek(*node, start);

    BOOST_TEST(start == rhs);
}
