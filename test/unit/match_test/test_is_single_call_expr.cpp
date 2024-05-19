/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#define BOOST_TEST_MODULE test is_single_call_expr

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

    create_nodes("sum(x, y);", "C++"),
    create_nodes("sum(x=1, y=3);", "C++"),
    
    // Nested function call
    create_nodes("sum(pow(x, y), z);", "C++"),

    // Class methods
    create_nodes("a.b();", "C++"),
    create_nodes("a.b();", "C++"),
    create_nodes("a.b(c.d());", "C++"),

    // Arrow operator
    create_nodes("a->b();", "C++"),
    
    // Call chain
    create_nodes("a().b();", "C++"),

    // Not single call expressions
    create_nodes("1 + a();", "C++"),
    create_nodes("a() + 1;", "C++"),
    create_nodes("x = a();", "C++"),
    create_nodes("cout << sum(x, y) << endl;", "C++"),

    // Start position is not <expr_stmt> or <expr>
    create_nodes("a.b();", "C++"),

};

const int start_pos[] = {

    0,      // <expr_stmt>:0
    0,      // <expr_stmt>:0

    // Nested function call
    0,      // <expr_stmt>:0

    // Class methods
    0,      // <expr_stmt>:0
    1,      // <expr>:1
    0,      // <expr_stmt>:0

    // Arrow operator
    0,

    // Call chain
    0,      // <expr_stmt>:0

    // Not single call expressions
    0,      // <expr_stmt>:0
    0,      // <expr_stmt>:0
    0,      // <expr_stmt>:0
    0,      // <expr_stmt>:0

    // Start position is not <expr_stmt> or <expr>
    2,      // <call>:2

};

const bool result[] = {

    true,
    true,

    // Nested function call
    true,

    // Class methods
    true,
    true,
    true,

    // Arrow operator
    true,

    // Call chain
    false,

    // Not single call expressions
    false,
    false,
    false,
    false,

    // Start position is not <expr_stmt> or <expr>
    false,

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(result), node, start_pos, rhs) {

    BOOST_TEST(is_single_call_expr(*node, start_pos) == rhs);
}
