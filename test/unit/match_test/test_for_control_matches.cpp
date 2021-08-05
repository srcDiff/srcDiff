#define BOOST_TEST_MODULE test for_control_matches

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <test_utils.hpp>
#include <srcdiff_match_internal.hpp>

namespace bu = boost::unit_test::data;

const node_set_data original[] = {

    // ===== Match =====

    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),

    // For loops without initialization, increment, and/or condition
    create_node_set("for(; 1; ) {}", "C++"),
    create_node_set("for(;;) {}", "C++"),

    // For loop with ternary condition
    create_node_set("for(int i = 0; (i < 10) ? true : false; i++) {}", "C++"),

    // Range based for
    create_node_set("for(const int& i : nums) {}", "C++"),
    create_node_set("for(auto i : nums) {}", "C++"),

    // QT foreach
    create_node_set("foreach(const QString &str, values) {}", "C++"),

};

const node_set_data modified[] = {

    // ===== Match =====

    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),

    // For loops without initialization, increment, and/or, condition
    create_node_set("for(; 1; ) {}", "C++"),
    create_node_set("for(;;) {}", "C++"),

    // For loop with ternary condition
    create_node_set("for(int i = 0; (i < 10) ? true : false; i++) {}", "C++"),

    // Range based for
    create_node_set("for(const int& i : nums) {}", "C++"),
    create_node_set("for(auto i : nums) {}", "C++"),

    // QT foreach
    create_node_set("foreach(const QString &str, values) {}", "C++"),

};

const bool result[] = {

    // ===== Match =====

    true,

    // For loops without initialization, increment, and/or condition
    true,
    true,

    // For loop with ternary condition
    true,

    // Range based for
    true,
    true,

    // QT foreach
    true,

};

BOOST_DATA_TEST_CASE(passes, bu::make(original) ^ bu::make(modified) ^ bu::make(result), org, mod, rhs) {

    BOOST_TEST(for_control_matches(org.set, mod.set) == rhs);
}