## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#


file(GLOB VIEW_HEADERS *.hpp)
file(GLOB VIEW_SOURCE *.cpp)

find_package(srcML REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(CLI11 REQUIRED)

# srcdiff
add_library(view OBJECT ${VIEW_HEADERS} ${VIEW_SOURCE})
target_link_libraries(view PRIVATE srcML::LibsrcML LibXml2::LibXml2 CLI11::CLI11)
