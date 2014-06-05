##
#  CMakeLists.txt
#
#  Copyright (C) 2014  SDML (www.sdml.info)
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

add_subdirectory(oldclient)
add_subdirectory(shortest_edit_script)
add_subdirectory(srcml)
add_subdirectory(translator)
add_subdirectory(visualization)

if(WIN32)
add_subdirectory(windows)
endif()

#
# build_library
#
# Used to help with the creation of the srcML library.
# - LIBRARY_NAME is the name of the library and target
# - LIBRARY_TYPE is either STATIC or SHARED.
# 
macro(build_library LIBRARY_NAME LIBRARY_TYPE)

    add_library(${LIBRARY_NAME} ${LIBRARY_TYPE} $<TARGET_OBJECTS:shortest_edit_script> $<TARGET_OBJECTS:srcml> $<TARGET_OBJECTS:translator> $<TARGET_OBJECTS:visualization>)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_target_properties(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME libsrcml LINK_FLAGS_DEBUG "/SAFESEH:NO")
elseif(APPLE)
    set_target_properties(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME srcml)
else()
    set_target_properties(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME srcml)
endif()

    target_link_libraries(${LIBRARY_NAME} PRIVATE ${LIBSRCML_LIBRARIES})

endmacro()

# Building static and dynamic libraries for srcML.
build_library(srcdiff_static STATIC)
set_target_properties(srcdiff_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

build_library(srcdiff_shared SHARED)
set_target_properties(srcdiff_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(APPLE)
    install(CODE "execute_process(COMMAND \"/usr/bin/strip\"  \"-x\" \"${CMAKE_BINARY_DIR}/bin/libsrcml.dylib\")")
elseif(NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") AND NOT WIN32)
    install(CODE "execute_process(COMMAND \"/usr/bin/strip\"  \"-x\" \"${CMAKE_BINARY_DIR}/bin/libsrcml.so\")")
endif()

install(TARGETS srcml_shared srcml_static RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
