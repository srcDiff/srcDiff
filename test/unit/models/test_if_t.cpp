#define BOOST_TEST_MODULE if_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <if.hpp>

// Define test data 
namespace data = boost::unit_test::data;

std::vector<std::tuple<std::string, std::string>> test_cases_has_real_block = {
    {"if (x) {y=1;}", "if"},
    {"if (y) {x=1;}", "if"},
    {"if (z) {a=0;}", "if"}
};

BOOST_DATA_TEST_CASE(if_t_has_real_block, data::make(test_cases_has_real_block), code, construct_name) {

    construct_test_data test_data = create_test_construct(code, construct_name);


    
    BOOST_TEST(test_data.test_construct);
    std::shared_ptr<if_t> if_construct = std::dynamic_pointer_cast<if_t>(test_data.test_construct);
    //BOOST_TEST_MESSAGE("DEREFERENCED IF_CONSTRUCT: " << *if_construct);
    BOOST_TEST(if_construct->has_real_block());

}

std::vector<std::tuple<std::string, std::string, std::string, std::string>> test_cases_is_block_matchable = {
       // Original            // Modified
      {"if (x) {y=1;}", "if", "if (y) {x=1;}", "if"},
  // Just swap X and Y
       // Original            // Modified
      {"if (y) {x=1;}", "if", "if (x) {y=1;}", "if"},  
  
};

BOOST_DATA_TEST_CASE(if_t_is_block_matchable, data::make(test_cases_is_block_matchable), code1, construct_name1, code2, construct_name2) {

  construct_test_data original_data = create_test_construct(code1, construct_name1);
  construct_test_data modified_data = create_test_construct(code2, construct_name2);

  BOOST_TEST(original_data.test_construct);
  BOOST_TEST(modified_data.test_construct);

  std::shared_ptr<if_t> if_construct_original = std::dynamic_pointer_cast<if_t>(original_data.test_construct);
  std::shared_ptr<if_t> if_construct_modified = std::dynamic_pointer_cast<if_t>(modified_data.test_construct);

  BOOST_TEST(!(if_construct_original)->is_block_matchable(*if_construct_modified));
}



