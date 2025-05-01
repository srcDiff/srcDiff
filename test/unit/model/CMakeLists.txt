## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2014-2025  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.

find_package(Boost CONFIG REQUIRED)

# add tests
file(GLOB TEST_FILES *.cpp)
foreach(TEST_FILE ${TEST_FILES})

    add_unit_test(${TEST_FILE} Boost::headers)

endforeach()
