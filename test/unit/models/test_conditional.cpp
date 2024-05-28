#define BOOST_TEST_MODULE ConditionalTests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <if.hpp>

// Define test data 
namespace data = boost::unit_test::data;

std::vector<std::tuple<std::string, std::string>> test_cases = {
    {"if (x) {y=1;}", "if"},
};

BOOST_DATA_TEST_CASE(test_constructs, data::make(test_cases), code, construct_name) {

    construct_test_data test_data = create_test_construct(code, construct_name);

    BOOST_TEST(test_data.construct);
    std::shared_ptr<if_t> if_construct = std::dynamic_pointer_cast<if_t>(test_data.construct);
    BOOST_TEST(if_construct->has_real_block());

}



