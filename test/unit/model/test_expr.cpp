// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_expr.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE expr_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <expr.hpp>

// Define test data
namespace data = boost::unit_test::data;

const std::string construct_type = "expr";

std::vector<std::tuple<std::string>> test_cases_is_single_call_true = {
    {"function();"      },
    {"to_string();"     },
    {"test.function();" },
    {"test->function();"},
   // {"new int(0);"      },
};

BOOST_DATA_TEST_CASE(expr_is_single_call_true, data::make(test_cases_is_single_call_true), code) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const expr_t> expr_construct = std::dynamic_pointer_cast<const expr_t>(test_data.test_construct);

    BOOST_TEST(expr_construct->is_single_call());
}

std::vector<std::tuple<std::string>> test_cases_is_single_call_false = {
    {"test()->to_string();"     },
    {"test().something();"      },
    {"vec.at(1).test();"        },
    {"map[key].test();"         },
    {"test.tester().to_string()"}
};

BOOST_DATA_TEST_CASE(expr_is_single_call_false, data::make(test_cases_is_single_call_false), code) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const expr_t> expr_construct = std::dynamic_pointer_cast<const expr_t>(test_data.test_construct);

    BOOST_TEST(!expr_construct->is_single_call());
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_true = {
    {"function();"      , "function();"      },
    {"test;"            , "test;"            },
    {"test->function();", "test->function();"},
    {"test.member();"   , "test.member();"   },
    {"new int(10);"     , "new int(10);"     },
    {"a++;"             , "a++;"             },
    {"x+=3;"            , "x+=3;"            },
    {"0;"               , "0;"               },
    {"(a>b)?a:b;"       , "(a>b)?a:b;"       }
};

BOOST_DATA_TEST_CASE(expr_is_matchable_impl_true, data::make(test_cases_is_matchable_impl_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const expr_t> original_expr = std::dynamic_pointer_cast<const expr_t>(original_data.test_construct);
    std::shared_ptr<const expr_t> modified_expr = std::dynamic_pointer_cast<const expr_t>(modified_data.test_construct);

    BOOST_TEST(original_expr->is_matchable_impl(*modified_expr));
    BOOST_TEST(modified_expr->is_matchable_impl(*original_expr));
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_false = {
    {"test->function();", "best->tester();"            },
    {"test.member();"   , "integer.private();"         },
    {"new int(10);"     , "new char('a');"             },
    {"a++;"             , "b--;"                       },
    {"x+=3;"            , "y-=3;"                      },
    {"0;"               , "1;"                         },
    {"(a>b)?a:b;"       , "(test>=0)?\"done\":\"not\";"}
};

BOOST_DATA_TEST_CASE(expr_is_matchable_impl_false, data::make(test_cases_is_matchable_impl_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const expr_t> original_expr = std::dynamic_pointer_cast<const expr_t>(original_data.test_construct);
    std::shared_ptr<const expr_t> modified_expr = std::dynamic_pointer_cast<const expr_t>(modified_data.test_construct);

    BOOST_TEST(!original_expr->is_matchable_impl(*modified_expr));
    BOOST_TEST(!modified_expr->is_matchable_impl(*original_expr));
}
