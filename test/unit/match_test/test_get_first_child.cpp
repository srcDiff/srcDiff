#define BOOST_TEST_MODULE test_get_first_child

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <test_utils.hpp>
#include <utility>
#include <srcdiff_match_internal.hpp>


namespace bu = boost::unit_test::data;


const node_set_data original_node_sets[] = {
						
	create_node_set("if(something){}", "C++"),

	create_node_set("if() i = 1;", "C++"),

	create_node_set("if constexpr (std::is_pointer_v<T>) return *t;", "C++"),

	create_node_set("if consteval { return true; } else { return false; }", "C++"),

	create_node_set("if not consteval { return true; } else { return false; }", "C++"),

	create_node_set("if() {} else {}", "C++"),

	create_node_set("if() {} else if(){}", "C++"),

	create_node_set("if() {} else if(){} else {}", "C++"),
};


const int child_start_positions[] = {
				   
        1,

        1,

        1,

        1,

        1,

        1,

        1,

        1,
	
};


const int child_end_positions[] = {
				   
        18,

        25,

        50,

        19,

        20,

        13,

        13,

        13,	
};


const int child_node_lengths[] = {

        18,

	25,

	50,

	19,

	20,

	13,

	13,

	13,

};


BOOST_DATA_TEST_CASE(passes, bu::make(original_node_sets) ^ bu::make(child_start_positions) ^ bu::make(child_end_positions) ^ bu::make(child_node_lengths), original, child_start, child_end, child_length) {

  BOOST_TEST((get_first_child(original.set).get_node_name(0) == original.set.get_node_name(child_start))
	     & (get_first_child(original.set).get_node_name(get_first_child(original.set).size()-1) == original.set.get_node_name(child_end))
	     & (get_first_child(original.set).size() == child_length));
}


