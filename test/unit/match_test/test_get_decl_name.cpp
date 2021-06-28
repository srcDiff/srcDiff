#define BOOST_TEST_MODULE test get_decl_name

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
						
       create_nodes("int i;", "C++"),
       create_nodes("int i;", "C++"),
       create_nodes("int i;", "C++"),
       create_nodes("int i;", "C++"),

       create_nodes("void func_name(int a) { }", "C++"),
       create_nodes("void func_name(int a) { }", "C++"),
       create_nodes("void func_name(int a) { }", "C++"),

       create_nodes("void some_func(int number);", "C++"),
       create_nodes("void some_func(int number);", "C++"),

       create_nodes("int array_name[2] = { 1, 2 };", "C++"),
       create_nodes("int array_name[2] = { 1, 2 };", "C++"),
       create_nodes("int array_name[2] = { 1, 2 };", "C++"),
       create_nodes("int array_name[2] = { 1, 2 };", "C++"),

       create_nodes("string a = b;", "C++"),
       create_nodes("string a = b;", "C++"),

       create_nodes("Class_Name object_name;", "C++"),
       create_nodes("Class_Name object_name;", "C++"),

       create_nodes("enum color { red };", "C++"),

       create_nodes("class Animal { string str_name; };", "C++"),
       create_nodes("class Animal { string str_name; };", "C++"),
};


const int start_pos[] = {

       0,  // int i;
       1,
       2,
       11,

       12, // void func_name(int a) { }
       13,
       15,

       12,  // void some_func(int number)
       13,

       0,  // int array_name[2] = { 1, 2 }
       1,
       2,
       3,

       0,  // string a = b
       1,

       0,  // Class_Name object_name;
       1,

       10, // enum color { red };

       11, // class Animal { string str_name; }
       12,
};


const std::string names[] = {

       "i",
       "i",
       "",
       "",

       "a",
       "a",
       "",

       "number",
       "number",

       "array_name[2]",
       "array_name[2]",
       "",
       "",

       "a",
       "a",

       "object_name",
       "object_name",

       "red",
       
       "str_name",
       "str_name",
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(names), node, start_pos, rhs) {
  
       BOOST_TEST(get_decl_name(*node, start_pos) == rhs);
  
       std::cerr << node->at(start_pos)->name << ":" << start_pos << ":" << rhs << "\n\n";
}
