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

#include <name.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "name";

std::vector<std::tuple<std::string, std::string>> test_cases_simple_name = {
    {"test_function();"     , "test_function"  },
    {"integerA = 10;"       , "integerA"       },
    {"characterB = 'b';"    , "characterB"     },
    {"class test{};"        , "test"           },
    {"template <type>"      , "type"           },
    {"int a;"               , "int"            }
};

BOOST_DATA_TEST_CASE(call_name, data::make(test_cases_simple_name), code, name) {

    construct_test_data test_data = create_test_construct(code, construct_type);

    BOOST_TEST(test_data.test_construct);
    std::shared_ptr<const name_t> name_construct = std::dynamic_pointer_cast<const name_t>(test_data.test_construct);

    BOOST_TEST(name_construct->simple_name() == name);
}
