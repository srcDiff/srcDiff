## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2024-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#
# Build for model unit tests 

find_package(Boost CONFIG REQUIRED)

# add tests
file(GLOB TEST_FILES *.cpp)
foreach(TEST_FILE ${TEST_FILES})
    add_unit_test(${TEST_FILE} Boost::headers)
endforeach()
