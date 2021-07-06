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
};


const string_vector_wrapper names_array[] = {

        std::vector<std::string>{"b", "Area"},           // <call>:2    b.Area(x, y)
	std::vector<std::string>{},                      // <name>:3

	
	std::vector<std::string>{"a"},                   // <call>:2    a().b();
	std::vector<std::string>{"b"},                   // <call>:14

	
	std::vector<std::string>{"v1", "emplace"},       // <call>:2    v1.emplace<std::string>(\"def\");

	
	std::vector<std::string>{"a"},                   // <call>:2    a<foo>().b<foo>()
	std::vector<std::string>{"b"},                   // <call>:27

	
	std::vector<std::string>{"obj", "func_name"},    // <call>:2    obj.func_name();

	
	std::vector<std::string>{"a", "b", "func_name"}, // <call>:2    a.b.func_name();

	
	std::vector<std::string>{"a", "b", "c"},         // <call>:2    a.b.c(); 

	
	std::vector<std::string>{"a"},                   // <call>:2    a().b().c(); 

	
	std::vector<std::string>{"func"},                // <call>:4    return func(value);

	
	std::vector<std::string>{"Default"},             // <call>:9    decltype(Default().foo()) n1 = 1;
	std::vector<std::string>{"foo"},                 // <call>:21

	
	std::vector<std::string>{"std", "declval"},      // <call>:9    decltype(std::declval<NonDefault>().foo()) n2 = n1;
	std::vector<std::string>{"foo"},                 // <call>:40

	
	std::vector<std::string>{"traits", "constr"},    // <call>:2    traits::constr(alloc, p, 7)

	
	std::vector<std::string>{"assert"},              // <call>:2    assert(std::equal(s.begin(), s.end(), s.c_str()));
	std::vector<std::string>{"s", "begin"},          // <call>:26
	std::vector<std::string>{"s", "end"},            // <call>:49
	std::vector<std::string>{"s", "c_str"},          // <call>:72

	
	std::vector<std::string>{"a", "+"},              // <call>:4    return a.operator+(b)
	std::vector<std::string>{},                      // <name>:5
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_positions) ^ bu::make(names_array), node, start_pos, rhs) {
 
       BOOST_TEST(get_call_name(*node, start_pos) == rhs.names);
}
