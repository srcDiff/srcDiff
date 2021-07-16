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

       
       // enum test cases

       create_nodes("enum Color;", "C++")+, // Probably supposed to work for enum but doesnt?
       
       create_nodes("enum Color { red, blue };", "C++"), 

       create_nodes("enum class Kind { None, A, B, Integer };", "C++"), 

       create_nodes("enum class Shape : uint8_t { circle = 0, };", "C++"),
       

       // struct test cases
       
       create_nodes("struct Employee { int age; };", "C++"),
       

       // union test cases
       
       create_nodes("union RecordType { };", "C++"),
    
};


const int start_pos[] = {

       // Class test cases
			 
       0,

       0,
       1,
       2,
       3,
       

       // enum test cases
       
       0,
       
       0,

       0,

       0,

       
       // struct test cases
       
       0,
       

       // union test cases
       
       0,
};


const std::string names[] = {

       // Class test cases
			     
       "Animal",
       
       "Animal",
       "",
       "",
       "",
       

       // enum test cases

       "",
       
       "", 

       "",

       // struct test cases
       
       "Employee",
       

       // union test cases
       
       "RecordType",

};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(names), node, start_pos, rhs) {
  
       BOOST_TEST(get_class_type_name(*node, start_pos) == rhs);

       for(int i = 0; i < node->size(); ++i) {
             const auto & n = node->at(i);
             std::cerr << *n << " : " << i << '\n';     
       }
       std::cerr << "\n\n";
}
