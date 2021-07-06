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
};


const int start_pos[] = {

       0, 
};


const std::string names[] = {

       "Animal",
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(names), node, start_pos, rhs) {
  
       BOOST_TEST(get_class_type_name(*node, start_pos) == rhs);

       for(int i = 0; i < node->size(); ++i) {
             const auto & n = node->at(i);
             std::cerr << *n << " : " << i << '\n';     
       }
       std::cerr << "\n\n";
}
