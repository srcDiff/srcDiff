#define BOOST_TEST_MODULE test for_control_matches

#include <iostream>
#include <srcdiff_match.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <string>
#include <test_utils.hpp>
#include <srcdiff_match_internal.hpp>

namespace bu = boost::unit_test::data;

const node_set_data original[] = {

    // Matches

    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),

};

const node_set_data modified[] = {

    // Matches

    create_node_set("for(int i = 0; i <= 10; i++) {}", "C++"),

};

const bool result[] = {

    // Matches
    
    true,

};

BOOST_DATA_TEST_CASE(passes, bu::make(original) ^ bu::make(modified) ^ bu::make(result), org, mod, rhs) {

    BOOST_TEST(for_control_matches(org.set, mod.set) == rhs);
}