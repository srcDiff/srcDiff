// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_string_utils.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE string_utils_Tests
#include <boost/test/included/unit_test.hpp>

#include <string_utils.hpp>

BOOST_AUTO_TEST_CASE(test_string_literal_empty) {
    constexpr string_literal literal("");
    BOOST_CHECK_EQUAL(std::string(literal), "");
}

BOOST_AUTO_TEST_CASE(test_string_literal_init) {
    constexpr string_literal literal("Hello");
    BOOST_CHECK_EQUAL(std::string(literal), "Hello");
}

BOOST_AUTO_TEST_CASE(test_string_literal_complex) {
    constexpr string_literal literal("a1(!)s2*d3&f4=%");
    BOOST_CHECK_EQUAL(std::string(literal), "a1(!)s2*d3&f4=%");
}
