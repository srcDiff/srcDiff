## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#   Copyright (C) 2011-2021  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.


# include sub directories
file(GLOB COMMON_HEADERS *.hpp ${CMAKE_SOURCE_DIR}/src/translator/srcdiff_constants.hpp)
file(GLOB COMMON_SOURCE *.cpp)
add_library(common OBJECT ${COMMON_HEADERS}	${COMMON_SOURCE})
