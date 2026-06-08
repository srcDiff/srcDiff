## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

file(GLOB LIBRARY_HEADERS *.hpp)
file(GLOB LIBRARY_SOURCE  *.cpp)

find_package(LibXml2 REQUIRED)
find_package(srcML REQUIRED)

# srcdiff
add_library(library OBJECT ${LIBRARY_HEADERS} ${LIBRARY_SOURCE})
target_link_libraries(library PRIVATE LibXml2::LibXml2 srcML::LibsrcML)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/srcdiff.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT DEVLIBS)
