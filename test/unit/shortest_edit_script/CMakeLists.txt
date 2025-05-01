## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2014-2025  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.

find_package(Boost CONFIG REQUIRED)

# add tests
add_unit_test(test_shortest_edit_script.cpp Boost::headers)
