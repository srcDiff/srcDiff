// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_construct.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE construct_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <construct.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "try";

std::vector<std::tuple<std::string, std::string>> test_cases_operator_equals_true = {
    {"try {int test = 15;}" , "try {int test = 15;}" },
    {"try {}"               , "try {}"               },
    {"try {test();} catch{}", "try {test();} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_equals_true, data::make(test_cases_operator_equals_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const construct> original_construct = std::dynamic_pointer_cast<const construct>(original_data.test_construct);
    std::shared_ptr<const construct> modified_construct = std::dynamic_pointer_cast<const construct>(modified_data.test_construct);

    BOOST_TEST(*original_construct == *modified_construct);
}

std::vector<std::tuple<std::string, std::string>> test_cases_operator_equals_false = {
    {"try {int test = 15;}" , "try {}"               },
    {"try {}"               , "try {test();}"        },
    {"try {} catch{}"       , "try {test();} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_equals_false, data::make(test_cases_operator_equals_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const construct> original_construct = std::dynamic_pointer_cast<const construct>(original_data.test_construct);
    std::shared_ptr<const construct> modified_construct = std::dynamic_pointer_cast<const construct>(modified_data.test_construct);

    BOOST_TEST(!(*original_construct == *modified_construct));
}

std::vector<std::tuple<std::string, std::string>> test_cases_operator_not_equals_true = {
    {"try {test();}"        , "try {}"               },
    {"try {int test = 15;}" , "try {}"               },
    {"try {}"               , "try {test();}"        },
    {"try {} catch{}"       , "try {test();} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_not_equals_true, data::make(test_cases_operator_not_equals_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const construct> original_construct = std::dynamic_pointer_cast<const construct>(original_data.test_construct);
    std::shared_ptr<const construct> modified_construct = std::dynamic_pointer_cast<const construct>(modified_data.test_construct);

    BOOST_TEST(*original_construct != *modified_construct);
}

std::vector<std::tuple<std::string, std::string>> test_cases_operator_not_equals_false = {
    {"try {int test = 15;}" , "try {int test = 15;}" },
    {"try {}"               , "try {}"               },
    {"try {test();} catch{}", "try {test();} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_not_equals_false, data::make(test_cases_operator_not_equals_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const construct> original_construct = std::dynamic_pointer_cast<const construct>(original_data.test_construct);
    std::shared_ptr<const construct> modified_construct = std::dynamic_pointer_cast<const construct>(modified_data.test_construct);

    BOOST_TEST(!(*original_construct != *modified_construct));
}

std::vector<std::tuple<std::string, std::string>> test_cases_to_string = {
    {"try {}"        , "try {}"        },
    {"try {} catch{}", "try {} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_to_string, data::make(test_cases_to_string), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);

    BOOST_CHECK_EQUAL(test_construct->to_string(), expected);
}

std::vector<std::tuple<std::string>> test_cases_term = {
    {"try {}"}
};

BOOST_DATA_TEST_CASE(construct_term, data::make(test_cases_term), code) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);

    std::shared_ptr<const srcML::node> test_term = test_construct->term(0);

    BOOST_CHECK_EQUAL(test_construct->term(0), test_term);
}
