/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#define BOOST_TEST_MODULE test get_condition

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

    // While
    create_nodes("while(found == false) {}", "C++"),
    create_nodes("while(true) {}", "C++"),
    create_nodes("while(1) {}", "C++"),
    create_nodes("while(i <= 10) {}", "C++"),

    // Switch
    create_nodes("switch(value) {}", "C++"),

    // Do
    create_nodes("do {} while(i <= 10);", "C++"),

    // If
    create_nodes("if(i <= 10) {}", "C++"),
    create_nodes("if(true) {}", "C++"),

    // Else If
    create_nodes("if(i < 10) {} else if(i > 10) {}", "C++"),

    // For
    create_nodes("for(int i = 0; i <= 10; i++) {}", "C++"),
    create_nodes("for(; 1; ) {}", "C++"),
    create_nodes("for(;;) {}", "C++"),
    create_nodes("for(int i = 0; (i < 10) ? true : false; i++) {}", "C++"),
    create_nodes("for(const int& i : nums) {}", "C++"),
    create_nodes("for(auto i : nums) {}", "C++"),

    // Qt foreach
    create_nodes("foreach(const QString &str, values) {}", "C++"),

    // No condition
    create_nodes("int value = 0;", "C++"),

};

const int start_pos[] = {

    // While
    0,      // <while>:0
    0,      // <while>:0
    0,      // <while>:0
    0,      // <while>:0

    // Switch
    0,      // <switch>:0

    // Do
    0,      // <do>:0

    // If
    0,      // <if_stmt>:0
    0,      // <if_stmt>:0

    // Else If
    29,      // <if type="elseif">:29

    // For
    0,      // <for>:0
    0,      // <for>:0
    0,      // <for>:0
    0,      // <for>:0
    0,      // <for>:0
    0,      // <for>:0

    // QT foreach
    0,      // <foreach>:0

    // No condition
    0,      // <decl_stmt>:0

};

const std::string result[] = {

    // While
    "found==false",
    "true",
    "1",
    "i<=10",

    // Switch
    "value",

    // Do
    "i<=10",

    // If
    "i<=10",
    "true",

    // Else If
    "i>10",

    // For
    "i<=10",
    "1",
    "",
    "(i<10)?true:false",
    "",
    "",

    // QT foreach
    "",

    // No condition
    "",

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(result), node, start_pos, rhs) {

    BOOST_TEST(get_condition(*node, start_pos) == rhs);
}
