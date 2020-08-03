#define BOOST_TEST_MODULE test_reject_match 
#include <boost/test/included/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <srcml_converter.hpp>
#include <srcml.h>
#include <string>
#include <node_set.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_text_measure.hpp>
#include <src_to_nodes.cpp>

namespace bu = boost::unit_test;

node_set return_Node(std::string, std::string);
std::string testfile1 = "testfile.cpp";
std::string testfile2 = "testfile2.cpp";
std::string testfile3 = "testfile3.cpp";
std::string testfile4 = "testfile4.cpp";
std::string testfile5 = "testfile5.cpp";
std::string testfile6 = "testfile6.cpp";


const srcml_nodes datainput[] = {
create_nodes(testfile1,"C++"),
create_nodes(testfile2,"C++"),
create_nodes(testfile3,"C++"),
};

const srcml_nodes datainput2[] = {
create_nodes(testfile4, "C++"),
create_nodes(testfile5, "C++"),
create_nodes(testfile6, "C++"),
};

BOOST_DATA_TEST_CASE(passes,bu::data::make(dataInput)^ bu::data::make(dataInput2), d1,d2){
	srcdiff_text_measure ameasure(d1, d2);
	BOOST_TEST(srcdiff_match::reject_match(ameasure, d1,d2);
		}
