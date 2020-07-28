#define BOOST_TEST_MODULE test_reject_match 
#include <boost/test/included/unit_test.hpp>
#include <boost/bind.hpp>
#include <srcml_converter.hpp>
#include <srcml.h>
#include <string>
#include <node_set.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_text_measure.hpp>
#include <src_to_nodes.cpp>

node_set return_Node(std::string, std::string);
std::string testfile1 = "testfile.cpp";
std::string testfile2 = "testfile2.cpp";



BOOST_AUTO_TEST_SUITE(reject_match)

BOOST_AUTO_TEST_CASE(example){
p2test atest = ret_node_set(testfile1, testfile2,"C++");	
	
BOOST_TEST(srcdiff_match::reject_match(atest.one, atest.two,atest.ameasure));
}





BOOST_AUTO_TEST_CASE(ex2){ BOOST_TEST(false);}

BOOST_AUTO_TEST_SUITE_END()


	
