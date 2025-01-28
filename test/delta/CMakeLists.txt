# SPDX-License-Identifier: GPL-3.0-only
##
# @file CMakeLists.txt
# 
# @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
# 
# CMake file for delta tests

add_subdirectory(src)
add_subdirectory(suite)

set(TEST_SUITE "test_suite.py")
configure_file(${TEST_SUITE} ${CMAKE_CURRENT_BINARY_DIR}/${TEST_SUITE} COPYONLY)

# copy test suite over to build directory, so tests run from that directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/reports)

add_custom_target(delta
    COMMAND python3 ${TEST_SUITE} || true
    COMMENT "Run all test delta cases"
    USES_TERMINAL
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(delta srcdiff archive_reader switch_differences)
