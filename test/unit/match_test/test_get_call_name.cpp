#define BOOST_TEST_MODULE test get_call_name

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
	
};


const int start_pos[] = {

       	0,
	
};


const std::vector<std::string> names[] = {

        {"ee", "ee"},
			     
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(names), node, start_pos, rhs) {
  
       BOOST_TEST(get_call_name(*node, start_pos) == rhs);
  
       std::cerr << node->at(start_pos)->name << ":" << start_pos; // << ":"  << rhs << "\n\n";
}
