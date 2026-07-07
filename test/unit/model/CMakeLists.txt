## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2024-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#
# Build for model unit tests 

add_unit_test(test_if_t.cpp Boost::headers)
set_tests_properties(test_if_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_for_t.cpp Boost::headers)
set_tests_properties(test_for_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_else_t.cpp Boost::headers)
set_tests_properties(test_else_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_name_t.cpp Boost::headers)
set_tests_properties(test_name_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_elseif.cpp Boost::headers)
set_tests_properties(test_elseif PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_named_construct.cpp Boost::headers)
set_tests_properties(test_named_construct PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

# add_unit_test(test_always_match.cpp Boost::headers)
# set_tests_properties(test_always_match PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_if_stmt.cpp Boost::headers)
set_tests_properties(test_if_stmt PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_case_t.cpp Boost::headers)
set_tests_properties(test_case_t PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_call.cpp Boost::headers)
set_tests_properties(test_call PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_decl_stmt.cpp Boost::headers)
set_tests_properties(test_decl_stmt PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_expr_construct.cpp Boost::headers)
set_tests_properties(test_expr_construct PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_identifier_decl.cpp Boost::headers)
set_tests_properties(test_identifier_decl PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_expr_stmt.cpp Boost::headers)
set_tests_properties(test_expr_stmt PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_expr.cpp Boost::headers)
set_tests_properties(test_expr PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")

add_unit_test(test_condition.cpp Boost::headers)
set_tests_properties(test_condition PROPERTIES ENVIRONMENT "BOOST_TEST_LOG_LEVEL=all")
