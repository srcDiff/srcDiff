## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2024  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#

file(GLOB NESTING_HEADERS *.hpp)
file(GLOB NESTING_SOURCE  *.cpp)

find_package(srcML REQUIRED)
find_package(LibXml2 REQUIRED)

add_library(nesting OBJECT ${NESTING_HEADERS} ${NESTING_SOURCE})
target_link_libraries(nesting PRIVATE srcML::LibsrcML)
