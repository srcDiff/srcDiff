## SPDX-License-Identifier: GPL-3.0-only
#
# @file FindCLI11.cmake
#
# @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

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
