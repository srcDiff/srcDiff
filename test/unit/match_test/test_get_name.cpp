#define BOOST_TEST_MODULE test get_name

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
					      
        create_nodes("int i;", "C++"),
	create_nodes("int i;", "C++"),

	create_nodes("int first_val = 10, second_val = 20;", "C++"),
	create_nodes("int first_val = 10, second_val = 20;", "C++"),
	create_nodes("int first_val = 10, second_val = 20;", "C++"),

	create_nodes("public foo:", "C++"),

	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),

	create_nodes("a::b;", "C++"),
	create_nodes("a::b;", "C++"),
	create_nodes("a::b;", "C++"),
	
	create_nodes("a::b", "C++"),
	create_nodes("a::b", "C++"),
	create_nodes("a::b", "C++"),

        create_nodes("array[size];", "C++"),
	create_nodes("array[size];", "C++"),
	create_nodes("array[size];", "C++"),

        create_nodes("object_name.obj_array[3] = 4;", "C++"),
	create_nodes("object_name.obj_array[3] = 4;", "C++"),
	create_nodes("object_name.obj_array[3] = 4;", "C++"),

	create_nodes("enum Color { red, blue } c;", "C++"),
	create_nodes("enum Color { red, blue } c;", "C++"),
	create_nodes("enum Color { red, blue } c;", "C++"),
	create_nodes("enum Color { red, blue } c;", "C++"),

	create_nodes("Color my_colors = Color::red;", "C++"),
	create_nodes("Color my_colors = Color::red;", "C++"),
	create_nodes("Color my_colors = Color::red;", "C++"),
	create_nodes("Color my_colors = Color::red;", "C++"),
	create_nodes("Color my_colors = Color::red;", "C++"),

	create_nodes("struct name_of_struct { }", "C++"),
	
	create_nodes("struct name_of _struct { }", "C++"),
	create_nodes("struct name_of _struct { }", "C++"),

	create_nodes("object_name.param_name = 5;", "C++"),
	create_nodes("object_name.param_name = 5;", "C++"),
	create_nodes("object_name.param_name = 5;", "C++"),

	create_nodes("const int&& function();", "C++"),
	create_nodes("const int&& function();", "C++"),

	create_nodes("decltype(forwards<T1>(t1) + forward<T2>(t2))", "C++"),
	create_nodes("decltype(forwards<T1>(t1) + forward<T2>(t2))", "C++"),
	create_nodes("decltype(forwards<T1>(t1) + forward<T2>(t2))", "C++"),
	create_nodes("decltype(forwards<T1>(t1) + forward<T2>(t2))", "C++"),

        create_nodes("A<T> a;", "C++"),
	create_nodes("A<T> a;", "C++"),
	create_nodes("A<T> a;", "C++"),
	create_nodes("A<T> a;", "C++"),

	create_nodes("class class_name { public: bool isFalse; }", "C++"),

	create_nodes("class subclass_name : public base_class_name { };", "C++"),
	create_nodes("class subclass_name : public base_class_name { };", "C++"),

	create_nodes("inline double Class_name::Function() { return something; }", "C++"),
	create_nodes("inline double Class_name::Function() { return something; }", "C++"),
	create_nodes("inline double Class_name::Function() { return something; }", "C++"),

	create_nodes("union union_name { int var_name1; int var_name2; };", "C++"),
	create_nodes("union union_name { int var_name1; int var_name2; };", "C++"),
	create_nodes("union union_name { int var_name1; int var_name2; };", "C++"),
	create_nodes("union union_name { int var_name1; int var_name2; };", "C++"),
	create_nodes("union union_name { int var_name1; int var_name2; };", "C++"),

	create_nodes("union union_name obj;", "C++"),
	create_nodes("union union_name obj;", "C++"),
	create_nodes("union union_name obj;", "C++"),
	create_nodes("union union_name obj;", "C++"),

	create_nodes("using namespace std;", "C++"),

	create_nodes("template <class T> Stack { };", "C++"),
	create_nodes("template <class T> Stack { };", "C++"),
	create_nodes("template <class T> Stack { };", "C++"),
	
	create_nodes("*number1 = * number2;", "C++"),
	create_nodes("*number1 = * number2;", "C++"),
	
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),
	create_nodes("const auto & n = node->at(i);", "C++"),

	
};


