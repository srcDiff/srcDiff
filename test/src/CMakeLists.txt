##
#  CMakeLists.txt
#
#  Copyright (C) 2014-2021  SDML (www.srcML.org)
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
find_package(LibXml2 REQUIRED)
find_package(Iconv REQUIRED)
include_directories(.)

file(GLOB TEST_SRC *.cpp)

add_executable(switch_differences ${TEST_SRC})
target_link_libraries(switch_differences ${LIBXML2_LIBRARIES})
set_target_properties(switch_differences PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
