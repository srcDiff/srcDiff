##
#  CMakeLists.txt
#
#  Copyright (C) 2011-2021  SDML (www.srcML.org)
#
#  This file is part of the srcDiff translator.
#
#  The srcDiff translator is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  The srcDiff translator is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the srcDiff translator; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

file(GLOB TRANSLATOR_HEADERS *.hpp)
file(GLOB TRANSLATOR_SOURCE *.cpp)

if(NOT SVN_ENABLED)

list(REMOVE_ITEM TRANSLATOR_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/srcdiff_input_svn.cpp)

endif()

if(NOT GIT_ENABLED)

list(REMOVE_ITEM TRANSLATOR_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/srcdiff_input_git.cpp)

endif()

find_package(LibXml2 REQUIRED)
# find_package(srcML REQUIRED)

# srcdiff
add_library(translator OBJECT ${TRANSLATOR_HEADERS} ${TRANSLATOR_SOURCE})
target_link_libraries(translator PRIVATE LibXml2::LibXml2 srcML::LibsrcML)
