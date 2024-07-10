// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_identifier_decl.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE identifier_decl_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <identifier_decl.hpp>

// Define test data
namespace data = boost::unit_test::data;

const std::string construct_type = "decl_stmt";

std::vector<std::tuple<std::string, std::string>> test_cases_name = {
    {"int a;"                     , "a"       },
    {"char a = 'b';"              , "a"       },
    {"char a[] = \"test\";"       , "a[]"     },
    {"void* test;"                , "test"    },
    {"std::vector<int> test;"     , "test"    },
    {"std::tuple<int, int> test;" , "test"    },
    {"int function = test(param);", "function"}
};

BOOST_DATA_TEST_CASE(identifier_decl_name, data::make(test_cases_name), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const identifier_decl> identifier_decl_construct = std::dynamic_pointer_cast<const identifier_decl>(test_data.test_construct);
    std::shared_ptr<const name_t> name_child = identifier_decl_construct->name();

    BOOST_TEST(name_child->to_string() == expected);
}



