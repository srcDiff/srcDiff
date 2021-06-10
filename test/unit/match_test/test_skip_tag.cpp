#define BOOST_TEST_MODULE test skip_tag

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
    create_nodes("int decl_stmt", "C++"),
    create_nodes("int parameter", "C++"),
    create_nodes("int param", "C++"),
    create_nodes("int decl", "C++"),

};

const int start_pos[] = {

    0,
    1,
    1,
    4,
    4,

};

const std::string names[] = {

    "i",
    "decl_stmt",
    "parameter",
    "param",
    "decl",

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(names), node, start_pos, rhs) {

    std::cerr << node->at(start_pos)->name << ":" << start_pos << ":" << rhs << '\n';
    //std::cerr << skip_tag(node, start_pos) << '\n';
    skip_tag(*node, start_pos);
    //BOOST_TEST(skip_tag(*node, start_pos) == rhs);
}