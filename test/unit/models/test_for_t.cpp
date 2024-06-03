#define BOOST_TEST_MODULE for_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <for.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "for";

std::vector<std::tuple<std::string, bool>> test_cases_for_has_control = {
	{"for (i = 0; i < 5; i++){j++;}", true},
	{"for {j++;}", false}
};

BOOST_DATA_TEST_CASE(for_t_control, data::make(test_cases_for_control), code, expected) {

	construct_test_data test_data = create_test_construct(code, construct_type);

	BOOST_TEST(test_data.test_construct);
	std::shared_ptr<const for_t> for_construct = std::dynamic_pointer_cast<const for_t>(test_data.test_construct);
	std::shared_ptr<const construct> control_child = for_construct->control();
	bool has_control = false;
	if (control_child) {
		has_control = true;
	}
	BOOST_TEST(has_control == expected);

}