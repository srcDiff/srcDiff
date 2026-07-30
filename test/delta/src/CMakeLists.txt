## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

find_package(LibXml2 REQUIRED)
find_package(Iconv REQUIRED)
find_package(CLI11 REQUIRED)
find_package(srcML REQUIRED)

add_executable(switch_differences switch_differences.cpp)
target_link_libraries(switch_differences LibXml2::LibXml2)
set_target_properties(switch_differences PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(archive_reader archive_reader.cpp)
target_link_libraries(archive_reader CLI11::CLI11 LibXml2::LibXml2 srcML::LibsrcML)
set_target_properties(archive_reader PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
