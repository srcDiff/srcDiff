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

	create_nodes("traits::constr(alloc, p, 7);", "C++"),

	create_nodes("assert(std::equal(s.begin(), s.end(), s.c_str()));", "C++"),
	create_nodes("assert(std::equal(s.begin(), s.end(), s.c_str()));", "C++"),
	create_nodes("assert(std::equal(s.begin(), s.end(), s.c_str()));", "C++"),
	create_nodes("assert(std::equal(s.begin(), s.end(), s.c_str()));", "C++"),

	create_nodes("return a.operator+(b);", "C++"),
	create_nodes("return a.operator+(b);", "C++"),

	create_nodes("return X<typename Iter::value_type>(b, e);", "C++"),

	create_nodes("std::cin.clear();", "C++"),

	create_nodes("std::experimental::sample();", "C++"),

	create_nodes("ptr->at(0);", "C++"),

	create_nodes("ptr->a.b();", "C++"),

	create_nodes("ptr->a.b<foo>();", "C++"),

	create_nodes("c.ptr->nmspc_a::nmspc_b::a.b<foo>();", "C++"),

	create_nodes("auto previous = h.promise().previous;", "C++"),

	create_nodes("std::numeric_limits<float>::min();", "C++"),

	create_nodes("templated_fn<std::vector<int>>({1, 2, 3,});", "C++"),
};


const int start_positions[] = {

         2,    // <call>:2    b.Area(x, y)
	 3,    // <name>:3
	 
	 
	 2,    // <call>:2    a().b();
	 14,   // <call>:14
	 

	 2,    // <call>:2    v1.emplace<std::string>(\"def\");
	 

	 2,    // <call>:2    a<foo>().b<foo>()
	 27,   // <call>:27

	 
	 2,    // <call>:2    obj.func_name();

	 
	 2,    // <call>:2    a.b.func_name();

	 
	 2,    // <call>:2    a.b.c();

	 
	 2,    // <call>:2    a().b().c();

	 
	 4,    // <call>:4    return func(value);

	 
	 9,    // <call>:9    decltype(Default().foo()) n1 = 1;
	 21,   // <call>:21
	 

	 9,    // <call>:9    decltype(std::declval<NonDefault>().foo()) n2 = n1;
	 40,   // <call>:40
	 

	 2,    // <call>:2    traits::constr(alloc, p, 7)

	 
	 2,    // <call>:2    assert(std::equal(s.begin(), s.end(), s.c_str()));
	 26,   // <call>:26
	 49,   // <call>:49
	 72,   // <call>:72

	 
	 4,    // <call>:4    return a.operator+(b)
	 5,    // <name>:5

	 
	 4,    // <call>:4    return X<typename Iter::value_type>(b, e);

	 
	 2,    // <call>:2    std::cin.clear();

	 
	 2,    // <call>:2    std::experimental::sample();

	 
	 2,    // <call>:2    ptr->at(0);

	 
	 2,    // <call>:2    ptr->a.b();

	 
	 2,    // <call>:2    ptr->a.b<foo>();

	 
	 2,    // <call>:2    c.ptr->nmspc_a::nmspc_b::a.b<foo>();

	 
	 16,   // <call>:16   auto previous = h.promise().previous;

	 
	 2,    // <call>:2    std::numeric_limits<float>::min()

	 
	 2,    // <call>:2    templated_fn<std::vector<int>>({1, 2, 3,});	 
};


const string_vector_wrapper names_array[] = {

        std::vector<std::string>{"b", "Area"},             // <call>:2    b.Area(x, y)
	std::vector<std::string>{},                        // <name>:3

	
	std::vector<std::string>{"a"},                     // <call>:2    a().b();
	std::vector<std::string>{"b"},                     // <call>:14

	
	std::vector<std::string>{"v1", "emplace"},         // <call>:2    v1.emplace<std::string>(\"def\");

	
	std::vector<std::string>{"a"},                     // <call>:2    a<foo>().b<foo>()
	std::vector<std::string>{"b"},                     // <call>:27

	
	std::vector<std::string>{"obj", "func_name"},      // <call>:2    obj.func_name();

	
	std::vector<std::string>{"a", "b", "func_name"},   // <call>:2    a.b.func_name();

	
	std::vector<std::string>{"a", "b", "c"},           // <call>:2    a.b.c(); 

	
	std::vector<std::string>{"a"},                     // <call>:2    a().b().c(); 

	
	std::vector<std::string>{"func"},                  // <call>:4    return func(value);

	
	std::vector<std::string>{"Default"},               // <call>:9    decltype(Default().foo()) n1 = 1;
	std::vector<std::string>{"foo"},                   // <call>:21

	
	std::vector<std::string>{"std", "declval"},        // <call>:9    decltype(std::declval<NonDefault>().foo()) n2 = n1;
	std::vector<std::string>{"foo"},                   // <call>:40

	
	std::vector<std::string>{"traits", "constr"},      // <call>:2    traits::constr(alloc, p, 7)

	
	std::vector<std::string>{"assert"},                // <call>:2    assert(std::equal(s.begin(), s.end(), s.c_str()));
	std::vector<std::string>{"s", "begin"},            // <call>:26
	std::vector<std::string>{"s", "end"},              // <call>:49
	std::vector<std::string>{"s", "c_str"},            // <call>:72

	
	std::vector<std::string>{"a", "+"},                // <call>:4    return a.operator+(b)
	std::vector<std::string>{},                        // <name>:5

	
	std::vector<std::string>{"X"},                     // return X<typename Iter::value_type>(b, e);

	
	std::vector<std::string>{"std", "cin", "clear"},   // std::cin.clear();

	
	std::vector<std::string>{"std", "experimental",    // std::experimental::sample();
				 "sample"},

	
	std::vector<std::string>{"ptr", "at"},             // ptr->at(0);

	
	std::vector<std::string>{"ptr", "a", "b"},         // ptr->a.b();

	
	std::vector<std::string>{"ptr", "a", "b"},         // ptr->a.b<foo>();

	
	std::vector<std::string>{"c", "ptr", "nmspc_a",    // c.ptr->nmspc_a::nmspc_b::a.b<foo>();
				 "nmspc_b", "a", "b"},

	
	std::vector<std::string>{"h", "promise"},          // auto previous = h.promise().previous;

	
	std::vector<std::string>{"std", "numeric_limits"}, // std::numeric_limits<float>::min()

	
	std::vector<std::string>{"templated_fn"},          // templated_fn<std::vector<int>>({1, 2, 3,});
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_positions) ^ bu::make(names_array), node, start_pos, rhs) {
  
       BOOST_TEST(get_call_name(*node, start_pos) == rhs.names);
}
