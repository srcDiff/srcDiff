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


<<<<<<< HEAD
const std::shared_ptr<srcml_nodes> nodes[] = {
					      
        create_nodes("int i;", "C++"),
	create_nodes("int i;", "C++"),

	create_nodes("public foo:", "C++"),

	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
};

const int names_start_pos[] = {
	
         3,  // int i;
	 8,
	 
	 3,  // public foo:
	 
	 13, // foreach (int i = 0; i < count; ++i) {}(
	 40,

	 
};

const std::string names[] = {

	"int",
	"i",
	
        "foo",
	
	"i",
	"count",
};



BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(names_start_pos) ^ bu::make(names), node, names_start_pos, rhs) {
 
        //std::cerr << node << ":" << names_start_pos << ":" << rhs << '\n';

        BOOST_TEST(get_name(*node, names_start_pos) == rhs);

	std::cerr << "\n" << get_name(*node, names_start_pos) << "\n\n";

=======

//=========================================================================
//===================== passing test set Start here =======================
//=========================================================================


const std::shared_ptr<srcml_nodes> nodes[] = {
	
	create_nodes("int i", "C++"),
	create_nodes("int decl_stmt;", "C++"),
	create_nodes("int parameter;", "C++"),
	create_nodes("int param;", "C++"),
	create_nodes("int decl;", "C++"),

	create_nodes("public foo:", "C++"),
	create_nodes("public foo:", "C++"),
	create_nodes("public foo:", "C++"),
	create_nodes("protected foo:", "C++"),
	create_nodes("protected foo:", "C++"),
	create_nodes("protected foo:", "C++"),
	create_nodes("private foo:", "C++"),
	create_nodes("private foo:", "C++"),
	create_nodes("private foo:", "C++"),

	create_nodes("for (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("for (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("for (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("for (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("foreach (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("while (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("while (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("while (int i = 0; i < count; ++i) {}(", "C++"),
	create_nodes("while (int i = 0; i < count; ++i) {}(", "C++"),

	create_nodes("if(i < 10) {} else {}(", "C++"),
	create_nodes("if(i < 10) {} else {}(", "C++"),
	create_nodes("if(i < 10) {} else {}(", "C++"),
	create_nodes("if(i < 10) {} else {}(", "C++"),

	create_nodes("expr_stmt("");(", "C++"),
	create_nodes("expr_stmt("");(", "C++"),
	create_nodes("expr_stmt("");(", "C++"),
	create_nodes("decl_stmt foo;(", "C++"),
	create_nodes("decl_stmt foo;(", "C++"),
	create_nodes("decl_stmt foo;(", "C++"),
	create_nodes("return 0;(", "C++"),
	create_nodes("return 0;(", "C++"),
	create_nodes("return 0;(", "C++")
};



const int name_start_pos[] = {
	
	0,
	1,
	1,
	4,
	4,

	0,
	1,
	1,
	4,
	4,

	0,
	1,
	1,
	4,
	4,

	0,
	1,
	1,
	4,
	4,

	0,
	1,
	1,
	4,
	4,

	0,
	1,
	1,
	4,
	4,

	0,
	1,
	1,
	4,
	4,

	0,
	1,
	1
};


const std::string names[] = {
  "i",
};


BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(name_start_pos) ^ bu::make(names), node, name_start_pos, rhs) {

	std::cerr << node << ":" << name_start_pos << ":" << rhs << '\n';

	  BOOST_TEST(get_name(*node, name_start_pos) == rhs);
>>>>>>> 0c50e9a6a72c35b3872e68ce4eb340f6511b0a10
}



<<<<<<< HEAD
=======
//=========================================================================
// ==================== passing test set End here =========================
//=========================================================================






//==========================================================================
// ====================== failed test set Start here =======================
//==========================================================================



//==========================================================================
// ====================== failed test set End here =========================
//==========================================================================


>>>>>>> 0c50e9a6a72c35b3872e68ce4eb340f6511b0a10
