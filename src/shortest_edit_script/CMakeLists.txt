## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2011-2024  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#


file(GLOB SHORTEST_EDIT_SCRIPT_HEADERS *.h *.hpp)
file(GLOB SHORTEST_EDIT_SCRIPT_SOURCE *.c *.cpp)

add_library(shortest_edit_script OBJECT ${SHORTEST_EDIT_SCRIPT_HEADERS} ${SHORTEST_EDIT_SCRIPT_SOURCE})
target_link_libraries(shortest_edit_script PRIVATE)
