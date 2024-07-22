// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_if_t.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE if_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <if.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "if";

std::vector<std::tuple<std::string>> test_cases_has_real_block = {
    {"if (y) {x=1;}"     },
    {"if (x) {y=1;}"     },
    {"if (y) {x=1;}"     },
    {"if (z) {a=0;}"     },
    {"if (z>1) {a=0;}"   },
    {"if (z<2) {b=1;}"   },
    {"if (a>55) {c=2;}"  },
    {"if (f=22) {d=3;}"  },
    {"if (z>=145) {e=4;}"},
    {"if (z<=1) {f=5;}"  },
    {"if (z!=1) {g=6;}"  },
};

BOOST_DATA_TEST_CASE(if_t_has_real_block, data::make(test_cases_has_real_block), code) {

    construct_test_data test_data = create_test_construct(code, construct_type);

    BOOST_TEST(test_data.test_construct);
    std::shared_ptr<const if_t> if_construct = std::dynamic_pointer_cast<const if_t>(test_data.test_construct);
    BOOST_TEST(if_construct->has_real_block());

}

std::vector<std::tuple<std::string, std::string>> test_cases_is_block_matchable = {
       // Original         // Modified
      {"if (x) {y=1;}"  , "if (y) {x=1;}"    },
      {"if (y) {x=1;}"  , "if (x) {y=1;}"    },
      {"if (z=1) {x=1;}", "if (x>=1) {z=1;}" },
      {"if (z>1) {x=1;}", "if (x>1) {z=1;}"  },
      {"if (z<1) {x=1;}", "if (x<1) {z=1;}"  },
      {"if (z<=1) {x=1;}", "if (x<=1) {z=1;}"},
      {"if (z>=1) {x=1;}", "if (x>=1) {z=1;}"},
      {"if (z!=1) {x=1;}", "if (x!=1) {z=1;}"},
      {"if (1) {a;}"     , "if (1) a;"       },

};

BOOST_DATA_TEST_CASE(if_t_is_block_matchable, data::make(test_cases_is_block_matchable), original, modified) {

  construct_test_data original_data = create_test_construct(original, construct_type);
  construct_test_data modified_data = create_test_construct(modified, construct_type);

  BOOST_TEST(original_data.test_construct);
  BOOST_TEST(modified_data.test_construct);

  std::shared_ptr<const if_t> if_original = std::dynamic_pointer_cast<const if_t>(original_data.test_construct);
  std::shared_ptr<const if_t> if_modified = std::dynamic_pointer_cast<const if_t>(modified_data.test_construct);

  BOOST_TEST(!(if_original)->is_block_matchable(*if_modified));
}


std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_true = {
   // Original          // Modified
  {"if (x) {y=1;}"    , "if (x) {y=1;}"      },
  {"if (z) {a=1;}"    , "if (z) {a=1;}"      },
  {"if (x>1) {y=1;}"  , "if (x>1) {y=1;}"    },
  {"if (x>=1) {y=1;}" , "if (x>=1) {y=1;}"   },
  {"if (y<1) {y=1;}"  , "if (y<1) {y=1;}"    },
  {"if (z<=1) {y=1;}" , "if (x<=1) {y=1;}"   },
  {"if (x=1) {y=1;}"  , "if (x=1) {y=1;}"    },
  {"if (x!=1) {y=1;}" , "if (x!=1) {y=1;}"   },
  {"if (x=1) {y!=1;}" , "if (x=1) {y!=1;}"   },
  {"if (x>=1) {y=1;}" , "if (x>=1) {y=1;}"   },
  {"if (1) {a;}"      , "if (1) a;"          },
};

BOOST_DATA_TEST_CASE(if_t_matchable_impl_true, data::make(test_cases_is_matchable_impl_true), original, modified) {
    construct_test_data original_data = create_test_construct(original, construct_type);
    construct_test_data modified_data = create_test_construct(modified, construct_type);

    std::shared_ptr<const if_t> if_original = std::dynamic_pointer_cast<const if_t>(original_data.test_construct);
    std::shared_ptr<const if_t> if_modified = std::dynamic_pointer_cast<const if_t>(modified_data.test_construct);

    BOOST_TEST(if_original->is_matchable_impl(*if_modified));
    BOOST_TEST(if_modified->is_matchable_impl(*if_original));
}


std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_false = {
    {"if (y) {x=1;}"   , "if (x) {x=2;}"   },
    {"if (x!=1) {y=1;}", "if (x=1) {y!=1;}"},
    {"if (x>=1) {y=1;}", "if (x=1) {y>=1;}"},
    {"if (x<1) {y=1;}" , "if (x=1) {y<1;}" },
};

BOOST_DATA_TEST_CASE(if_t_matchable_impl_false, data::make(test_cases_is_matchable_impl_false), original, modified) {
    construct_test_data original_data = create_test_construct(original, construct_type);
    construct_test_data modified_data = create_test_construct(modified, construct_type);

    std::shared_ptr<const if_t> if_original = std::dynamic_pointer_cast<const if_t>(original_data.test_construct);
    std::shared_ptr<const if_t> if_modified = std::dynamic_pointer_cast<const if_t>(modified_data.test_construct);

    BOOST_TEST(!if_original->is_matchable_impl(*if_modified));
    BOOST_TEST(!if_modified->is_matchable_impl(*if_original));
}





