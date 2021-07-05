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


struct string_vector_wrapper {
  
        std::vector<std::string> names;
  
        string_vector_wrapper(const std::vector<std::string> & names) : names(names) {}

        friend std::ostream & operator<<(std::ostream & out, const string_vector_wrapper & wrapper) {
	  
                if(wrapper.names.empty()) return out;

                out << wrapper.names[0];
                for(size_t pos = 1; pos < wrapper.names.size(); ++pos) {
                        out << ", " << wrapper.names[pos];
                }
                return out;
        }
};


const std::shared_ptr<srcml_nodes> nodes[] = {

        create_nodes("b.Area(x, y);", "C++"),
	
	create_nodes("a().b();", "C++"),
	create_nodes("a().b();", "C++"),

	create_nodes("v1.emplace<std::string>(\"def\");", "C++"),

        create_nodes("a<foo>().b<foo>();", "C++"),
	create_nodes("a<foo>().b<foo>();", "C++"),

	create_nodes("obj.func_name();", "C++"),

	create_nodes("a.b.func_name();", "C++"),

	create_nodes("a.b.c();", "C++"),

	create_nodes("a().b().c();", "C++"),

	create_nodes("return func(value);", "C++"),

	create_nodes("decltype(Default().foo()) n1 = 1;", "C++"),
	create_nodes("decltype(Default().foo()) n1 = 1;", "C++"),

	create_nodes("decltype(std::declval<NonDefault>().foo()) n2 = n1;", "C++"),
	create_nodes("decltype(std::declval<NonDefault>().foo()) n2 = n1;", "C++"),

	create_nodes("traits_t::construct(alloc, p, 7);", "C++"),

	create_nodes("assert(std::equal(s.begin(), s.end(), s.c_str()));", "C++"),
	create_nodes("assert(std::equal(s.begin(), s.end(), s.c_str()));", "C++"),
	create_nodes("assert(std::equal(s.begin(), s.end(), s.c_str()));", "C++"),
	create_nodes("assert(std::equal(s.begin(), s.end(), s.c_str()));", "C++"),

	create_nodes("return a.operator+(b);", "C++"),
};


const int start_positions[] = {

         2,
	 
	 2,
	 14,

	 2,

	 2,
	 27,

	 2,

	 2,

	 2,

	 2,

	 4,

	 9,
	 21,

	 9,
	 40,

	 2,

	 2,
	 26,
	 49,
	 72,

	 4,
};


const string_vector_wrapper names_array[] = {

        std::vector<std::string>{"b", "Area"},
	
	std::vector<std::string>{"a"},
	std::vector<std::string>{"b"},

	std::vector<std::string>{"v1", "emplace"},

	std::vector<std::string>{"a"},
	std::vector<std::string>{"b"},

	std::vector<std::string>{"obj", "func_name"},

	std::vector<std::string>{"a", "b", "func_name"},

	std::vector<std::string>{"a", "b", "c"},

	std::vector<std::string>{"a"},

	std::vector<std::string>{"func"},

	std::vector<std::string>{"Default"},
	std::vector<std::string>{"foo"},

	std::vector<std::string>{"std", "declval"},
	std::vector<std::string>{"foo"},

	std::vector<std::string>{"traits_t", "construct"},

	std::vector<std::string>{"assert"},
	std::vector<std::string>{"s", "begin"},
	std::vector<std::string>{"s", "end"},
	std::vector<std::string>{"s", "c_str"},

	std::vector<std::string>{"a", "+"},
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_positions) ^ bu::make(names_array), node, start_pos, rhs) {
 
       BOOST_TEST(get_call_name(*node, start_pos) == rhs.names); 
}
