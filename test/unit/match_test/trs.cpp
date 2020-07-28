#define BOOST_TEST_MODULE reject 
#include <boost/test/included/unit_test.hpp>
#include <boost/bind.hpp>
#include <srcml_converter.hpp>
#include <srcml.h>
#include <string>
#include <node_set.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_text_measure.hpp>





std::string testfile1 = "testfile1.cpp";
std::string testfile2 = "testfile2.cpp";


BOOST_AUTO_TEST_SUITE(reject_match)

BOOST_AUTO_TEST_CASE(example){
        srcml_nodes test_nodes1 = test_nodes(testfile1);
        srcml_nodes test_nodes2 = test_nodes(testfile2);
        node_set anode(test_nodes1);
        node_set anode2(test_nodes2);
        srcdiff_text_measure expr_measure(anode,anode2);
        expr_measure.compute();
	
	BOOST_TEST(srcdiff_match::reject_similarity(expr_measure,anode,anode2));
	//BOOST_TEST();
}BOOST_AUTO_TEST_SUITE_END()




