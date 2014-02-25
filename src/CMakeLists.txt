###
#    CMakeLists.txt
#
#
#    Optional cmake file for srcML.
#    The main way to build srcML is with normal Makefile

cmake_minimum_required(VERSION 2.8)
project(srcDiff)

#set_source_files_properties(shortest_edit_script.c PROPERTIES LANGUAGE CXX)

file(GLOB OLDCLIENT_HEADERS oldclient/*.hpp)
file(GLOB OLDCLIENT_SOURCE oldclient/*.cpp)

list(REMOVE_ITEM OLDCLIENT_HEADERS ${CMAKE_SOURCE_DIR}/src/oldclient/svn_io.hpp)
list(REMOVE_ITEM OLDCLIENT_SOURCE ${CMAKE_SOURCE_DIR}/src/oldclient/svn_io.cpp)
message(${OLDCLIENT_HEADERS})

file(GLOB TRANSLATOR_HEADERS translator/*.hpp)
file(GLOB TRANSLTOR_SOURCE translator/*.cpp)

file(GLOB SRCML_HEADERS srcml/*.hpp)
file(GLOB SRCML_SOURCE srcml/*.cpp)

file(GLOB SHORTEST_EDIT_SCRIPT_HEADERS shortest_edit_script/*.h shortest_edit_script/*.hpp)
file(GLOB SHORTEST_EDIT_SCRIPT_SOURCE shortest_edit_script/*.c shortest_edit_script/*.cpp)

file(GLOB VISUALIZATION_HEADERS visualization/*.hpp)
file(GLOB VISUALIZATION_SOURCE visualization/*.cpp)

if(WIN32)
file(GLOB WINDOWS_HEADERS windows/*.hpp)
file(GLOB WINDOWS_SOURCE windows/*.cpp)
endif()

# srcdiff
add_executable(srcdiff ${OLDCLIENT_HEADERS} ${OLDCLIENT_SOURCE} ${TRANSLATOR_HEADERS} ${TRANSLATOR_SOURCE} 
		       ${SRCML_HEADERS} ${SRCML_SOURCE} ${SHORTEST_EDIT_SCRIPT_HEADERS} ${SHORTEST_EDIT_SCRIPT_SOURCE}
		       ${VISUALIZATION_HEADERS} ${VISUALIZATION_SOURCE} ${WINDOWS_HEADERS} ${WINDOWS_SOURCE})

target_link_libraries(srcdiff archive xml2 ${LIBANTLR_LIB} ${LIBSRCML_LIB} pthread)

