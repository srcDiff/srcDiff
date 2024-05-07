## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2014-2021  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#
# add tests
add_unit_test(test_is_interchangeable_match.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_extract_name.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_name.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_skip_tag.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_call_name.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_top_level_name_seek.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_decl_name.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_name_list_similarity.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_first_child.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_is_single_call_expr.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_function_type_name.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_for_condition.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_class_type_name.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_conditional_has_block.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_condition.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_for_control_matches.cpp $<TARGET_OBJECTS:common>)
