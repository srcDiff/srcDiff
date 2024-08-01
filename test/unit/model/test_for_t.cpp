// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_for_t.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE for_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <for.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "for";

std::vector<std::tuple<std::string, std::string>> test_cases_for_control = {
  {"for (i = 0; i < 5; i++){j++;}", "(i = 0; i < 5; i++)"},
  {"for (i = 5; i > 0; i--){j++;}", "(i = 5; i > 0; i--)"},
  {"for (){}"                     , "()"                 },
  {"for ()"                       , "()"                 }
};

BOOST_DATA_TEST_CASE(for_t_control, data::make(test_cases_for_control), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const for_t> for_construct = std::dynamic_pointer_cast<const for_t>(test_data.test_construct);
    std::shared_ptr<const construct> control_child = for_construct->control();

    BOOST_TEST(control_child);
    BOOST_TEST(control_child->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_for_condition = {
    {"for (i = 0; i < 5; i++){j++;}", "i < 5;"},
    {"for (; i < 5;){j++;}"         , "i < 5;"},
    {"for (; i < 5;)"               , "i < 5;"},
    {"for (;;){}"                   , ";"     },

};

BOOST_DATA_TEST_CASE(for_t_condition, data::make(test_cases_for_condition), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const for_t> for_construct = std::dynamic_pointer_cast<const for_t>(test_data.test_construct);
    std::shared_ptr<const construct> condition_child = for_construct->condition();

    BOOST_TEST(condition_child);
    BOOST_TEST(condition_child->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_for_matchable_impl_true = {
    {"for (i = 0; i < 5; i++){j++}"   , "for (i = 0; i < 5; i++);"},
    {"for (i = 5; i > 0; i--){j++}"   , "for (i = 5; i > 0; i--);"},
    {"for (; i < 5; i++){j++}"        , "for (; i < 5; i++);"     },
    {"for (i = 5; i > 5;){j++}"       , "for (i = 5; i > 5;);"    },
    {"for (; i < 5;){}"               , "for(; i < 5;);"          },
    {"for (i : j){}"                  , "for (i : j);"            },
};

BOOST_DATA_TEST_CASE(for_t_control_matchable_true, data::make(test_cases_for_matchable_impl_true), original , modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const for_t> original_for_construct = std::dynamic_pointer_cast<const for_t>(original_data.test_construct);
    std::shared_ptr<const for_t> modified_for_construct = std::dynamic_pointer_cast<const for_t>(modified_data.test_construct);

    BOOST_TEST(original_for_construct->is_matchable_impl(*modified_for_construct));
    BOOST_TEST(modified_for_construct->is_matchable_impl(*original_for_construct));
}
std::vector<std::tuple<std::string, std::string>> test_cases_for_matchable_impl_false = {
    {"for (i = 0; i < 4; i++){d++}"  , "for (d = 0; d < 5;){o++}"},
    {"for (i = 0; i < 5; i++){j++}"  , "for (f = 0; f < 5;){j++}"},
    {"for (i = 5; i > 0; i--){j++}"  , "for (g = 5; g > 0;){k++}"},
    {"for (; i < 5; i++){j++}"       , "for (t; t < 5;){f++}"    },
    {"for (i = 5; i > 5;){j++}"      , "for (o = 5; o > 5;){d++}"},
    {"for (; i < 5;){}"              , "for(; i > 5; i++){k++}"  },
    {"for (j = 0;;){}"               , "for(i = 0;;j++){i++}"    },
    {"for (j = 0; j = 5;){}"         , "for(; l < 3; j++){l++}"  }
};

BOOST_DATA_TEST_CASE(for_t_control_matchable_false, data::make(test_cases_for_matchable_impl_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const for_t> original_for_construct = std::dynamic_pointer_cast<const for_t>(original_data.test_construct);
    std::shared_ptr<const for_t> modified_for_construct = std::dynamic_pointer_cast<const for_t>(modified_data.test_construct);

    BOOST_TEST(!original_for_construct->is_matchable_impl(*modified_for_construct));
    BOOST_TEST(!modified_for_construct->is_matchable_impl(*original_for_construct));
}

std::vector<std::tuple<std::string, std::string>> test_cases_foreach_control = {
    {"for (int n : a){}"         , "(int n : a)"   },
    {"for (auto &x : y){}"       , "(auto &x : y)" },
};

BOOST_DATA_TEST_CASE(foreach_control, data::make(test_cases_foreach_control), original, expected) {

    construct_test_data test_data = create_test_construct(original, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const for_t> for_construct = std::dynamic_pointer_cast<const for_t>(test_data.test_construct);
    std::shared_ptr<const construct> control_child = for_construct->control();

    BOOST_TEST(control_child);
    BOOST_TEST(control_child->to_string() == expected);
}
