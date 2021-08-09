#define BOOST_TEST_MODULE test get_class_type_name

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

       // class test cases
					     
       create_nodes("class Animal;", "C++"),
					      
       create_nodes("class Animal { string species; };", "C++"),
       create_nodes("class Animal { string species; };", "C++"),
       create_nodes("class Animal { string species; };", "C++"),
       create_nodes("class Animal { string species; };", "C++"),

       create_nodes("class A { class B { }; };", "C++"),
       create_nodes("class A { class B { }; };", "C++"),

       create_nodes("template<class T> class Z { };", "C++"),

       create_nodes("class B final : A { };", "C++"),


       // enum test cases

       create_nodes("enum Color;", "C++"),
       
       create_nodes("enum Color { red, blue };", "C++"), 

       create_nodes("enum class Kind { None, A, B, Integer };", "C++"), 

       create_nodes("enum class Shape : uint8_t { circle = 0, };", "C++"),

       create_nodes("enum Type { new, old } c ;", "C++"),

  
       // struct test cases

       create_nodes("struct Employee;", "C++"),
       
       create_nodes("struct Employee { int age; };", "C++"),

       create_nodes("struct X { enum direction { left = 'l', right = 'r' }; };", "C++"),

       create_nodes("struct A { struct B { }; };", "C++"),
       create_nodes("struct A { struct B { }; };", "C++"),

  
       // union test cases
       
       create_nodes("union RecordType { };", "C++"),

       create_nodes("union RecordType;", "C++"),

       create_nodes("template <typename T> struct struct_name { };", "C++"),

       create_nodes("struct B final : A { };", "C++"),

       create_nodes("struct derived : base { };", "C++"),

       
       // union test cases
       
       create_nodes("union RecordType;", "C++"),

       create_nodes("union RecordType { };", "C++"),
  
       create_nodes("struct A { union B { }; };", "C++"),
       create_nodes("struct A { union B { }; };", "C++"),
};


const int start_pos[] = {

       // Class test cases

       0,

       0,
       1,
       2,
       3,

       0,
       11,
       

       // enum test cases
       
       0,
       
       0,

       0,

       0,

       0,

       0,   // <class_decl>:0   class Animal;

       0,   // <class>:0        class Animal { string species; }
       1,   // 'class':1
       2,   // ' ':2 
       3,   // <name>:3 

       0,   // <class>:0        class A { class B { }; };
       11,  // <class>:11

       0,   // <class>:0        template<class T> class Z { };

       0,   // <class>:0        class B final : A { };

       
       // enum test cases
       
       0,   // <enum_decl>:0            enum Color;
       
       0,   // <enum>:0                 enum Color { red, blue };

       0,   // <enum type="class">:0    enum class Kind { None, A, B, Integer };

       0,   // <enum type="class">:0    enum class Shape : uint8_t { circle = 0, };
 
       0,   // <enum>:0                 enum Type { new, old } c ; 

       
       // struct test cases
       
       0,
       
       0,

       0,

       0,
       11,

       0,   // <struct_decl>:0   struct Employee;
       
       0,   // <struct>:0        struct Employee { int age; }

       0,   // <struct>:0        struct X { enum direction { left = 'l', right = 'r' }; }

       0,   // <struct>:0        struct A { struct B { }; };
       11,  // <struct>:11

       0,   // <struct>:0        template <typename T> struct struct_name { };

       0,   // <struct>:0        struct B final : A { }

       0,   // <struct>:0        struct derived : base { };

  
       // union test cases
       
       0,

       0,

       0,
       11,

       0,   // <union_decl>:0    union RecordType;

       0,   // <union>:0         union RecordType { };

       0,   // <union>:0         struct A { union B { }; };
       11,  // <union>:11
};


const std::string names[] = {

       // Class test cases

       "Animal",
       
       "Animal",
       "",
       "",
       "",

       "A",
       "B",
       

       // enum test cases

       "Color",
       
       "", 

       "",

       "",

       "",
       

       // struct test cases

       "Employee",
       
       "Employee",

       "X",

       "A",
       "B",
       

       // union test cases
       
       "RecordType",

       "RecordType",

       "A",
       "B",

       "Animal",   // <class_decl>:0   class Animal;
       
       "Animal",   // <class>:0        class Animal { string species; }
       "",         // 'class':1
       "",         // ' ':2 
       "",         // <name>:3 

       "A",        // <class>:0        class A { class B { }; };
       "B",        // <class>:11

       "Z",        // <class>:0        template<class T> class Z { };

       "B",        // <class>:0        class B final : A { };
       

       
       // enum test cases

       "Color",    // <enum_decl>:0            enum Color;
       
       "Color",    // <enum>:0                 enum Color { red, blue };

       "Kind",     // <enum type="class">:0    enum class Kind { None, A, B, Integer };

       "Shape",    // <enum type="class">:0    enum class Shape : uint8_t { circle = 0, };

       "Type",     // <enum>:0                 enum Type { new, old } c ; 
       

       
       // struct test cases

       "Employee",     // <struct_decl>:0   struct Employee;
       
       "Employee",     // <struct>:0        struct Employee { int age; }

       "X",            // <struct>:0        struct X { enum direction { left = 'l', right = 'r' }; }

       "A",            // <struct>:0        struct A { struct B { }; };
       "B",            // <struct>:11

       "struct_name",  // <struct>:0        template <typename T> struct struct_name { };

       "B",            // <struct>:0        struct B final : A { }

       "derived",      // <struct>:0        struct derived : base { };
       

       
       // union test cases
       
       "RecordType",   // <union_decl>:0    union RecordType;

       "RecordType",   // <union>:0         union RecordType { };
 
       "A",            // <union>:0         struct A { union B { }; };
       "B",            // <union>:11
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(start_pos) ^ bu::make(names), node, start_pos, rhs) {
  
       BOOST_TEST(get_class_type_name(*node, start_pos) == rhs);
}
