## SPDX-License-Identifier: GPL-3.0-only
#  CMakeLists.txt
#
#  Copyright (C) 2014-2024  SDML www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#

# include sub directories
if(BUILD_UNIT_TESTS)
    add_subdirectory(unit)
endif()

if(BUILD_TRANSLATOR_TESTS)
    add_subdirectory(src)
endif()
