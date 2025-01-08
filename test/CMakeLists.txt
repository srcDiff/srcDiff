## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#


# include sub directories
if(BUILD_CLI_TESTS)
    add_subdirectory(cli)
endif()

if(BUILD_UNIT_TESTS)
    add_subdirectory(unit)
endif()

if(BUILD_TRANSLATOR_TESTS)
    add_subdirectory(src)
endif()


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
