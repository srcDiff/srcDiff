#define BOOST_TEST_MODULE test extract_name

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <test_utils.hpp>
#include <srcdiff_match_internal.hpp>


namespace bu = boost::unit_test::data;




//=========================================================================
//===================== passing test set Start here =======================
//=========================================================================




	
const std::shared_ptr<srcml_nodes> nodes[] = {
	
	
	create_nodes("int i", "C++"),
	create_nodes("int i", "C++"),
	create_nodes("int i", "C++"),
	create_nodes("int i", "C++"),
	
	create_nodes("int decl_stmt;", "C++"),
	create_nodes("int parameter;", "C++"),
	create_nodes("int param;", "C++"),
	create_nodes("int decl;", "C++"),

	create_nodes("const std::shared_ptr<srcml_nodes> nodes[] = {", "C++"),
	
	create_nodes("const", "C++"),
	create_nodes("const", "C++"),
	create_nodes("const", "C++"),
	create_nodes("const", "C++"),

	create_nodes("int numbers[5] = { 1, 2, 3, 4, 5 };", "C++"),

	create_nodes("int numbers[5];", "C++"),
	create_nodes("int numbers[5];", "C++"),
	
};



const int start_pos[] = {
	

	0,
	1,
	2,
	3,
	
	1,
	1,
	4,
	4,
	
	2,
	
	0,
	1,
	2,
	3,

	0,
	
	7,
        0,
	
};




const std::string names[] = {


	"i",
	"int i",
	"int i",
	"int i",
	
	"decl_stmt",
	"parameter",
	"param",
	"decl",

	"",
	
	"",
	"",
	"",
	"",

	"int",
	
	"ee",
	"ww",
	
};



BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(names), node, start_pos, rhs) {

  
	std::cerr << node->at(start_pos)->name << ":" << start_pos << ":" << rhs << "\n\n";

    BOOST_TEST(extract_name(*node, start_pos) == rhs);
}









