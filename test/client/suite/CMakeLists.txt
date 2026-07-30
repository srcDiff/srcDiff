# SPDX-License-Identifier: GPL-3.0-only
##
# @file CMakeLists.txt
# 
# @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
# 
# CMake file for client tests

# add client tests
file(GLOB CLIENT_TESTS *.sh)

foreach(CLIENT_TEST IN ITEMS ${CLIENT_TESTS})
    get_filename_component(TEST_NAME ${CLIENT_TEST} NAME)
    add_test(NAME srcdiff.${TEST_NAME} COMMAND ${CLIENT_TEST})
endforeach()
