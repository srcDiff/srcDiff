## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

option(BUILD_CLIENT_TESTS "Build all cli tests"   ON)
option(BUILD_UNIT_TESTS   "Build all unit tests"  OFF)
option(BUILD_DELTA_TESTS  "Build all delta tests" ON)
option(BUILD_STRESS_TESTS "Build all stress tests" OFF)

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
