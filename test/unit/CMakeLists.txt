## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2014-2021  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.

# include sub directories
include_directories(utils)

add_subdirectory(utils)
add_subdirectory(match_test)
add_subdirectory(shortest_edit_script)
