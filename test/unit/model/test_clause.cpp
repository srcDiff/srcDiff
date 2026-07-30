// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_clause.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE clause_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <clause.hpp>

// Define test data
namespace data = boost::unit_test::data;

std::vector<std::tuple<std::string, std::string, std::string>> test_cases_block = {
    {"if (x<1){x+=1;}"               , "{x+=1;}"        , "if"   },
    {"if (i < 5){test;}"             , "{test;}"        , "if"   },
    {"if (i<0){rhs-=lhs;}"           , "{rhs-=lhs;}"    , "if"   },
    {"if (i=0){i=1;}"                , "{i=1;}"         , "if"   },
    {"if (i>0){f;g;}"                , "{f;g;}"         , "if"   },
    {"if (i!=0){i=0;}"               , "{i=0;}"         , "if"   },
    {"if (test){}"                   , "{}"             , "if"   },
    {"if (!test){not_pass();}"       , "{not_pass();}"  , "if"   },
    {"if (test && pass){throw i;}"   , "{throw i;}"     , "if"   },
    {"if (*test)"                    , ""               , "if"   },
    {"if (*test && !pass){null();}"  , "{null();}"      , "if"   },
    {"if (!test || !pass){t+=x;}"    , "{t+=x;}"        , "if"   },
    {"else if (i<0){i--;}"           , "{i--;}"         , "if"   },
    {"else if (i=0){i++;}"           , "{i++;}"         , "if"   },
    {"else if (i>0){subtract();}"    , "{subtract();}"  , "if"   },
    {"else if (i!=0){make_zero();}"  , "{make_zero();}" , "if"   },
    {"else if (test){test->pass();}" , "{test->pass();}", "if"   },
    // Can't do for?
};

BOOST_DATA_TEST_CASE(block_clause, data::make(test_cases_block), code, expected, construct_type) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const clause> clause_construct = std::dynamic_pointer_cast<const clause>(test_data.test_construct);
    std::shared_ptr<const construct> block_contents = clause_construct->block();

    BOOST_TEST(block_contents->to_string() == expected);
}
