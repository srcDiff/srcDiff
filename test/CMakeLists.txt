## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2014-2024  SDML www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#
macro(add_unit_test TEST_FILE)

    get_filename_component(TEST_NAME_WITH_EXTENSION ${TEST_FILE} NAME)
    string(FIND ${TEST_NAME_WITH_EXTENSION} "." EXTENSION_BEGIN)
    string(SUBSTRING ${TEST_NAME_WITH_EXTENSION} 0 ${EXTENSION_BEGIN} TEST_NAME)

    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} ${ARGN} srcdiff_static Boost::headers)
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

endmacro()
# include sub directories
if(BUILD_UNIT_TESTS)
    find_package(Boost REQUIRED)
    add_subdirectory(unit)
endif()

if(BUILD_TRANSLATOR_TESTS)
    add_subdirectory(src)
endif()
