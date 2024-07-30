##
#  CMakeLists.txt
#
#  Copyright (C) 2024  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#
#  The srcDiff translator is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  The srcDiff translator is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the srcDiff translator; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

find_package(Boost REQUIRED)

# add tests

set(TEST_FILES
    test_if_t.cpp
    test_for_t.cpp
    test_else_t.cpp
    test_name_t.cpp
    test_elseif.cpp
    test_named_construct.cpp
    test_if_stmt.cpp
    test_case_t.cpp
    test_call.cpp
    test_decl_stmt.cpp
    test_identifier_decl.cpp
    test_expr_construct.cpp
    test_expr_stmt.cpp
    test_expr.cpp
    test_condition.cpp
    test_conditional.cpp
    test_clause.cpp
    test_block.cpp
    test_string_utils.cpp
)

foreach(TEST_FILE ${TEST_FILES})

    add_unit_test(${TEST_FILE} Boost::headers)

endforeach()
#add_unit_test(test_if_t.cpp Boost::headers)
#set_tests_properties(test_if_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_for_t.cpp Boost::headers)
#set_tests_properties(test_for_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_else_t.cpp Boost::headers)
#set_tests_properties(test_else_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_name_t.cpp Boost::headers)
#set_tests_properties(test_name_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_elseif.cpp Boost::headers)
#set_tests_properties(test_elseif PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_named_construct.cpp Boost::headers)
#set_tests_properties(test_named_construct PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

# add_unit_test(test_always_match.cpp Boost::headers)
# set_tests_properties(test_always_match PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_if_stmt.cpp Boost::headers)
#set_tests_properties(test_if_stmt PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_case_t.cpp Boost::headers)
#set_tests_properties(test_case_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_call.cpp Boost::headers)
#set_tests_properties(test_call PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_decl_stmt.cpp Boost::headers)
#set_tests_properties(test_decl_stmt PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_expr_construct.cpp Boost::headers)
#set_tests_properties(test_expr_construct PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_identifier_decl.cpp Boost::headers)
#set_tests_properties(test_identifier_decl PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_expr_stmt.cpp Boost::headers)
#set_tests_properties(test_expr_stmt PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_expr.cpp Boost::headers)
#set_tests_properties(test_expr PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_condition.cpp Boost::headers)
#set_tests_properties(test_condition PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_conditional.cpp Boost::headers)
#set_tests_properties(test_conditional PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_clause.cpp Boost::headers)
#set_tests_properties(test_clause PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_block.cpp Boost::headers)
#set_tests_properties(test_block PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

#add_unit_test(test_string_utils.cpp Boost::headers)
#set_tests_properties(test_string_utils PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")
