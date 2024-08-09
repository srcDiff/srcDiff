## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

file(GLOB MODEL_HEADERS *.hpp)
file(GLOB MODEL_SOURCE  *.cpp)

find_package(srcML REQUIRED)
find_package(LibXml2 REQUIRED)

add_subdirectory(nest)
add_subdirectory(convert)

add_library(model OBJECT ${MODEL_HEADERS} ${MODEL_SOURCE})
target_link_libraries(model PRIVATE srcML::LibsrcML LibXml2::LibXml2)
