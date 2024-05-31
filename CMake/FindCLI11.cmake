## SPDX-License-Identifier: GPL-3.0-only
#
#  Copyright (C) 2011-2024  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
#  This retrieves CLI11 from Github and makes it available as a library. Or, if
#  vcpkg is being used, it defers to vcpkg. This file is run whenever
#  find_package(CLI11 [...]) is called.

if(NOT CMAKE_USE_VCPKG)
    set(CLI11_BUILD_TESTS OFF)
    set(CLI11_BUILD_DOCS OFF)
    set(CLI11_BUILD_EXAMPLES OFF)
    set(CLI11_SINGLE_FILE OFF)
    set(CLI11_INSTALL OFF)
    set(CLI11_PRECOMPILED ON)

    include(FetchContent)
    FetchContent_Declare(
        cli11_proj
        QUIET
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.4.0
    )
    FetchContent_MakeAvailable(cli11_proj)
else()
    find_package(CLI11 CONFIG REQUIRED)
endif()
