## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2011-2024  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#


file(GLOB VIEW_HEADERS *.hpp)
file(GLOB VIEW_SOURCE *.cpp)

find_package(LibXml2 REQUIRED)
find_package(srcML REQUIRED)
find_package(CLI11 REQUIRED)

# srcdiff
add_library(view OBJECT ${VIEW_HEADERS} ${VIEW_SOURCE})
target_link_libraries(view PRIVATE LibXml2::LibXml2 srcML::LibsrcML CLI11::CLI11)
