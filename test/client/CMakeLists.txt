# SPDX-License-Identifier: GPL-3.0-only
##
# @file CMakeLists.txt
# 
# @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
# 
# CMake file for client tests

get_target_property(SRCDIFF_DIR srcdiff RUNTIME_OUTPUT_DIRECTORY)
if(WIN32)
	set(SRCDIFF_PATH ${SRCDIFF_DIR}/srcdiff.exe)
else()
	set(SRCDIFF_PATH ${SRCDIFF_DIR}/srcdiff)
endif()

configure_file(framework.sh.in ${CMAKE_CURRENT_SOURCE_DIR}/framework.sh USE_SOURCE_PERMISSIONS @ONLY)

add_subdirectory(suite)
