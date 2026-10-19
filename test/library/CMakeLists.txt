## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#
# Tests for libsrcdiff (src/library/srcdiff.h).

find_package(LibXml2 REQUIRED)
find_package(srcML REQUIRED)
find_package(Threads REQUIRED)

set(LIBRARY_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# delta suite files used to round trip the read functions
if(BUILD_DELTA_TESTS)
    set(DELTA_SUITE_DIR ${PROJECT_BINARY_DIR}/test/delta/suite)
else()
    set(DELTA_SUITE_DIR ${PROJECT_SOURCE_DIR}/test/delta/suite)
endif()

add_library(library_test STATIC ${LIBRARY_TEST_DIR}/library_test.cpp ${LIBRARY_TEST_DIR}/library_test.hpp
                                ${PROJECT_SOURCE_DIR}/test/delta/src/delta_utils.cpp)
target_include_directories(library_test PUBLIC ${LIBRARY_TEST_DIR} ${PROJECT_SOURCE_DIR}/test/delta/src)
target_link_libraries(library_test PUBLIC srcdiff_static srcML::LibsrcML LibXml2::LibXml2)

set(LIBRARY_TEST_TARGETS)

# one ctest per TEST_CASE named library.<file>.<case>
file(GLOB LIBRARY_TEST_SOURCES ${LIBRARY_TEST_DIR}/test_*.cpp)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${LIBRARY_TEST_SOURCES})
foreach(TEST_SOURCE IN ITEMS ${LIBRARY_TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    string(REGEX REPLACE "^test_" "" TEST_GROUP ${TEST_NAME})

    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} library_test)
    target_compile_definitions(${TEST_NAME} PRIVATE DELTA_SUITE_DIR="${DELTA_SUITE_DIR}")
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    list(APPEND LIBRARY_TEST_TARGETS ${TEST_NAME})

    file(STRINGS ${TEST_SOURCE} TEST_CASE_LINES REGEX "^TEST_CASE\\(")
    foreach(TEST_CASE_LINE IN ITEMS ${TEST_CASE_LINES})
        string(REGEX REPLACE "^TEST_CASE\\(([A-Za-z0-9_]+)\\).*" "\\1" TEST_CASE_NAME ${TEST_CASE_LINE})
        add_test(NAME library.${TEST_GROUP}.${TEST_CASE_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}> ${TEST_CASE_NAME})
    endforeach()
endforeach()

# the public header must be usable from C
foreach(LIBRARY_TYPE IN ITEMS static shared)
    set(TEST_NAME test_header_${LIBRARY_TYPE})
    add_executable(${TEST_NAME} ${LIBRARY_TEST_DIR}/test_header.c)
    # link the library file like an outside program would, so the library's own link dependencies are not added
    target_link_libraries(${TEST_NAME} $<TARGET_FILE:srcdiff_${LIBRARY_TYPE}> srcML::LibsrcML LibXml2::LibXml2 Threads::Threads)
    add_dependencies(${TEST_NAME} srcdiff_${LIBRARY_TYPE})
    set_target_properties(${TEST_NAME} PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF
                                                  LINKER_LANGUAGE CXX
                                                  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    if(NOT MSVC)
        target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Wstrict-prototypes -pedantic-errors)
    endif()
    list(APPEND LIBRARY_TEST_TARGETS ${TEST_NAME})
    add_test(NAME library.header.c_${LIBRARY_TYPE} COMMAND $<TARGET_FILE:${TEST_NAME}>)
endforeach()
