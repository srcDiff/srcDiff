## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

file(GLOB SHORTEST_EDIT_SCRIPT_HEADERS *.h *.hpp)
file(GLOB SHORTEST_EDIT_SCRIPT_SOURCE *.c *.cpp)

add_library(shortest_edit_script OBJECT ${SHORTEST_EDIT_SCRIPT_HEADERS} ${SHORTEST_EDIT_SCRIPT_SOURCE})
target_link_libraries(shortest_edit_script PRIVATE)
