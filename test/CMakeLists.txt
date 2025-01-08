## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

enable_testing()
include(CTest)

option(BUILD_CLI_TESTS  "Build all cli tests" ON)
option(BUILD_UNIT_TESTS "Build all unit tests" OFF)
option(BUILD_TRANSLATOR_TESTS "Build all transltor tests" ON)

# include sub directories
if(BUILD_CLI_TESTS)
    add_subdirectory(cli)
endif()

if(BUILD_UNIT_TESTS)
    add_subdirectory(unit)
endif()

if(BUILD_TRANSLATOR_TESTS)
    add_subdirectory(suite)
endif()
