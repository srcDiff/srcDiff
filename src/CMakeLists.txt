## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2013-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

add_subdirectory(client)
add_subdirectory(shortest_edit_script)
add_subdirectory(srcml)
add_subdirectory(translator)
add_subdirectory(model)
add_subdirectory(project)
add_subdirectory(view)
add_subdirectory(srcSAX/src)

if(WIN32)
    add_subdirectory(windows)
endif()

# WINDOWS_EXPORT_ALL_SYMBOLS has to be true for srcsax_shared for Clang to be
# able to link srcdiff.exe to it
set_target_properties(srcsax_shared PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

set(SRCDIFF_LIBRARY_SOURCE_EXTRA CACHE INTERNAL "libsrcdiff Extra Source")

find_package(srcML REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Threads REQUIRED)
find_package(CLI11 REQUIRED)

#
# build_library
#
# Used to help with the creation of the srcdiff library.
# - LIBRARY_NAME is the name of the library and target
# - LIBRARY_TYPE is either STATIC or SHARED.
# 
macro(build_library LIBRARY_NAME LIBRARY_TYPE LIBRARY_OUTPUT_NAME)

    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        add_library(${LIBRARY_NAME} ${LIBRARY_TYPE} $<TARGET_OBJECTS:shortest_edit_script> 
        $<TARGET_OBJECTS:srcml> $<TARGET_OBJECTS:translator> $<TARGET_OBJECTS:model> $<TARGET_OBJECTS:nest> $<TARGET_OBJECTS:convert> $<TARGET_OBJECTS:project>
        $<TARGET_OBJECTS:view>)
    else()
        add_library(${LIBRARY_NAME} ${LIBRARY_TYPE} $<TARGET_OBJECTS:shortest_edit_script> 
        $<TARGET_OBJECTS:srcml> $<TARGET_OBJECTS:translator> $<TARGET_OBJECTS:model> $<TARGET_OBJECTS:nest> $<TARGET_OBJECTS:convert> $<TARGET_OBJECTS:project>
        $<TARGET_OBJECTS:view>)
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set_target_properties(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${LIBRARY_OUTPUT_NAME} LINK_FLAGS_DEBUG "/SAFESEH:NO")
    elseif(APPLE)
        set_target_properties(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${LIBRARY_OUTPUT_NAME})
    else()
        set_target_properties(${LIBRARY_NAME} PROPERTIES OUTPUT_NAME ${LIBRARY_OUTPUT_NAME})
    endif()

    target_link_libraries(${LIBRARY_NAME} PRIVATE srcsax_shared srcML::LibsrcML LibXml2::LibXml2 CLI11::CLI11)

endmacro()

# Building static and dynamic libraries for srcDiff.
if (NOT DEFINED STATIC_LIB_NAME)
    set(STATIC_LIB_NAME srcdiff)
endif()

build_library(srcdiff_static STATIC ${STATIC_LIB_NAME})
set_target_properties(srcdiff_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

build_library(srcdiff_shared SHARED srcdiff)
set_target_properties(srcdiff_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# which types of libraries
install(TARGETS srcdiff_shared srcdiff_static COMPONENT DEVLIBS)
