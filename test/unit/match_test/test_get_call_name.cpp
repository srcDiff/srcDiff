#define BOOST_TEST_MODULE test get_call_name

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <vector>
#include <test_utils.hpp>
#include <srcdiff_match_internal.hpp>


namespace bu = boost::unit_test::data;


const std::shared_ptr<srcml_nodes> nodes[] = {

        create_nodes("b.Area(x, y);", "C++"),
	
};


const int start_positions[] = {

       	2,
	
};


const std::vector<std::string> name_vectors[] = {

        std::vector<std::string>{"b", "Area"},
						 
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_positions) ^ bu::make(name_vectors), node, start_pos, rhs) {
  
       BOOST_TEST(get_call_name(*node, start_pos) == rhs);
  
       std::cerr << node->at(start_pos)->name << ":" << start_pos; // << ":"  << rhs << "\n\n";
}
