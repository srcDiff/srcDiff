#define BOOST_TEST_MODULE test name_list_similarity

#include <iostream>
#include <srcdiff_match.hpp>
#include <srcdiff_match_internal.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <vector>
#include <test_utils.hpp>

namespace bu = boost::unit_test::data;


struct string_vector_wrapper {
  
        std::vector<std::string> contents;
  
        string_vector_wrapper(const std::vector<std::string> & contents) : contents(contents) {}

        friend std::ostream & operator<<(std::ostream & out, const string_vector_wrapper & wrapper) {
	  
                if(wrapper.contents.empty()) return out;

                out << wrapper.contents[0];
                for(size_t pos = 1; pos < wrapper.contents.size(); ++pos) {
                        out << ", " << wrapper.contents[pos];
                }
                return out;
        }
};


const string_vector_wrapper original_name_lists[] = {

        std::vector<std::string>{"a", "Area"},  
							
};


const string_vector_wrapper modified_name_lists[] = {

        std::vector<std::string>{"b", "Area"}, 
							
};


const int similarities[] = {

        1,
			    
};


BOOST_DATA_TEST_CASE(passes, bu::make(original_name_lists) ^ bu::make(modified_name_lists) ^ bu::make(similarities), original, modified, rhs){

        BOOST_TEST(name_list_similarity(original.contents, modified.contents) == rhs);
	
}

