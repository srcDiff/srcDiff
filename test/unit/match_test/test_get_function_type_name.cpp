#define BOOST_TEST_MODULE test get_function_type_name

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

    // Function
    create_nodes("int main() {}", "C++"),
    create_nodes("int foo::sum(int a, int b) { return a + b; }", "C++"),

    // Function decl
    create_nodes("std::string foo();", "C++"),
    create_nodes("void bar();", "C++"),
    create_nodes("int foo::sum(int a, int b);", "C++"),

    // Constructor
    create_nodes("class Animal { public: int age; Animal() { age = 10;} };", "C++"),
    create_nodes("Animal::Animal() {age = 10;}", "C++"),

    // Constructor decl
    create_nodes("class Animal { public: int age; Animal(); }", "C++"),
    create_nodes("Animal::Animal();", "C++"),

    // Destructor
    create_nodes("class Animal { public: int age; ~Animal() {} };", "C++"),
    create_nodes("Animal::~Animal() {}", "C++"),

    // Destructor decl
    create_nodes("class Animal { public: int age; ~Animal(); };", "C++"),
    
    // Operator
    create_nodes("operator void*() const {}", "C++"),
    create_nodes("explict operator bool() const {}", "C++"),
    create_nodes("friend bool operator==() {}", "C++"),

    // Operator decl
    create_nodes("operator void*() const;", "C++"),
    create_nodes("explict operator bool() const;", "C++"),
    create_nodes("friend bool operator==();", "C++"),

    // Incorrect start position
    create_nodes("int sum(int x, int y) { return x + y; }", "C++"),
    create_nodes("int sum(int x, int y) { return x + y; }", "C++"),
    create_nodes("int sum(int x, int y) { return x + y; }", "C++"),

};

const int start_pos[] = {

    // Function
    0,      // <function>:0
    0,      // <function>:0

    // Function decl
    0,      // <function_decl>:0
    0,      // <function_decl>:0
    0,      // <function_decl>:0

    // Constuctor
    30,     // <constructor>:30
    0,      // <constructor>:0

    // Constructor decl
    30,     // <constructor_decl>:30
    0,      // <constructor_decl>:0

    // Destructor
    30,     // <destructor>:30
    0,      // <destructor>:0

    // Destructor decl
    30,     // <destructor_decl>:30

    // Operator
    0,      // <function type="operator">:0
    0,      // <function type="operator">:0
    3,      // <function type="operator">:3

    // Operator decl
    0,      // <function_decl type="operator">:0
    0,      // <function_decl type="operator">:0
    3,      // <function_decl type="operator">:3

    // Incorrect start position
    1,      // <type>:1
    3,      // 'int':3
    4,      // </name>:4

};

const std::string function_type_name[] = {

    // Function
    "main",
    "foo::sum",

    // Function decl
    "foo",
    "bar",
    "foo::sum",

    // Constructor
    "Animal",
    "Animal::Animal",

    // Constructor decl
    "Animal",
    "Animal::Animal",

    // Destructor
    "~Animal",
    "Animal::~Animal",

    // Destructor decl
    "~Animal",

    // Operator
    "operatorvoid*",
    "operatorbool",
    "operator==",

    // Operator decl
    "operatorvoid*",
    "operatorbool",
    "operator==",

    // Incorrect start position
    "",
    "",
    "",

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(function_type_name), node, start_pos, rhs) {

    int start = start_pos;

    BOOST_TEST(get_function_type_name(*node, start) == rhs);
}

