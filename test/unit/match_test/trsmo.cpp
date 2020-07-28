#define BOOST_TEST_MODULE reject_similarity_test_only
#include <boost/test/included/unit_test.hpp>
#include <boost/bind.hpp>
#include <node_set.hpp>
#include <srcdiff_match.hpp>


BOOST_AUTO_TEST_SUITE(reject_similarity_test_only)
/*j
srcml_nodes testnodes1 = test_nodes;
srcml_nodes testnodes2 = test_nodes;
srcml_nodes testnodes3 = test_nodes;
node_set anode(testnodes1);
node_set anode2(testnodes2);
node_set anode3(testnodes3);

//Test for when both cases are not 
BOOST_AUTO_TEST_CASE(similar){
BOOST_TEST(!srcdiff_match::reject_similarity_test_only(anode,anode);
}

//Test for when both cases are different
BOOST_AUTO_TEST_CASE(different1){
BOOST_TEST(anode,anode2);
}

//Test for when both cases re different_2
BOOST_AUTO_TEST_CASE(different2){
BOOST_TEST(anode, anode3);
}*/

BOOST_AUTO_TEST_CASE(example){
BOOST_TEST(false);

}
BOOST_AUTO_TEST_SUITE_END()
