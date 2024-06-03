#define BOOST_TEST_MODULE if_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <if.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "block";

std::vector<std::tuple<std::string>> test_cases_block_content = {
    {"{}"},
    {"{{}}"},
    {"{{{}}}"},
    {"{a=1;}"},
    {"{a=2;{}}"},
    {"{a=3;{a=4;}}"},
    {"{{a=333;{;}};;;;;;;};;"},
    {"{a=3;{a=5;{a=6;}}}"}
};

std::vector<std::tuple<std::string>> test_cases_is_syntax_similar_impl = {
    {"", ""}, //identical
    {}, //similar
    {}, //completely different
    {}, //TODO: add tests based on how the compute function in srcdif_syntax_measure works
};

std::vector<std::tuple<std::string>> test_cases_is_matchable_imple = {
    {""},
};
