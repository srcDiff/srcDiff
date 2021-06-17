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
    create_nodes("int main { int num = 0; }", "C++"),

    // Nested - Skip Outer Tag
    create_nodes("if(x >= 10) { if(y <= 10) {} }", "C++"),
    create_nodes("for(int i = 0; i < count; ++i) { for(int i = 0; i < count; ++i) {} }", "C++"),
    create_nodes("while(true) { while(true) {} }", "C++"),

    // Nested - Skip Inner Tag
    create_nodes("if(x >= 10) { if(y <= 10) {} }", "C++"),
    create_nodes("for(int i = 0; i < count; ++i) { for(int i = 0; i < count; ++i) {} }", "C++"),
    create_nodes("while(true) { while(true) {} }", "C++"),

    // Skip Empty Tag
    create_nodes("for(; 1; ) {}", "C++"),
};

const int start_pos[] = {

    0,      // <decl>:0, skip to '':11
    1,      // <decl>:1, skip to </decl_stmt>:37

    // Nested - Skip Outer Tag
    1,      // <if>:1, skip to </if_stmt>:59
    0,      // <for>:0, skip to '':134
    0,      // <while>:0, skip to '':40

    // Nested - Skip Inner Tag
    26,     // <if>:26, skip to </if_stmt>:53
    63,     // <for>:63, skip to ' ':129
    16,     // <while>:16, skip to ' ':35

    // Skip Empty Tag
    17,     // <incr/>:17, should skip <incr>:17 to ')':19

};

const int end_pos[] = {

    11,     // '':11
    37,     // </decl_stmt>:37

    // Nested - Skip Outer Tag
    59,     // </if_stmt>:59
    134,    // '':134
    40,     // '':40

    // Nested - Skip Inner Tag
    53,     // </if_stmt>:53
    129,    // ' ':129
    35,     // ' ':35

    // Skip Empty Tag
    19,     // ')':19

};

BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(end_pos), node, start_pos, rhs) {
    int start = start_pos;

    for(int i = 0; i < node->size(); ++i) {
        const auto & n = node->at(i);
        std::cerr << *n << " : " << i << '\n';
    }
    std::cerr << "\n\n";

    skip_tag(*node, start);
    
    BOOST_TEST(start == rhs);
}