#define BOOST_TEST_MODULE test get_name

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

	create_nodes("public foo:", "C++"),

	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
};

const int names_start_pos[] = {
	
         3,  // int i;
	 8,
	 
	 3,  // public foo:
	 
	 13, // foreach (int i = 0; i < count; ++i) {}(
	 40,

	 
};

const std::string names[] = {

	"int",
	"i",
	
        "foo",
	
	"i",
	"count",
};



BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(names_start_pos) ^ bu::make(names), node, names_start_pos, rhs) {
 
        //std::cerr << node << ":" << names_start_pos << ":" << rhs << '\n';

        BOOST_TEST(get_name(*node, names_start_pos) == rhs);

	std::cerr << "\n" << get_name(*node, names_start_pos) << "\n\n";

}



