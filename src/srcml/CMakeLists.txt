## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

file(GLOB SRCML_HEADERS *.hpp)
file(GLOB SRCML_SOURCE *.cpp)

find_package(LibXml2 REQUIRED)
find_package(srcML REQUIRED)

if(NOT SVN_ENABLED)

    list(REMOVE_ITEM SRCML_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/svn_io.hpp)
    list(REMOVE_ITEM SRCML_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/svn_io.cpp)

endif()

# srcdiff
add_library(srcml OBJECT ${SRCML_HEADERS} ${SRCML_SOURCE})
target_link_libraries(srcml PRIVATE LibXml2::LibXml2 srcML::LibsrcML)
