## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

option(BUILD_ALL_TESTS    "Build all suite and unit tests" OFF)
option(BUILD_CLIENT_TESTS "Build all client tests"         ON)
option(BUILD_UNIT_TESTS   "Build all unit tests"           OFF)
option(BUILD_DELTA_TESTS  "Build all delta tests"          ON)
option(BUILD_STRESS_TESTS "Build all stress tests"         OFF)

if(BUILD_ALL OR BUILD_ALL_TESTS)
    set(BUILD_CLIENT_TESTS ON)
    set(BUILD_UNIT_TESTS   ON)
    set(BUILD_DELTA_TESTS  ON)
    set(BUILD_STRESS_TESTS ON)
endif()

# include sub directories
if(BUILD_CLIENT_TESTS)
    add_subdirectory(client)
endif()

if(BUILD_UNIT_TESTS)
    add_subdirectory(unit)
endif()

if(BUILD_DELTA_TESTS)
    add_subdirectory(delta)
endif()

if(BUILD_STRESS_TESTS)
    add_subdirectory(stress)
endif()
