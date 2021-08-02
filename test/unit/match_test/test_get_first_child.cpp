#define BOOST_TEST_MODULE test_get_first_child

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <test_utils.hpp>
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


const std::string child_starting_node_tags[] = {

	"if",

	"if",

	"if",

	"if",

	"if",

	"if",

	"if",

	"if",
};


BOOST_DATA_TEST_CASE(passes, bu::make(original_node_sets) ^ bu::make(child_starting_node_tags), original, child_start_tag) {
  
        BOOST_TEST(get_first_child(original.set).get_node_name(0) == child_start_tag);
}


