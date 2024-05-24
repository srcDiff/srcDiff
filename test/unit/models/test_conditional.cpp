#define BOOST_TEST_MODULE ConditionalTests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "test_utils.hpp"
#include "if.hpp"
#include "clause.hpp"
#include "conditional.hpp"

// Define test data 
namespace boostdata = boost::unit_test::data;

std::vector<std::tuple<std::string, std::string>> test_cases = {
    {"if (x) {y=1;}", "if"},
    {"else {y=0;}", "else"}
};

BOOST_DATA_TEST_CASE(test_constructs, boostdata::make(test_cases), code, construct_name) {
    std::shared_ptr<construct> construct = create_test_construct(code, construct_name);

    BOOST_TEST(construct);

}
