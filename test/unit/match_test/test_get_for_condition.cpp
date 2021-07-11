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

    // For loop without initialization or increment
    create_nodes("for(; 1; ) {}", "C++"),

    // For loop without condition
    create_nodes("for(;;) {}", "C++"),

};

const int start_pos[] = {

    0,      // <for>:0
    0,      // <for>:0
    0,      // <for>:0
    0,      // <for>:0

    // For loop without initialization or increment
    0,      // <for>:0

    // For loop without condition
    0,      // <for>:0

};

const std::string result[] = {

    "i<10",
    "i<=10",
    "i>10",
    "i>=10",

    // For loop without initialization or increment
    "1",

    // For loop without condition
    "",

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(result), node, start_pos, rhs) {

    BOOST_TEST(get_for_condition(*node, start_pos) == rhs);
}
