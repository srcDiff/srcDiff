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

file(READ framework.sh.in FRAMEWORK_STR)
string(REPLACE "SRCDIFF_PATH" "${SRCDIFF_PATH}" FRAMEWORK_STR "${FRAMEWORK_STR}")
file(WRITE framework.sh "${FRAMEWORK_STR}")
file(CHMOD ${CMAKE_CURRENT_SOURCE_DIR} 
     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
                 GROUP_READ GROUP_EXECUTE
                 WORLD_READ WORLD_EXECUTE)

add_subdirectory(suite)
