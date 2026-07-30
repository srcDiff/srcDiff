// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_condition.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE condition_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <condition.hpp>

// Define test data
namespace data = boost::unit_test::data;

const std::string construct_type = "condition";

std::vector<std::tuple<std::string, std::string>> test_cases_to_string = {
    {"if (i<0){}"                , "i<0"           },
    {"if (i=0){}"                , "i=0"           },
    {"if (i>0){}"                , "i>0"           },
    {"if (i!=0){}"               , "i!=0"          },
    {"if (test){}"               , "test"          },
    {"if (!test){}"              , "!test"         },
    {"if (test && pass){}"       , "test && pass"  },
    {"if (*test)"                , "*test"         },
    {"if (*test && !pass){}"     , "*test && !pass"},
    {"if (!test || !pass){}"     , "!test || !pass"},
    {"else if (i<0){}"           , "i<0"           },
    {"else if (i=0){}"           , "i=0"           },
    {"else if (i>0){}"           , "i>0"           },
    {"else if (i!=0){}"          , "i!=0"          },
    {"else if (test){}"          , "test"          },
    {"else if (!test){}"         , "!test"         },
    {"else if (test && pass){}"  , "test && pass"  },
    {"else if (*test)"           , "*test"         },
    {"else if (*test && !pass){}", "*test && !pass"},
    {"else if (!test || !pass){}", "!test || !pass"},
    {"for (int i=0;i<1;i++){}"   , "i<1;"          },
    {"for (int j=0;j>10;j++){}"  , "j>10;"         },
    {"for (int k=0;k!=0;k++){}"  , "k!=0;"         },
    {"for (int l=0;l>=1;l++){}"  , "l>=1;"         },
    {"for (int m=5;m<=0;m--){}"  , "m<=0;"         },
    {"for (int n=0;n==0;n++){}"  , "n==0;"         },
    {"for (;o>5;){}"             , "o>5;"          }
};

BOOST_DATA_TEST_CASE(condition_to_string, data::make(test_cases_to_string), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const condition> condition_construct = std::dynamic_pointer_cast<const condition>(test_data.test_construct);

    BOOST_TEST(condition_construct->to_string() == expected);
}