const int names_start_pos[] = {
	
         3,  // int i;
	 8,

	 3,  // int first_val = 10, second_val = 20
	 8,
	 27,
	 
	 
	 3,  // public foo:

	 13, // foreach (int i = 0; i < count; ++i) {}(
	 40,
	 8,

	 2,  // a::b;
	 3,
	 7,
	 
	 1,  // a::b
	 2,
	 6,

	 2,  // array[size];
	 3,
	 9,

	 2,  // object_name.obj_array[3] = 4;
	 3,
	 9,

	 3,  // enum color { red, blue } c;
	 11,
	 18,
	 27,

	 3,  // Color my_colors = Color::red;
	 8,
	 16,
	 17,
	 23,
	 
	 3,  // struct name_of_struct { }

	 4,  // struct name_of struct { }
	 9,

	 2,  // object_name.param_name = 5;
	 3,
	 9,

	 6,  // const int&& function();
	 14,

	 8,  // decltype(forwards<T1>(t1) + forward<T2>(t2) 8,42 MAYBE DOESNT WORK RIGHT?
	 9,  
	 42,
	 43,

	 3,  // A<T> a; simpler version of problem with case above
	 4,
	 11,
	 21,

	 3,  // class class_name { public: bool isFalse; }

	 3,  // class subclass_name : public base_class_name { };
	 15,

	 11, // inline double Class_name::Function() { return something; 
	 12,
	 18,

	 3,  // union union_name { int var_name1; int var_name2; };
	 14,
	 19,
	 29,
	 34, 

	 3,  // union union_name obj; MAYBE DOESNT WORK
	 4,
	 8,
	 14,

	 6,  // using namespace std;

	 7,  // template <class T> Stack { };
	 12,
	 22,

	 5,  // *number1 = * number2;
	 17,

	 25, // const auto & n = node->at(i)
	 26,
	 32,

};


const std::string names[] = {

	"int",
	"i",

	"int",
	"first_val",
	"second_val",
	
        "foo",

	"i",
	"count",
	"int",

	"a::b",
	"a",
	"b",
	
	"a::b",
	"a",
	"b",

	"array[size]",
	"array",
	"size",

        "object_name.obj_array[3]",
	"object_name",
	"obj_array",

	"Color",
	"red",
	"blue",
	"c",

	"Color",
	"my_colors",
	"Color::red",
	"Color",
	"red",

	"name_of_struct",
	
	"name_of",
	"_struct",

	"object_name.param_name",
	"object_name",
	"param_name",

	"int",
	"function",

	"forwards", // MIGHT NOT BE RIGHT?
	"forwards",
	"forward", // MIGHT NOT BE RIGHT?
	"forward",

        "A", // Should be A<T> ?? simpler case of above
	"A",
	"T",
	"a",

	"class_name",

	"subclass_name",
	"base_class_name",

	"Class_name::Function",
	"Class_name",
	"Function",

	"union_name",
	"int",
	"var_name1",
	"int",
	"var_name2",

	"unionunion_name", // NOT RIGHT should return union union_name?
	"union",
	"union_name",
	"obj",
	
	"std",

	"class",
	"T",
	"Stack",

	"number1",
	"number2",

	"node->at",
	"node",
	"at",

};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(names_start_pos) ^ bu::make(names), node, names_start_pos, rhs) {

        BOOST_TEST(get_name(*node, names_start_pos) == rhs);

	std::cerr << names_start_pos << " : " << get_name(*node, names_start_pos) << "\n\n";
}
