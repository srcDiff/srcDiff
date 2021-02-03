#define BOOST_TEST_MODULE test is_interchangeable_match

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <src_to_nodes.hpp>

namespace bu = boost::unit_test;

const node_set original_sets[] = {
    create_node_set("if(i < 10) {}", "C++"),
    create_node_set("if(i < 10) {}", "C++")
};


const node_set modified_sets[] = {
    create_node_set("else if(i>5){}","C++"),
    create_node_set("int i = 20;","C++")
};

BOOST_DATA_TEST_CASE(passes, bu::data::make(original_sets) ^ bu::data::make(modified_sets), original, modified){
    BOOST_TEST(srcdiff_match::is_interchangeable_match(original, modified));
}

