// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_case_t.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE case_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <case.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "case";

std::vector<std::tuple<std::string, std::string>> test_cases_expr = {
    {"case 1:"      , "1"       },
    {"case a:"      , "a"       },
    {"case test::a:", "test::a" },
    {"case 'x'"     , "'x'"     },
};

BOOST_DATA_TEST_CASE(case_expr, data::make(test_cases_expr), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const case_t> case_t_construct = std::dynamic_pointer_cast<const case_t>(test_data.test_construct);
    std::shared_ptr<const construct> case_t_expr = case_t_construct->expr();

    BOOST_TEST(case_t_expr->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_true = {
    {"case 1:"      , "case 1:"       },
    {"case a:"      , "case a:"       },
    {"case test::a:", "case test::a:" },
    {"case 'x':"    , "case 'x':"     },
};

BOOST_DATA_TEST_CASE(case_is_matchable_impl_true, data::make(test_cases_is_matchable_impl_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const case_t> original_case_construct = std::dynamic_pointer_cast<const case_t>(original_data.test_construct);
    std::shared_ptr<const case_t> modified_case_construct = std::dynamic_pointer_cast<const case_t>(modified_data.test_construct);

    BOOST_TEST(original_case_construct->is_matchable_impl(*modified_case_construct));
    BOOST_TEST(modified_case_construct->is_matchable_impl(*original_case_construct));
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_false = {
    {"case 1:"      , "case 2:"       },
    {"case a:"      , "case b:"       },
    {"case test::a:", "case test::b:" },
    {"case 'x':"    , "case 'y':"     },
};

BOOST_DATA_TEST_CASE(case_is_matchable_impl_false, data::make(test_cases_is_matchable_impl_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const case_t> original_case_construct = std::dynamic_pointer_cast<const case_t>(original_data.test_construct);
    std::shared_ptr<const case_t> modified_case_construct = std::dynamic_pointer_cast<const case_t>(modified_data.test_construct);

    BOOST_TEST(!original_case_construct->is_matchable_impl(*modified_case_construct));
    BOOST_TEST(!modified_case_construct->is_matchable_impl(*original_case_construct));
}
