##
#  CMakeLists.txt
#
#  Copyright (C) 2011-2011  SDML (www.sdml.info)
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

file(GLOB OLDCLIENT_HEADERS *.hpp)
file(GLOB OLDCLIENT_SOURCE *.cpp)

list(REMOVE_ITEM OLDCLIENT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/svn_io.hpp)
list(REMOVE_ITEM OLDCLIENT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/svn_io.cpp)

# srcdiff
add_executable(srcdiff ${OLDCLIENT_HEADERS} ${OLDCLIENT_SOURCE} ${TRANSLATOR_HEADERS} ${TRANSLATOR_SOURCE} 
		       ${SRCML_HEADERS} ${SRCML_SOURCE} ${SHORTEST_EDIT_SCRIPT_HEADERS} ${SHORTEST_EDIT_SCRIPT_SOURCE}
		       ${VISUALIZATION_HEADERS} ${VISUALIZATION_SOURCE} ${WINDOWS_HEADERS} ${WINDOWS_SOURCE})

target_link_libraries(srcdiff shortest_edit_script srcml translator visualization windows {LIBXML2_LIB} ${LIBSRCML_LIB} pthread)
