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
  {"for (i = 5; i > 0; i--){j++;}", "(i = 5; i > 0; i--)"},
  {"for (){}"                     , "()"                 },
  {"for ()"                       , "()"                 }
};

BOOST_DATA_TEST_CASE(for_t_control, data::make(test_cases_for_control), code, expected) {

	construct_test_data test_data = create_test_construct(code, construct_type);

	BOOST_TEST(test_data.test_construct);
	std::shared_ptr<const for_t> for_construct = std::dynamic_pointer_cast<const for_t>(test_data.test_construct);
	std::shared_ptr<const construct> control_child = for_construct->control();

	BOOST_TEST(control_child);
	BOOST_TEST(control_child->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_for_condition = {
	{"for (i = 0; i < 5; i++){j++;}", "i < 5;"},
	{"for (; i < 5;){j++;}"         , "i < 5;"},
	{"for (; i < 5;)"               , "i < 5;"},
	{"for (;;){}"                   , ";"     },

};

BOOST_DATA_TEST_CASE(for_t_condition, data::make(test_cases_for_condition), code, expected) {

	construct_test_data test_data = create_test_construct(code, construct_type);

	BOOST_TEST(test_data.test_construct);
	std::shared_ptr<const for_t> for_construct = std::dynamic_pointer_cast<const for_t>(test_data.test_construct);
	std::shared_ptr<const construct> condition_child = for_construct->condition();

	BOOST_TEST_MESSAGE("Condition Child Dereferenced:" << *condition_child);

	BOOST_TEST(condition_child);
	BOOST_TEST(condition_child->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_for_matchable_impl_true = {
    {"for (i = 0; i < 5; i++){j++}"   , "for (i = 0; i < 5; i++)"},
    {"for (i = 5; i > 0; i--){j++}"   , "for (i = 5; i > 0; i--)"},
    {"for (; i < 5; i++){j++}"        , "for (; i < 5; i++)"     },
    {"for (i = 5; i > 5;){j++}"       , "for (i = 5; i > 5;)"    },
    {"for (; i < 5;){}"               , "for(; i < 5;)"          },
    {"for (i = 0; j = 5;){}"          , "for(i = 0;;)"           },
};

BOOST_DATA_TEST_CASE(for_t_control_matchable_true, data::make(test_cases_for_matchable_impl_true), code1 , expected) {
    construct_test_data original_data = create_test_construct(code1, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(expected, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const for_t> for_construct = std::dynamic_pointer_cast<const for_t>(original_data.test_construct);
    std::shared_ptr<const for_t> modified_for_construct = std::dynamic_pointer_cast<const for_t>(modified_data.test_construct);

    std::shared_ptr<const construct> original_for_control = for_construct->control();
    std::shared_ptr<const construct> modified_for_control = modified_for_construct->control();

    BOOST_TEST(original_for_control->is_matchable_impl(*modified_for_control));
    BOOST_TEST(modified_for_control->is_matchable_impl(*original_for_control));
}
std::vector<std::tuple<std::string, std::string>> test_cases_for_matchable_impl_false = {
    {"for (i = 0; i < 4; i++){d++}"  , "for (d = 0; d < 5;)"},
    {"for (i = 0; i < 5; i++){j++}"  , "for (f = 0; f < 5;)"},
    {"for (i = 5; i > 0; i--){j++}"  , "for (g = 5; g > 0;)"},
    {"for (; i < 5; i++){j++}"       , "for (t; t < 5;)"    },
    {"for (i = 5; i > 5;){j++}"      , "for (o = 5; o > 5;)"},
    {"for (; i < 5;){}"              , "for(; i > 5; i++)"  },
    {"for (j = 0;;){}"               , "for(i = 0;;j++)"    },
    {"for (j = 0; j = 5;){}"         , "for(; l < 3; j++){}"}

};
/*
BOOST_DATA_TEST_CASE(for_t_control_matchable_false, data::make(test_cases_for_matchable_impl_false), code1, expected) {
    construct_test_data original_data = create_test_construct(code1, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(expected, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const for_t> for_construct = std::dynamic_pointer_cast<const for_t>(original_data.test_construct);
    std::shared_ptr<const for_t> modified_for_construct = std::dynamic_pointer_cast<const for_t>(modified_data.test_construct);

    std::shared_ptr<const construct> original_for_control = for_construct->control();
    std::shared_ptr<const construct> modified_for_control = modified_for_construct->control();

    BOOST_TEST_MESSAGE("DEREFERENCED: " << *original_for_control);
    BOOST_TEST_MESSAGE("DEREFERENCED: " << *modified_for_control);

    BOOST_TEST(!(original_for_control->is_matchable_impl(*modified_for_control)));
    BOOST_TEST(!(modified_for_control->is_matchable_impl(*original_for_control)));
}
*/
