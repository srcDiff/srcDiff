#define BOOST_TEST_MODULE test get_condition

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

    // While
    create_nodes("while(found == false) {}", "C++"),
    create_nodes("while(true) {}", "C++"),
    create_nodes("while(1) {}", "C++"),
    create_nodes("while(i <= 10) {}", "C++"),

};

const int start_pos[] = {

    // While
    0,
    0,
    0,
    0,

};

const std::string result[] = {

    // While
    "found==false",
    "true",
    "1",
    "i<=10",

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(result), node, start_pos, rhs) {

    BOOST_TEST(get_condition(*node, start_pos) == rhs);
}