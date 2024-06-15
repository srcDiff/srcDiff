## SPDX-License-Identifier: GPL-3.0-only
#  toolchain-gcc.cmake
#
#  @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff Infrastructure.

# GCC-specific tool configuration
# 
# Link the stdc++fs library if on gcc 7.5.0
if(CMAKE_CXX_COMPILER_VERSION STREQUAL "7.5.0")
    link_libraries(stdc++fs)
endif()



set(CMAKE_POSITION_INDEPENDENT_CODE ON)
