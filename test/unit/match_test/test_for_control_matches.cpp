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


    // ===== Not Match =====

    // Different variable name / value
    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),
    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),

    // Different condition
    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),

    // Different increment
    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),

    // Invalid syntax
    create_node_set("for;", "C++"),
    create_node_set("foreach;", "C++"),

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


    // ===== Not Match =====

    // Different variable name / value
    create_node_set("for(int a = 0; a <= 10; a++) {}", "C++"),
    create_node_set("for(int i = 1; i <= 10; i++) {}", "C++"),

    // Different condition
    create_node_set("for(int i = 0; i > 10; i++) {}", "C++"),

    // Different increment
    create_node_set("for(int i = 0; i <= 10; i--) {}", "C++"),

    // Invalid syntax
    create_node_set("for;", "C++"),
    create_node_set("foreach;", "C++"),

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


    // ===== Not Match =====
    
    // Different variable name / value
    false,
    false,

    // Different condition
    false,

    // Different increment
    false,

    // Invalid syntax
    false,
    false,

};

BOOST_DATA_TEST_CASE(passes, bu::make(original) ^ bu::make(modified) ^ bu::make(result), org, mod, rhs) {

    BOOST_TEST(for_control_matches(org.set, mod.set) == rhs);
}