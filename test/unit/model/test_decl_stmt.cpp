// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_decl_stmt.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE decl_stmt_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <decl_stmt.hpp>

// Define test data
namespace data = boost::unit_test::data;

std::vector<std::tuple<std::string, std::string, std::string>> test_cases_expr = {
	{"int i = 0;"                  , "0;"        , "0"           },
	{"char s = 's';"               , "'s';"      , "'s'"         },
	{"float f = 3.14;"             , "3.14;"     , "3.14"        },
	{"double d = 6.28;"            , "6.28;"     , "6.28"        },
	{"bool b = true;"              , "true;"     , "true"        },
	{"short sh = 10;"              , "10;"       , "10"          },
	{"long l = 100000;"            , "100000;"   , "100000"      },
	{"unsigned u = 42;"            , "42;"       , "42"          },
	{"char c = 'c';"               , "'c';"      , "'c'"         },
	{"int* p = &i;"                , "&i;"       , "&i"          },
	{"const int ci = 5;"           , "5;"        , "5"           },
	{"std::string str = \"hello\";", "\"hello\";", "\"hello\""   },
    {"int function = test(param);" , "param;"    , "test(param)" }
};

BOOST_DATA_TEST_CASE(decl_stmt_expr, data::make(test_cases_expr), code, expr, expected) {

	construct_test_data decl_data = create_test_construct(code, "decl_stmt");
	BOOST_TEST(decl_data.test_construct);

	construct_test_data expr_data = create_test_construct(expr, "expr_stmt");
	BOOST_TEST(expr_data.test_construct);

	std::shared_ptr<const decl_stmt> decl_stmt_construct = std::dynamic_pointer_cast<const decl_stmt>(decl_data.test_construct);
	std::shared_ptr<const expr_construct> expr_stmt_construct = std::dynamic_pointer_cast<const expr_construct>(expr_data.test_construct);
	std::shared_ptr<const expr_t> expr_child = decl_stmt_construct->expr(*expr_stmt_construct);

	BOOST_TEST(expr_child->to_string() == expected);
}
