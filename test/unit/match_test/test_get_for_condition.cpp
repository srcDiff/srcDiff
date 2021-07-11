#define BOOST_TEST_MODULE test get_for_condition

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <test_utils.hpp>
#include <srcdiff_match_internal.hpp>

namespace bu = boost::unit_test::data;

const std::shared_ptr<srcml_nodes> nodes[] = {

    create_nodes("for(int i = 0; i < 10; i++) {}", "C++"),
    create_nodes("for(int i = 0; i <= 10; i++) {}", "C++"),
    create_nodes("for(int i = 20; i > 10; i--) {}", "C++"),
    create_nodes("for(int i = 20; i >= 10; i--) {}", "C++"),

};

const int start_pos[] = {

    0,      // <for>:0
    0,      // <for>:0
    0,      // <for>:0
    0,      // <for>:0

};

const std::string result[] = {

    "i<10",
    "i<=10",
    "i>10",
    "i>=10",

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(result), node, start_pos, rhs) {

    BOOST_TEST(get_for_condition(*node, start_pos) == rhs);
}
