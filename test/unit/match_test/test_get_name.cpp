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






//=========================================================================
//===================== passing test set Start here =======================
//=========================================================================


const std::shared_ptr<srcml_nodes> nodes[] = {
	
	create_nodes("int i", "C++"),
	create_nodes("int decl_stmt;", "C++"),
	create_nodes("int parameter;", "C++"),
	create_nodes("int param;", "C++"),
	create_nodes("int decl;", "C++"),
};



const int name_start_pos[] = {
	
	0,
	1,
	1,
	4,
	4,
};



BOOST_DATA_TEST_CASE(passes, bu::make(nodes) ^ bu::make(name_start_pos), node, name_start_pos, rhs) {

	std::cerr << node << ":" << name_start_pos << ":" << rhs << '\n';

    BOOST_TEST(get_name(*node, name_start_pos) == rhs);
}


//=========================================================================
// ==================== passing test set End here =========================
//=========================================================================






//==========================================================================
// ====================== failed test set Start here =======================
//==========================================================================



//==========================================================================
// ====================== failed test set End here =========================
//==========================================================================


