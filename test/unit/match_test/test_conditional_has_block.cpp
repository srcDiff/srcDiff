/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#define BOOST_TEST_MODULE test conditional_has_block

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <test_utils.hpp>
#include <srcdiff_match_internal.hpp>

namespace bu = boost::unit_test::data;

const node_set_data non_if_nodes[] = {

    // Conditional with block
    create_node_set("while(true) {}", "C++"),
    create_node_set("switch(value) {}", "C++"),
    create_node_set("do {} while(i <= 10);", "C++"),
    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),
    create_node_set("foreach(const QString &str, values) {}", "C++"),

    // Conditional without block
    create_node_set("while(x <= 10) cout << x;", "C++"),
    create_node_set("for(int i = 0; i <= 10; i++) cout << i;", "C++"),
    create_node_set("foreach(const QString &str, values) cout << str;", "C++"),
    create_node_set("(value % 2 == 0) ? cout << \"even\" : cout << \"odd\";", "C++"),

};

const bool non_if_result[] = {

    // Conditional with block
    true,
    true,
    true,
    true,
    true,

    // Conditional without block
    false,
    false,
    false,
    false,

};

const node_set_data if_nodes[] = {

    // If with block
    create_node_set("if(true) {}", "C++"),

    // If without block
    create_node_set("if(true) return true;", "C++"),

};

const bool if_result[] = {

    // If with block
    true,

    // If without block
    false,

};

BOOST_DATA_TEST_CASE(non_ifs, bu::make(non_if_nodes) ^ bu::make(non_if_result), node, rhs) {

    BOOST_TEST(conditional_has_block(node.set) == rhs);
}

BOOST_DATA_TEST_CASE(ifs, bu::make(if_nodes) ^ bu::make(if_result), node, rhs) {

    node_set first = get_first_child(node.set);

    BOOST_TEST(conditional_has_block(first) == rhs);
}
