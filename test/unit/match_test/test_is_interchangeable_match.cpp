#define BOOST_TEST_MODULE test match
#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/bind.hpp>
#include <string>
#include <src_to_nodes.cpp>

namespace bu = boost::unit_test;

const p2nodes dinput1[] = {
ret_node_set_code("if(i <10){}","else if(i>5){}","C++"),
ret_node_set_code("if(i <10){}","int i = 20;","C++"),
ret_node_set_code("","","C++"),
ret_node_set_code("","","C++"),
}



BOOST_DATA_TEST_CASE(passes, bu::data::make(dinput1.nsone)^bu::data::make(dinput1.nstwo),d1,d2){
BOOST_TEST(srcdiff_match::is_interchangeable_match(d1,d2);
}

