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

if(BUILD_SUITE_TESTS)
    add_subdirectory(suite)
endif()