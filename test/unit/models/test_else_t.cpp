// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_else_t.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE else_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <else.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "else";

std::vector<std::tuple<std::string>> test_cases_else_condition = {
    {"else{}"},
    {"else;"},
    {"else{i=0;y=0;d=0;}"},
    {"else i=0;"}
};

BOOST_DATA_TEST_CASE(else_t_condition, data::make(test_cases_else_condition), original) {

    construct_test_data test_data = create_test_construct(original, construct_type);

    BOOST_TEST(test_data.test_construct);
    std::shared_ptr<const else_t> else_construct = std::dynamic_pointer_cast<const else_t>(test_data.test_construct);

    BOOST_TEST(else_construct);
    BOOST_TEST(!else_construct->condition());
}
