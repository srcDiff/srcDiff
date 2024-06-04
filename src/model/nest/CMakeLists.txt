## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2024  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#

file(GLOB NEST_HEADERS *.hpp)
file(GLOB NEST_SOURCE  *.cpp)

# find_package(srcML REQUIRED)
find_package(LibXml2 REQUIRED)

add_library(nest OBJECT ${NEST_HEADERS} ${NEST_SOURCE})
target_link_libraries(nest PRIVATE srcML::LibsrcML LibXml2::LibXml2)
