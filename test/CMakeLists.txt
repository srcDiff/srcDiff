## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

option(BUILD_ALL_TESTS    "Build all suite and unit tests" OFF)
option(BUILD_SUITE_TESTS  "Build all suite tests" ON)
option(BUILD_CLIENT_TESTS "Build all client tests"  ON)
option(BUILD_UNIT_TESTS   "Build all unit tests"  OFF)

if(BUILD_ALL OR BUILD_ALL_TESTS)
    set(BUILD_SUITE_TESTS ON)
    set(BUILD_CLIENT_TESTS ON)
    set(BUILD_UNIT_TESTS ON)
endif()

# include sub directories
if(BUILD_SUITE_TESTS)
    add_subdirectory(src)
endif()

if(BUILD_UNIT_TESTS)
    add_subdirectory(unit)
endif()

## @todo Push down to appropriate directory
if(BUILD_SUITE_TESTS)

# copy test suite over to build directory, so tests run from that directory
set(TEST_SUITE "test_suite.py")
configure_file(${TEST_SUITE} ${CMAKE_CURRENT_BINARY_DIR}/${TEST_SUITE} COPYONLY)

file(GLOB TEST_FILES ${CMAKE_SOURCE_DIR}/test/suite/*.xml)
file(MAKE_DIRECTORY suite/)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/reports/)
foreach(PATH IN ITEMS ${TEST_FILES})
    get_filename_component(FILENAME ${PATH} NAME)
    configure_file(suite/${FILENAME} ${CMAKE_CURRENT_BINARY_DIR}/suite/${FILENAME} NEWLINE_STYLE LF)
endforeach()

add_custom_target(suite
    COMMAND python3 ${TEST_SUITE}
    COMMENT "Run all test suite cases"
    USES_TERMINAL
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(suite srcdiff archive_reader switch_differences)

endif()