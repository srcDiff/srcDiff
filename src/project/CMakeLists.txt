## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2015-2021  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#


file(GLOB PROJECT_HEADERS *.hpp)
file(GLOB PROJECT_SOURCE  *.cpp)

add_library(project OBJECT ${PROJECT_HEADERS} ${PROJECT_SOURCE})
