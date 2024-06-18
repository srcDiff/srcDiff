// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_named_construct.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE named_construct_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>
#include <named_construct.hpp>

// Define test data
namespace data = boost::unit_test::data;

std::vector<std::tuple<std::string, std::string, std::string>> test_cases_named_construct_name = {
    {"class test_class {};"                , "test_class"     , "class"        },
    {"struct test_struct {};"              , "test_struct"    , "struct"       },
    {"union test_union {};"                , "test_union"     , "union"        },
    {"enum test_enum {};"                  , "test_enum"      , "enum"         },
    {"void test_function();"               , "test_function"  , "function_decl"}
};

BOOST_DATA_TEST_CASE(named_construct_name, data::make(test_cases_named_construct_name), original, expected, construct_type) {

    construct_test_data test_data = create_test_construct(original, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const named_construct> test_named_construct = std::dynamic_pointer_cast<const named_construct>(test_data.test_construct);

    BOOST_TEST(test_named_construct->name()->to_string() == expected);
 }
