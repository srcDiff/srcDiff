## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2008-2019 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# include sub directories
include_directories(utils)

find_package(srcML REQUIRED)
find_package(LibXml2 REQUIRED)


macro(add_unit_test TEST_FILE)

    get_filename_component(TEST_NAME_WITH_EXTENSION ${TEST_FILE} NAME)
    string(FIND ${TEST_NAME_WITH_EXTENSION} "." EXTENSION_BEGIN)
    string(SUBSTRING ${TEST_NAME_WITH_EXTENSION} 0 ${EXTENSION_BEGIN} TEST_NAME)

    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} ${ARGN} srcdiff_static srcML::LibsrcML LibXml2::LibXml2 $<TARGET_OBJECTS:common> ${ARGN})
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

endmacro()

add_subdirectory(utils)
add_subdirectory(model)
add_subdirectory(shortest_edit_script)
