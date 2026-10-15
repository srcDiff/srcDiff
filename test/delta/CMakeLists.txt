# SPDX-License-Identifier: GPL-3.0-only
##
# @file CMakeLists.txt
# 
# @copyright Copyright (C) 2024-2026 SDML (www.srcDiff.org)
# 
# CMake file for delta tests

add_subdirectory(src)
add_subdirectory(suite)

# one test per suite file, named after said  file (e.g. 'delta.if.cpp')
file(GLOB SUITE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/suite/*.xml)
set(BUILD_SUITE_FILES)
foreach(SUITE_FILE IN ITEMS ${SUITE_FILES})
    get_filename_component(SUITE_FILENAME ${SUITE_FILE} NAME)
    get_filename_component(SUITE_NAME ${SUITE_FILE} NAME_WLE)
    list(APPEND BUILD_SUITE_FILES ${CMAKE_CURRENT_BINARY_DIR}/suite/${SUITE_FILENAME})
    add_test(NAME delta.${SUITE_NAME}
        COMMAND $<TARGET_FILE:test_delta> ${CMAKE_CURRENT_BINARY_DIR}/suite/${SUITE_FILENAME})
endforeach()

add_custom_target(delta
    COMMAND $<TARGET_FILE:test_delta> ${BUILD_SUITE_FILES}
    COMMENT "Run all delta test cases"
    USES_TERMINAL
)
add_dependencies(delta test_delta)
