# SPDX-License-Identifier: GPL-3.0-only
##
# @file CMakeLists.txt
# 
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
# 
# CMake file for stress tests

get_target_property(SRCDIFF_DIR srcdiff RUNTIME_OUTPUT_DIRECTORY)
if(WIN32)
	set(SRCDIFF_PATH ${SRCDIFF_DIR}/srcdiff.exe)
else()
	set(SRCDIFF_PATH ${SRCDIFF_DIR}/srcdiff)
endif()

configure_file(stress_test.sh.in ${CMAKE_BINARY_DIR}/stress_test.sh USE_SOURCE_PERMISSIONS @ONLY)
