#define BOOST_TEST_MODULE test name_list_similarity

#include <iostream>
#include <srcdiff_match.hpp>
#include <srcdiff_match_internal.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <vector>
#include <utility>
#include <test_utils.hpp>

namespace bu = boost::unit_test::data;

const string_vector_wrapper name_pairs[] = {

	std::make_pair(std::vector<std::string>{},
		       std::vector<std::string>{}),
	
	std::make_pair(std::vector<std::string>{},
		       std::vector<std::string>{"one", "two"}),
	
        std::make_pair(std::vector<std::string>{"one", "two"},
		       std::vector<std::string>{}),

	std::make_pair(std::vector<std::string>{"a", "Area"},
		       std::vector<std::string>{"b", "Area"}),

	std::make_pair(std::vector<std::string>{"a", "b", "c"},
		       std::vector<std::string>{"a", "b", "d", "d"}),
	
	std::make_pair(std::vector<std::string>{"a", "b", "c"},
		       std::vector<std::string>{"a ", "b", "d", "d"}),
	
	std::make_pair(std::vector<std::string>{"b", "b", "a"},
		       std::vector<std::string>{"a", "b", "z"}),
	
	std::make_pair(std::vector<std::string>{"1", "2", "3", "4"},
		       std::vector<std::string>{"1", "2", "3", "4"}),
	
	
	std::make_pair(std::vector<std::string>{"1", "2", "3", "4", "5",
	                                        "6", "7", "8", "9", "10",
	                                        "11", "12", "13", "14", "15",
	                                        "16", "17", "18", "19", "20"},
	  
	               std::vector<std::string>{"1", "2", "3", "4", "5",
						"6", "7", "8", "9", "10",
						"11", "12", "13", "14", "15",
						"16", "17", "18", "19", "20"}),
	
	
	std::make_pair(std::vector<std::string>{"1", "2", "3", "4", "5",
						"6", "7", "8", "9", "10",
						"11", "12", "13", "14", "15",
						"16", "17", "18", "19", "20"},
	  
	               std::vector<std::string>{"2", "3", "4", "5",
						"6", "7", "8", "9", "10",
						"11", "12", "13", "14", "15",
						"16", "17", "18", "19", "20"}),
	
	
	std::make_pair(std::vector<std::string>{"int z = 4;", "string name;",
                                               "for(int i; i < 10; i++){ cout << i; }"},
	  
	               std::vector<std::string>{"int z = 5;", "string name;",
					        "for(int j; j < 15; j++){ cout << j; }"}),
	

	std::make_pair(std::vector<std::string>{"a", "b", "c", "d", "e"},
		       std::vector<std::string>{"a", "bc", "c", "d"}),

	std::make_pair(std::vector<std::string>{"a", "b", "c", "d", "e", "f"},
		       std::vector<std::string>{"a", "b", "c"}),

	std::make_pair(std::vector<std::string>{"1", "3", "5"},
		       std::vector<std::string>{"1", "2", "3", "4", "5"}),

	std::make_pair(std::vector<std::string>{"-1", "1", "3", "5"},
		       std::vector<std::string>{"0", "1", "2", "3", "4", "5"}),

	std::make_pair(std::vector<std::string>{"int i = 0;", "int p = 1;",
						"int w = 3;", "int c = 9;"},
	  
	               std::vector<std::string>{"int i = 0;", "bool isThere = false;",
						"bool changed = false;", "int p = 1;",
						"int w = 3;", "char grade = 'a';",
						"int c = 9;"}),

	std::make_pair(std::vector<std::string>{"a", "b", "c", "d", "e", "f"},
		       std::vector<std::string>{"a", "f", "b", "c", "d", "e"}),

	std::make_pair(std::vector<std::string>{"a", "f"},
		       std::vector<std::string>{"a", "b", "c", "d", "e", "f"}),

	std::make_pair(std::vector<std::string>{"1", "2", "3", "4", "5"},
		       std::vector<std::string>{"5", "5", "4", "3", "2", "2", "2", "1"}),

	std::make_pair(std::vector<std::string>{"1", "2", "3", "4", "5"},
		       std::vector<std::string>{"5", "4", "3", "2", "1"}),
};


const int similarities[] = {

        0,

	0,

	0,
			    
        1,

	2,

	1,

	1,

	4,

	20,

	19,

	1,

	3,

	3,

	3,

	3,

	4,

        5,

        2,

	1,

	1,
};


BOOST_DATA_TEST_CASE(passes, bu::make(name_pairs) ^ bu::make(similarities), pair, original, modified, rhs){

        BOOST_TEST(name_list_similarity(pair.first, pair.second) == rhs);
	
}

