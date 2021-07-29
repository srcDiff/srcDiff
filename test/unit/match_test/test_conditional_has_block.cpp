#define BOOST_TEST_MODULE test conditional_has_block

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <test_utils.hpp>
#include <srcdiff_match_internal.hpp>

namespace bu = boost::unit_test::data;

const node_set_data nodes[] = {

    // Conditional with block
    create_node_set("while(true) {}", "C++"),
    create_node_set("switch(value) {}", "C++"),
    create_node_set("do {} while(i <= 10);", "C++"),
    create_node_set("if(true) {}", "C++"),
    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),
    create_node_set("foreach(const QString &str, values) {}", "C++"),

    // Conditional without block
    create_node_set("while(x <= 10) cout << x;", "C++"),
    create_node_set("if(x <= 10) return true;", "C++"),
    create_node_set("for(int i = 0; i <= 10; i++) cout << i;", "C++"),
    create_node_set("foreach(const QString &str, values) cout << str;", "C++"),
    create_node_set("(value % 2 == 0) ? cout << \"even\" : cout << \"odd\";", "C++"),

};

const bool result[] = {

    // Conditional with block
    true,
    true,
    true,
    true,
    true,
    true,

    // Conditional without block
    false,
    false,
    false,
    false,
    false,

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(result), node, rhs) {

    BOOST_TEST(conditional_has_block(node.set) == rhs);
}