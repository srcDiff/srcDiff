## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff Infrastructure.
#

file(GLOB WINDOWS_HEADERS *.hpp)
file(GLOB WINDOWS_SOURCE *.cpp)

# srcdiff
add_library(windows OBJECT ${WINDOWS_HEADERS} ${WINDOWS_SOURCE})

