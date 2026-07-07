## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

file(GLOB DELTA_HEADERS *.hpp)
file(GLOB DELTA_SOURCE *.cpp)

if(NOT SVN_ENABLED)

list(REMOVE_ITEM DELTA_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/srcdiff_input_svn.cpp)

endif()

if(NOT GIT_ENABLED)

list(REMOVE_ITEM DELTA_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/srcdiff_input_git.cpp)

endif()

find_package(LibXml2 REQUIRED)
find_package(srcML REQUIRED)

# srcdiff
add_library(delta OBJECT ${DELTA_HEADERS} ${DELTA_SOURCE})
target_link_libraries(delta PRIVATE LibXml2::LibXml2 srcML::LibsrcML)
