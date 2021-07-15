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
						
       create_nodes("class Animal { string species; };", "C++"),
       create_nodes("class Animal { string species; };", "C++"),
       create_nodes("class Animal { string species; };", "C++"),
       create_nodes("class Animal { string species; };", "C++"),
       
       create_nodes("enum Color { red, blue };", "C++"), // fails to get Color

       create_nodes("enum class Kind { None, A, B, Integer };", "C++"), // fails to get Kind

       create_nodes("enum class Shape : uint8_t;", "C++"), // Gets Shape just fine? <enum_decl>:0
       
       create_nodes("struct Employee { int age; };", "C++"),
       
       create_nodes("union RecordType { };", "C++"),
       
        
       
};


const int start_pos[] = {

       0,
       1,
       2,
       3,

       0, // Fails?

       0, // fails

       0,

       0,

       0,
};


const std::string names[] = {

       "Animal",
       "",
       "",
       "",

       "Color", // fails
       
       "Kind",  // fails

       "Shape",
       
       "Employee",
       
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
