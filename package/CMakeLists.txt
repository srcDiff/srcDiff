# SPDX-License-Identifier: GPL-3.0-only
#
# @file package.cmake
#
# @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
#
# CPack configuration for all installers

# Copy preset file to build directory
configure_file(${CMAKE_SOURCE_DIR}/CMakePresets.json ${CMAKE_BINARY_DIR} COPYONLY)

# Copy user preset file to the build directory if it exists
if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeUserPresets.json)
    configure_file(${CMAKE_SOURCE_DIR}/CMakeUserPresets.json ${CMAKE_BINARY_DIR} COPYONLY)
endif()

# especially for archives
set(CPACK_COMPONENTS_GROUPING "ONE_PER_GROUP")

# Package name
set(CPACK_PACKAGE_NAME "srcdiff")

# Package release number (NOT srcdiff or libsrcdiff release)
# Note: Update when package is updated, but not contents
set(SRCDIFF_PACKAGE_RELEASE 1)

# summary
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "srcDiff Infrastructure")

# description
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/package/welcome.html)

# vendor
set(CPACK_PACKAGE_VENDOR "srcML, LLC.")

# contact
set(CPACK_PACKAGE_CONTACT "Software Development Laboratories <srcDiff.org>")

# package version
# set as part of project()

# license
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING.txt)

# README
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

# welcome message
set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_SOURCE_DIR}/package/welcome.html)

# strip executables
set(CPACK_STRIP_FILES ON)

# Generate user and development tar.gz's
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# Client component, SRCDIFF, is just one component so it appears as on item
# for installers that let you pick what to install
# Development group, SRCDIFFDEV, is composed of multiple components
include(CPackComponent)
cpack_add_install_type(CLIENT DISPLAY_NAME "srcdiff cli")
cpack_add_install_type(DEVELOPER DISPLAY_NAME "development")

cpack_add_component(SRCDIFF
                    DISPLAY_NAME "srcdiff"
                    DESCRIPTION "srcdiff cli program with required libraries"
                    REQUIRED
                    INSTALL_TYPES CLIENT DEVELOPER)

cpack_add_component(DEVLIBS
                    DISPLAY_NAME "libsrcdiff"
                    DESCRIPTION "Include file, development libraries, i.e.., static library, and CMake configuration"
                    INSTALL_TYPES DEVELOPER)

# Copy and rename the README file for the proper extension
configure_file(${CMAKE_SOURCE_DIR}/README.md ${CMAKE_BINARY_DIR}/README.txt COPYONLY)
set(CPACK_RESOURCE_FILE_README ${CMAKE_BINARY_DIR}/README.txt)

# Test targets for workflows
set(SRCDIFF_TEST_TIMEOUT 5 CACHE STRING "ctest timeout in seconds")
function(add_workflow_test_targets binary_dir output_directory package_file_name)

    # Target for cli tests
    add_custom_target(cli
        WORKING_DIRECTORY ${binary_dir}
        COMMAND ${CMAKE_CTEST_COMMAND} -R ^srcdiff --timeout ${SRCDIFF_TEST_TIMEOUT} --output-log ${output_directory}/${package_file_name}-client.log || true
    )

    # Target for unit tests
    add_custom_target(unit
        WORKING_DIRECTORY ${binary_dir}
        COMMAND ${CMAKE_CTEST_COMMAND} -R ^unit --timeout ${SRCDIFF_TEST_TIMEOUT} --output-log ${output_directory}/${package_file_name}-unit.log || true
    )

endfunction()

# Generators are defined in the *.cpack files
set(CPACK_GENERATOR)
file(GLOB INSTALLERS "*.cpack")
foreach(INSTALLER IN ITEMS ${INSTALLERS})
    include(${INSTALLER})
endforeach()

message(STATUS "CPack generators: ${CPACK_GENERATOR}")

# Hide the LOCAL components
get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
list(REMOVE_ITEM CPACK_COMPONENTS_ALL "LOCAL")

# needs to be last so not overwritten
include(CPack)
