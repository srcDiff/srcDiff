# SPDX-License-Identifier: GPL-3.0-only
##
# @file CMakeLists.txt
# 
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#
#  CMake for examples directory.

# Turn ON/OFF building examples
option(BUILD_EXAMPLES "Build examples usage files for libsrcdiff" OFF)

if(BUILD_ALL)
    set(BUILD_EXAMPLES ON)
endif()

add_subdirectory(libsrcdiff)
