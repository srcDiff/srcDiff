/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
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

	
const std::shared_ptr<srcml_nodes> nodes[] = {
	
	
	create_nodes("int i", "C++"),
	
	create_nodes("int i;", "C++"),

	create_nodes("a::b", "C++"),
        create_nodes("a::b", "C++"),
	create_nodes("a::b", "C++"),

	create_nodes("a::b;", "C++"),

	create_nodes("const int NUM = 6;", "C++"),
	create_nodes("const int NUM = 6;", "C++"),
	create_nodes("const int NUM = 6;", "C++"),
	create_nodes("const int NUM = 6;", "C++"),

	create_nodes("*number1 = * number2;", "C++"),
	create_nodes("*number1 = * number2;", "C++"),
	create_nodes("*number1 = * number2;", "C++"),
	create_nodes("*number1 = * number2;", "C++"),
	create_nodes("*number1 = * number2;", "C++"),
	create_nodes("*number1 = * number2;", "C++"),

	create_nodes("object_name.param_name = 5;", "C++"),
	create_nodes("object_name.param_name = 5;", "C++"),
	create_nodes("object_name.param_name = 5;", "C++"),
	
	create_nodes("object_name.obj_array[3] = 4;", "C++"),
	create_nodes("object_name.obj_array[3] = 4;", "C++"),
	create_nodes("object_name.obj_array[3] = 4;", "C++"),
	create_nodes("object_name.obj_array[3] = 4;", "C++"),

	create_nodes("struct name_of_struct { }", "C++"),
	create_nodes("struct name_of_struct { }", "C++"),
	create_nodes("struct name_of_struct { }", "C++"),

	create_nodes("const auto & n = node->at(i);", "C++"), 
	create_nodes("const auto & n = node->at(i);", "C++"),  
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),

	create_nodes("const auto & n = node->at(i);", "C++"),
        create_nodes("const auto & n = node->at(i);", "C++"),

	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),

	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),

	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),

	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),

	create_nodes("int b = a.size();", "C++"),
	create_nodes("int b = a.size();", "C++"),
	create_nodes("int b = a.size();", "C++"),
	create_nodes("int b = a.size();", "C++"),
};


const int start_pos[] = {

        1, // int i
	
	7, // int i;

	0, // a::b
	1,
	7,
	
	1, // a::b;
	
	4, // const int NUM = 6;
	1,
	2,
	0,

	0, // *number1 = * number2;
	1,
	2,
	3,
	4,
	5,

	0, // object_name.param_name = 5;
	1,
	2,

	0, // object_name.obj_array[3] = 4
	1,
	2,
	3,

	0, // struct name_of_struct { }
	1,
	2,

	0, // const auto & n = node->at(i)
	1,
	2,
	3,
	4,
	
	5,
	6,

	11,
	12,
	13,
	14,
	15,

	16,
	17, 
	18,  

	19,  
	20,
	21,
	22,
	
	23,
	24,
	25,
	26,
	
	27,
	28,

	36,
	37,

	0, // int b = a.size();
	1,
	2,
	6,
};


const std::string names[] = {
	
	"int",
	"",

	"a::b",
	"a",
	"",

	"a::b",
	
        "",
	"NUM",
	"int",
	"",

	"",
	"number1",
	"",
	"",
	"",
	"",
	
	"",
	"object_name.param_name",
	"object_name",

	"",
	"object_name.obj_array[3]",
	"object_name",
	"",

	"name_of_struct",
        "",
	"",

	"",  // Position 0 in const auto & n = node->at(i);
	"n", 
	"", 
	"",
	"",
	"",
	"",
	
	"",  // 11
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"", // 19
	"",
	"",
	"",

	"", // 23
	"node->at",
	"node",
	"",
	
	"", // 27 
	"",

	"",  // 36
	"",

	"",
	"b",
	"int",
	"",
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(names), node, start_pos, rhs) {

	BOOST_TEST(extract_name(*node, start_pos) == rhs);
  
	std::cerr << node->at(start_pos)->name << ":" << start_pos << ":" << rhs << "\n\n";
}









