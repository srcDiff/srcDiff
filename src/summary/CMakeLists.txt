##
#  CMakeLists.txt
#
#  Copyright (C) 2015  SDML (www.sdml.info)
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

file(GLOB SUMMARY_HEADERS *.hpp)
file(GLOB SUMMARY_SOURCE  *.cpp)

list(REMOVE_ITEM SUMMARY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/text_summary_top_down.hpp ${CMAKE_CURRENT_SOURCE_DIR}/text_summary_verbose.hpp)
list(REMOVE_ITEM SUMMARY_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/text_summary_top_down.cpp ${CMAKE_CURRENT_SOURCE_DIR}/text_summary_verbose.cpp)

add_library(summary OBJECT ${SUMMARY_HEADERS} ${SUMMARY_SOURCE})

