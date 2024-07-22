// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_call.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE name_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <name.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "name";

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_name_true = {
    {"test_function();"     , "test_function"  },
    {"integerA = 10;"       , "integerA"       },
    {"characterB = 'b';"    , "characterB"     },
    {"class test{};"        , "test"           },
    {"template <type>"      , "type"           },
    {"int a;"               , "a"              },

};

BOOST_DATA_TEST_CASE(name_matchable_true, data::make(test_cases_is_matchable_impl_name_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const name_t> original_name_construct = std::dynamic_pointer_cast<const name_t>(original_data.test_construct);
    std::shared_ptr<const name_t> modified_name_construct = std::dynamic_pointer_cast<const name_t>(modified_data.test_construct);

    BOOST_TEST(original_name_construct->is_matchable_impl(*modified_name_construct));
    BOOST_TEST(modified_name_construct->is_matchable_impl(*original_name_construct));
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_name_false = {
    {"test_function;"       , "a.test_function;"      },
    {"integerA;"            , "boost::integerA;"      },
    {"shared_ptr"           , "shared_ptr<type>;"     },
};

BOOST_DATA_TEST_CASE(name_matchable_false, data::make(test_cases_is_matchable_impl_name_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const name_t> original_name_construct = std::dynamic_pointer_cast<const name_t>(original_data.test_construct);
    std::shared_ptr<const name_t> modified_name_construct = std::dynamic_pointer_cast<const name_t>(modified_data.test_construct);

    BOOST_TEST(!original_name_construct->is_matchable_impl(*modified_name_construct));
    BOOST_TEST(!modified_name_construct->is_matchable_impl(*original_name_construct));
}
