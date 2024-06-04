#define BOOST_TEST_MODULE for_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <for.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "for";

std::vector<std::tuple<std::string, std::string>> test_cases_for_control = {
  {"for (i = 0; i < 5; i++){j++;}", "(i = 0; i < 5; i++)"},
  //	{"for (;i < 5;) {j++;}"}
};

BOOST_DATA_TEST_CASE(for_t_control, data::make(test_cases_for_control), code, expected) {

	construct_test_data test_data = create_test_construct(code, construct_type);

	BOOST_TEST(test_data.test_construct);
	std::shared_ptr<const for_t> for_construct = std::dynamic_pointer_cast<const for_t>(test_data.test_construct);
	std::shared_ptr<const construct> control_child = for_construct->control();

	BOOST_TEST(control_child);
	BOOST_TEST(control_child->to_string() == expected);
}
