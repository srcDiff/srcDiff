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

if(NOT SVN_ENABLED)

    list(REMOVE_ITEM OLDCLIENT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/svn_io.hpp)
    list(REMOVE_ITEM OLDCLIENT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/svn_io.cpp)

endif()

# srcdiff
if(WIN32)
add_executable(srcdiff ${OLDCLIENT_HEADERS} ${OLDCLIENT_SOURCE} $<TARGET_OBJECTS:shortest_edit_script>
		       $<TARGET_OBJECTS:srcml> $<TARGET_OBJECTS:translator> $<TARGET_OBJECTS:visualization>
		       $<TARGET_OBJECTS:windows>)
else()
add_executable(srcdiff ${OLDCLIENT_HEADERS} ${OLDCLIENT_SOURCE} $<TARGET_OBJECTS:shortest_edit_script>
		       $<TARGET_OBJECTS:srcml> $<TARGET_OBJECTS:translator> $<TARGET_OBJECTS:visualization>)
endif()

target_link_libraries(srcdiff ${LIBXML2_LIBRARIES} ${LIBSRCML_LIBRARY} pthread ${LIBSVN_LIBRARIES})

set_target_properties(srcdiff PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

install(TARGETS srcdiff RUNTIME DESTINATION bin)
