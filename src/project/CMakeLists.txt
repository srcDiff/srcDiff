## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

file(GLOB PROJECT_HEADERS *.hpp)
file(GLOB PROJECT_SOURCE  *.cpp)

add_library(project OBJECT ${PROJECT_HEADERS} ${PROJECT_SOURCE})
