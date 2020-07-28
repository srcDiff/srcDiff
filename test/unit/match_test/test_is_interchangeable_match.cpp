#define BOOST_TEST_MODULE test match
#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/bind.hpp>
#include <string>
BOOST_AUTO_TEST_SUITE(test_match)

BOOST_AUTO_TEST_CASE(uri_match){
BOOST_TEST(!srcdiff_match::is_interchangeable_match("first", "first", "first", "second"));
}

BOOST_AUTO_TEST_CASE(IF_and_Namespace){
BOOST_TEST(!srcdiff_match::is_interchangeable_match("if", "random","random", "random"));
}

BOOST_AUTO_TEST_CASE(IF_and_Namespace2){
BOOST_TEST(!srcdiff_match::is_interchangealbe_match("random", "random", "if", "random"));
}

BOOST_AUTO_TEST_CASE(Interchange_match1){
BOOST_TEST(srcdiff_match::is_interchageable_match("else", "http://www.srcML.org/srcML/src","elseif", "http://www.srcML.org/srcML/src"));
}

BOOST_AUTO_TEST_CASE(Interchange_match2){
BOOST_TEST(!srcdiff_match::is_interchangeable_match("for", "http://www.srcML.org/srcML/src", "random", "http://www.srcML.org/srcML/src");
}
BOOST_AUTO_TEST_SUITE_END()


