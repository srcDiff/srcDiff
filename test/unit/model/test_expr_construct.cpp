// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_expr_construct.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE expr_construct_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <expr_construct.hpp>

// Define test data
namespace data = boost::unit_test::data;

const std::string construct_type = "expr_stmt";

std::vector<std::tuple<std::string, std::string>> test_cases_expr = {
    {"test;"            , "test"            },
    {"0;"               , "0"               },
    {"function();"      , "function()"      },
    {"x+=3;"            , "x+=3"            },
    {"new int(10);"     , "new int(10)"     },
    {"test->function();", "test->function()"},
    {"test.member();"   , "test.member()"   },
    {"(a>b)?a:b;"       , "(a>b)?a:b"       },
    {"a++;"              , "a++"            }
};

BOOST_DATA_TEST_CASE(expr_construct_expr, data::make(test_cases_expr), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const expr_construct> test_expr_construct = std::dynamic_pointer_cast<const expr_construct>(test_data.test_construct);
    std::shared_ptr<const expr_t> expr_child = test_expr_construct->expr(*test_expr_construct);

    BOOST_TEST(expr_child->to_string() == expected);
}
