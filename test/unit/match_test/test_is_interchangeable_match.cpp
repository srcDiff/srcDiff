#define BOOST_TEST_MODULE test is_interchangeable_match

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <src_to_nodes.hpp>

namespace bu = boost::unit_test;

const test_data_t data[] = {
    create_node_set_code("if(i <10){}","else if(i>5){}","C++"),
    create_node_set_code("if(i <10){}","int i = 20;","C++"),
    create_node_set_code("","","C++"),
    create_node_set_code("","","C++"),
};


BOOST_DATA_TEST_CASE(passes, bu::data::make(data), input){
    BOOST_TEST(srcdiff_match::is_interchangeable_match(input.node_set_one, input.node_set_two));
}

