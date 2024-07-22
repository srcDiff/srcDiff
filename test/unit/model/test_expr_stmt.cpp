// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_expr_stmt.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE expr_construct_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <expr_stmt.hpp>

// Define test data
namespace data = boost::unit_test::data;

const std::string construct_type = "expr_stmt";

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_true = {
    {"test;"            , "test;"            },
    {"test->function();", "test->function();"},
    {"test.member();"   , "test.member();"   },
    {"new int(10);"     , "new int(10);"     },
    {"a++;"             , "a++;"             },
    {"x+=3;"            , "x+=3;"            },
    {"0;"               , "0;"               },
    {"(a>b)?a:b;"       , "(a>b)?a:b;"       }
};

BOOST_DATA_TEST_CASE(expr_stmt_is_matchable_impl_true, data::make(test_cases_is_matchable_impl_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const expr_stmt> original_expr_stmt = std::dynamic_pointer_cast<const expr_stmt>(original_data.test_construct);
    std::shared_ptr<const expr_stmt> modified_expr_stmt = std::dynamic_pointer_cast<const expr_stmt>(modified_data.test_construct);

    BOOST_TEST(original_expr_stmt->is_matchable_impl(*modified_expr_stmt));
    BOOST_TEST(modified_expr_stmt->is_matchable_impl(*original_expr_stmt));
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_false = {
    {"test;"            , "best;"                      },
    {"test->function();", "test->tester();"            },
    {"test.member();"   , "integer.private();"         },
    {"new int(10);"     , "new char('a');"             },
    {"a++;"             , "b--;"                       },
    {"x+=3;"            , "y-=3;"                      },
    {"0;"               , "1;"                         },
    {"(a>b)?a:b;"       , "(test>=0)?\"done\":\"not\";"}
};

BOOST_DATA_TEST_CASE(expr_stmt_is_matchable_impl_false, data::make(test_cases_is_matchable_impl_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const expr_stmt> original_expr_stmt = std::dynamic_pointer_cast<const expr_stmt>(original_data.test_construct);
    std::shared_ptr<const expr_stmt> modified_expr_stmt = std::dynamic_pointer_cast<const expr_stmt>(modified_data.test_construct);

    BOOST_TEST(!original_expr_stmt->is_matchable_impl(*modified_expr_stmt));
    BOOST_TEST(!modified_expr_stmt->is_matchable_impl(*original_expr_stmt));
}
