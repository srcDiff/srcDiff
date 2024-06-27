// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_elseif.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE elseif_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <elseif.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "if";

std::vector<std::tuple<std::string, std::string>> test_cases_elseif_find_if = {
    {"else if (i){}"         , "if (i){}"           },
    {"else if (d>1){}"       , "if (d>1){}"         },
    {"else if (d<1){}"       , "if (d<1){}"         },
    {"else if (){}"          , "if (){}"            },
    {"else if (x) {y=1;}"    , "if (x) {y=1;}"      },
    {"else if (z) {a=1;}"    , "if (z) {a=1;}"      },
    {"else if (x>1) {y=1;}"  , "if (x>1) {y=1;}"    },
    {"else if (x>=1) {y=1;}" , "if (x>=1) {y=1;}"   },
    {"else if (y<1) {y=1;}"  , "if (y<1) {y=1;}"    },
    {"else if (z<=1) {y=1;}" , "if (z<=1) {y=1;}"   },
    {"else if (x=1) {y=1;}"  , "if (x=1) {y=1;}"    },
    {"else if (x!=1) {y=1;}" , "if (x!=1) {y=1;}"   },
    {"else if (x=1) {y!=1;}" , "if (x=1) {y!=1;}"   },
    {"else if (x>=1) {y=1;}" , "if (x>=1) {y=1;}"   },
    {"else if (1) {a;}"      , "if (1) {a;}"        },
};

BOOST_DATA_TEST_CASE(elseif_find_if_test, data::make(test_cases_elseif_find_if), original, expected) {

    construct_test_data test_data = create_test_construct(original, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const elseif> elseif_construct = std::dynamic_pointer_cast<const elseif>(test_data.test_construct);
    BOOST_TEST(elseif_construct);

    std::shared_ptr<const if_t> elseif_construct_if = elseif_construct->find_if();

    BOOST_TEST(elseif_construct_if);
    BOOST_TEST(elseif_construct_if->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_elseif_condition = {
    {"else if (i){}"         , "i"     },
    {"else if (d>1){}"       , "d>1"   },
    {"else if (d<1){}"       , "d<1"   },
    {"else if (){}"          , ""      },
    {"else if (x) {y=1;}"    , "x"     },
    {"else if (z) {a=1;}"    , "z"     },
    {"else if (x>1) {y=1;}"  , "x>1"   },
    {"else if (x>=1) {y=1;}" , "x>=1"  },
    {"else if (y<1) {y=1;}"  , "y<1"   },
    {"else if (z<=1) {y=1;}" , "z<=1"  },
    {"else if (x=1) {y=1;}"  , "x=1"   },
    {"else if (x!=1) {y=1;}" , "x!=1"  },
    {"else if (x=1) {y!=1;}" , "x=1"   },
    {"else if (x>=1) {y=1;}" , "x>=1"  },
    {"else if (1) {a;}"      , "1"     },
};

BOOST_DATA_TEST_CASE(elseif_condition_test, data::make(test_cases_elseif_condition), original, expected) {

    construct_test_data test_data = create_test_construct(original, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const elseif> elseif_construct = std::dynamic_pointer_cast<const elseif>(test_data.test_construct);
    BOOST_TEST(elseif_construct);

    std::shared_ptr<const construct> elseif_condition = elseif_construct->condition();
    BOOST_TEST(elseif_condition);

    BOOST_TEST(elseif_condition->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_elseif_block = {
    {"else if (i){test;}"    , "{test;}"      },
    {"else if (i>0){a.out;}" , "{a.out;}"     },
    {"else if (d>1){d=1}"    , "{d=1}"        },
    {"else if (d<1){d=1}"    , "{d=1}"        },
    {"else if (i){}"         , "{}"           },
    {"else if (d>1){}"       , "{}"           },
    {"else if (d<1){}"       , "{}"           },
    {"else if (){}"          , "{}"           },
    {"else if (x) {y=1;}"    , "{y=1;}"       },
    {"else if (z) {a=1;}"    , "{a=1;}"       },
    {"else if (x>1) {y=1;}"  , "{y=1;}"       },
    {"else if (x>=1) {y=1;}" , "{y=1;}"       },
    {"else if (y<1) {y=1;}"  , "{y=1;}"       },
    {"else if (z<=1) {y=1;}" , "{y=1;}"       },
    {"else if (x=1) {y=1;}"  , "{y=1;}"       },
    {"else if (x!=1) {y=1;}" , "{y=1;}"       },
    {"else if (x=1) {y!=1;}" , "{y!=1;}"      },
    {"else if (x>=1) {y=1;}" , "{y=1;}"       },
    {"else if (1) {a;}"      , "{a;}"         },
    {"else if (1) a;"        , " a;"          } 
};

BOOST_DATA_TEST_CASE(elseif_block_test, data::make(test_cases_elseif_block), original, expected) {

    construct_test_data test_data = create_test_construct(original, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const elseif> elseif_construct = std::dynamic_pointer_cast<const elseif>(test_data.test_construct);
    BOOST_TEST(elseif_construct);

    std::shared_ptr<const construct> elseif_block = elseif_construct->block();
    
    BOOST_TEST(elseif_block->to_string() == expected);
}





