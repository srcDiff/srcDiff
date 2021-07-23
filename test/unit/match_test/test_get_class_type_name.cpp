#define BOOST_TEST_MODULE test get_class_type_name

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

       // class test cases
					     
       create_nodes("class Animal;", "C++"),
					      
       create_nodes("class Animal { string species; };", "C++"),
       create_nodes("class Animal { string species; };", "C++"),
       create_nodes("class Animal { string species; };", "C++"),
       create_nodes("class Animal { string species; };", "C++"),

       create_nodes("class A { class B { }; };", "C++"),
       create_nodes("class A { class B { }; };", "C++"),

       
       // enum test cases

       create_nodes("enum Color;", "C++"),
       
       create_nodes("enum Color { red, blue };", "C++"), 

       create_nodes("enum class Kind { None, A, B, Integer };", "C++"), 

       create_nodes("enum class Shape : uint8_t { circle = 0, };", "C++"),

       create_nodes("enum Type { new, old } c ;", "C++"),
       

       // struct test cases

       create_nodes("struct Employee;", "C++"),
       
       create_nodes("struct Employee { int age; };", "C++"),

       create_nodes("struct X { enum direction { left = 'l', right = 'r' }; };", "C++"),

       create_nodes("struct A { struct B { }; };", "C++"),
       create_nodes("struct A { struct B { }; };", "C++"),
       

       // union test cases
       
       create_nodes("union RecordType;", "C++"),

       create_nodes("union RecordType { };", "C++"),

       create_nodes("struct A { union B { }; };", "C++"),
       create_nodes("struct A { union B { }; };", "C++"),
};


const int start_pos[] = {

       // Class test cases
			 
       0,

       0,
       1,
       2,
       3,

       0,
       11,
       

       // enum test cases
       
       0,
       
       0,

       0,

       0,

       0,

       
       // struct test cases
       
       0,
       
       0,

       0,

       0,
       11,
       

       // union test cases
       
       0,

       0,

       0,
       11,
};


const std::string names[] = {

       // Class test cases
			     
       "Animal",
       
       "Animal",
       "",
       "",
       "",

       "A",
       "B",
       

       // enum test cases

       "Color",
       
       "Color", 

       "Kind",

       "Shape",

       "Type",
       

       // struct test cases

       "Employee",
       
       "Employee",

       "X",

       "A",
       "B",
       

       // union test cases
       
       "RecordType",

       "RecordType",

       "A",
       "B",
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(names), node, start_pos, rhs) {
  
       BOOST_TEST(get_class_type_name(*node, start_pos) == rhs);
}
