## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

file(GLOB CLIENT_HEADERS *.hpp)
file(GLOB CLIENT_SOURCE *.cpp)

if(APPLE)
    # Use rpath for the install directory
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # Specify the rpath directory (in this case, /usr/local/lib)
    list(APPEND CMAKE_INSTALL_RPATH "/usr/local/lib")
endif()

if(NOT SVN_ENABLED)

    list(REMOVE_ITEM CLIENT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/input_source_svn.cpp)

endif()

if(NOT GIT_ENABLED)

    list(REMOVE_ITEM CLIENT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/input_source_git.cpp)

endif()

find_package(srcML REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Threads REQUIRED)
find_package(CLI11 REQUIRED)

# Add rpath for macos
if(APPLE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    list(APPEND CMAKE_INSTALL_RPATH "/usr/local/lib")
endif()

# srcdiff
if(WIN32)

    add_executable(srcdiff ${CLIENT_HEADERS} ${CLIENT_SOURCE} $<TARGET_OBJECTS:shortest_edit_script>
                $<TARGET_OBJECTS:srcml> $<TARGET_OBJECTS:translator> $<TARGET_OBJECTS:view>
                $<TARGET_OBJECTS:model> $<TARGET_OBJECTS:nest> $<TARGET_OBJECTS:convert> $<TARGET_OBJECTS:project>
                # extra for windows:
                $<TARGET_OBJECTS:windows>)

else()
    add_executable(srcdiff ${CLIENT_HEADERS} ${CLIENT_SOURCE} $<TARGET_OBJECTS:shortest_edit_script>
              $<TARGET_OBJECTS:srcml> $<TARGET_OBJECTS:translator> $<TARGET_OBJECTS:view>
              $<TARGET_OBJECTS:model> $<TARGET_OBJECTS:nest> $<TARGET_OBJECTS:convert> $<TARGET_OBJECTS:project>)
endif()

target_link_libraries(srcdiff PRIVATE srcML::LibsrcML LibXml2::LibXml2 Threads::Threads srcsax_static CLI11::CLI11)

set_target_properties(srcdiff PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_custom_command(TARGET srcdiff POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:srcdiff> $<TARGET_RUNTIME_DLLS:srcdiff>
  COMMAND_EXPAND_LISTS
)

install(TARGETS srcdiff COMPONENT SRCDIFF)
