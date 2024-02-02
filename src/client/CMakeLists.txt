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

file(GLOB CLIENT_HEADERS *.hpp)
file(GLOB CLIENT_SOURCE *.cpp)

if(NOT SVN_ENABLED)

    list(REMOVE_ITEM CLIENT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/srcdiff_input_source_svn.cpp)

endif()

if(NOT GIT_ENABLED)

    list(REMOVE_ITEM CLIENT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/srcdiff_input_source_git.cpp)

endif()

# Fetch CLI11 from Github
# See: https://cliutils.github.io/CLI11/book/chapters/installation.html#using-fetchcontent

include(FetchContent)
FetchContent_Declare(
    cli11_proj
    QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)

FetchContent_MakeAvailable(cli11_proj)

# srcdiff
if(WIN32)

    add_executable(srcdiff ${CLIENT_HEADERS} ${CLIENT_SOURCE} $<TARGET_OBJECTS:shortest_edit_script>
                $<TARGET_OBJECTS:srcml> $<TARGET_OBJECTS:translator> $<TARGET_OBJECTS:view>
                $<TARGET_OBJECTS:model> $<TARGET_OBJECTS:project>
                # extra for windows:
                $<TARGET_OBJECTS:windows>)

else()
    add_executable(srcdiff ${CLIENT_HEADERS} ${CLIENT_SOURCE} $<TARGET_OBJECTS:shortest_edit_script>
              $<TARGET_OBJECTS:srcml> $<TARGET_OBJECTS:translator> $<TARGET_OBJECTS:view>
              $<TARGET_OBJECTS:model> $<TARGET_OBJECTS:project>)
endif()

target_link_libraries(srcdiff ${LIBSRCML_LIBRARY} ${LIBXML2_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads srcsax_shared CLI11::CLI11)

set_target_properties(srcdiff PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

install(TARGETS srcdiff RUNTIME DESTINATION bin)
