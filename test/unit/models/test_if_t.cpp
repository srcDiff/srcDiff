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
    //BOOST_TEST_MESSAGE("DEREFERENCED IF_CONSTRUCT: " << *if_construct);
    BOOST_TEST(if_construct->has_real_block());

}

std::vector<std::tuple<std::string, std::string>> test_cases_is_block_matchable = {
       // Original            // Modified
      {"if (x) {y=1;}", "if (y) {x=1;}"      },
       // Original            // Modified
      {"if (y) {x=1;}", "if (x) {y=1;}"      },
      // Original            // Modified
      {"if (z=1) {x=1;}", "if (x>=1) {z=1;}" },   
      {"if (z>1) {x=1;}", "if (x>1) {z=1;}"  },
      {"if (z<1) {x=1;}", "if (x<1) {z=1;}"  },
      {"if (z<=1) {x=1;}", "if (x<=1) {z=1;}"},
      {"if (z>=1) {x=1;}", "if (x>=1) {z=1;}"},
      {"if (z!=1) {x=1;}", "if (x!=1) {z=1;}"},
      {"if (1) {a;}"     , "if (1) a;"       },

};

BOOST_DATA_TEST_CASE(if_t_is_block_matchable, data::make(test_cases_is_block_matchable), code1, code2) {

  construct_test_data original_data = create_test_construct(code1, construct_type);
  construct_test_data modified_data = create_test_construct(code2, construct_type);

  BOOST_TEST(original_data.test_construct);
  BOOST_TEST(modified_data.test_construct);

  std::shared_ptr<const if_t> if_construct_original = std::dynamic_pointer_cast<const if_t>(original_data.test_construct);
  std::shared_ptr<const if_t> if_construct_modified = std::dynamic_pointer_cast<const if_t>(modified_data.test_construct);

  BOOST_TEST(!(if_construct_original)->is_block_matchable(*if_construct_modified));
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

BOOST_DATA_TEST_CASE(if_t_matchable_impl_true, data::make(test_cases_is_matchable_impl_true), code1, code2) {
    construct_test_data original_data = create_test_construct(code1, construct_type);
    construct_test_data modified_data = create_test_construct(code2, construct_type);

    std::shared_ptr<const if_t> if_construct_original = std::dynamic_pointer_cast<const if_t>(original_data.test_construct);
    std::shared_ptr<const if_t> if_construct_modified = std::dynamic_pointer_cast<const if_t>(modified_data.test_construct);


    /* Debug
    BOOST_TEST_MESSAGE("Original Construct: " << *if_construct_original << (if_construct_original ? " !!!!Created!!!! " : " NULL "));
    BOOST_TEST_MESSAGE("Modified Construct: " << *if_construct_modified << (if_construct_original ? " !!!!Created!!!! " : " NULL "));
    //*/

    /* More debug
    BOOST_TEST_MESSAGE("is_matchable_impl result: " << matchable);
    //*/

    BOOST_TEST(if_construct_original->is_matchable_impl(*if_construct_modified));
}
std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_false = {
    {"if (y) {x=1;}"   , "if (x) {x=2;}"   },
    {"if (x!=1) {y=1;}", "if (x=1) {y!=1;}"},
    {"if (x>=1) {y=1;}", "if (x=1) {y>=1;}"},
    {"if (x<1) {y=1;}" , "if (x=1) {y<1;}" },
    {"if (1) a;"       , "if (1) {a;}"     }
};

BOOST_DATA_TEST_CASE(if_t_matchable_impl_false, data::make(test_cases_is_matchable_impl_false), code1, code2) {
    construct_test_data original_data = create_test_construct(code1, construct_type);
    construct_test_data modified_data = create_test_construct(code2, construct_type);

    std::shared_ptr<const if_t> if_construct_original = std::dynamic_pointer_cast<const if_t>(original_data.test_construct);
    std::shared_ptr<const if_t> if_construct_modified = std::dynamic_pointer_cast<const if_t>(modified_data.test_construct);


    /* Debug
    BOOST_TEST_MESSAGE("Original Construct: " << *if_construct_original << (if_construct_original ? " !!!!Created!!!! " : " NULL "));
    BOOST_TEST_MESSAGE("Modified Construct: " << *if_construct_modified << (if_construct_original ? " !!!!Created!!!! " : " NULL "));
    //*/

    /* More debug
    BOOST_TEST_MESSAGE("is_matchable_impl result: " << matchable);
    //*/
    BOOST_TEST(!(if_construct_original->is_matchable_impl(*if_construct_modified)));
}





