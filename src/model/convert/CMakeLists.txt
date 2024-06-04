## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2024  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#

file(GLOB CONVERT_HEADERS *.hpp)
file(GLOB CONVERT_SOURCE  *.cpp)

find_package(srcML REQUIRED)
find_package(LibXml2 REQUIRED)

add_library(convert OBJECT ${CONVERT_HEADERS} ${CONVERT_SOURCE})
target_link_libraries(convert PRIVATE srcML::LibsrcML LibXml2::LibXml2)
