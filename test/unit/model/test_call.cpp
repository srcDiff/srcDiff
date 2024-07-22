// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_call.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE call_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <call.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "call";

std::vector<std::tuple<std::string, std::string>> test_cases_name = {
    {"test_function();" , "test_function" },
    {"test::function();", "function"      },
    {"test.function();" , "function"      },
    {"test_function(x);", "test_function" },
    {"test.function(x);", "function"      }
};

BOOST_DATA_TEST_CASE(call_name, data::make(test_cases_name), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const call> call_construct = std::dynamic_pointer_cast<const call>(test_data.test_construct);
    std::shared_ptr<const name_t> call_name = call_construct->name();

    BOOST_TEST(call_name->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_true = {
    {"test_function();" , "test_function();" },
    {"test::function();", "::function();"    },
    {"test.function();" , "test.function();" },
    {"function::test();", "function::test();"},
    {"test_function(x);", "test_function(y);"},
    {"test.function(x);", "function();"      },
};

BOOST_DATA_TEST_CASE(call_is_matchable_impl_true, data::make(test_cases_is_matchable_impl_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const call> original_call_construct = std::dynamic_pointer_cast<const call>(original_data.test_construct);
    std::shared_ptr<const call> modified_call_construct = std::dynamic_pointer_cast<const call>(modified_data.test_construct);

    BOOST_TEST(original_call_construct->is_matchable_impl(*modified_call_construct));
    BOOST_TEST(modified_call_construct->is_matchable_impl(*original_call_construct));
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_false = {
    {"test_function();" , "best_function();" },
    {"test::function();", "::func();"        },
    {"test.function();" , "test.func(x,y);"  },
    {"function::test();", "function::jest();"},
    {"test_function(x);", "test(y);"         },
    {"test.function(x);", "f();"             },
};

BOOST_DATA_TEST_CASE(call_is_matchable_impl_false, data::make(test_cases_is_matchable_impl_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const call> original_call_construct = std::dynamic_pointer_cast<const call>(original_data.test_construct);
    std::shared_ptr<const call> modified_call_construct = std::dynamic_pointer_cast<const call>(modified_data.test_construct);

    BOOST_TEST(!original_call_construct->is_matchable_impl(*modified_call_construct));
    BOOST_TEST(!modified_call_construct->is_matchable_impl(*original_call_construct));
}
