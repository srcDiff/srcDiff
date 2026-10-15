## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2014-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

find_package(LibXml2 REQUIRED)
find_package(srcML REQUIRED)

add_executable(test_delta test_delta.cpp delta_utils.cpp delta_utils.hpp)
target_include_directories(test_delta PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_delta srcdiff_static srcML::LibsrcML LibXml2::LibXml2)
set_target_properties(test_delta PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
