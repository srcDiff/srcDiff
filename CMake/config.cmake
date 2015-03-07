##
#  config.cmake
#
#  Copyright (C) 2011-2011 SDML (www.sdml.info)
#
#  This file is part of the srcDiff translator.
#
#  The srcDiff translator is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  The srcDiff translator is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the srcDiff translator; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#  Build configuration file

# build options
option(BUILD_TRANSLATOR_TESTS "Build files for testing srcDiff translator" ON)

option(BUILD_UNIT_TESTS       "Build unit tests for srcDiff"              OFF)

option(SVN_ENABLED            "Build in svn source input support"         OFF)

option(GIT_ENABLED            "Build in git source input support"         OFF)

if(SVN_ENABLED)

find_package(LibApr REQUIRED)
find_package(LibSvn REQUIRED)

add_definitions("-DSVN")

endif()

if(GIT_ENABLED)

find_package(LibGit2 REQUIRED)

add_definitions("-DGIT")

endif()

# find needed libraries
find_library(LIBSRCML_LIBRARY NAMES libsrcml.dll libsrcml.a PATHS /usr/local/lib)
set(LIBSRCML_INCLUDE_DIR /usr/local/include)

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options filesystem system thread regex date_time REQUIRED)

find_package(LibXml2 REQUIRED)

find_library(ANTLR_LIBRARY NAMES libantlr-pic.a libantlr.a libantlr2-0.dll antlr.lib PATHS /usr/lib /usr/local/lib)

# Set libsrcdiff libraries
set(LIBSRCDIFF_LIBRARIES ${LIBSRCML_LIBRARY} ${Boost_LIBRARIES} ${LIBXML2_LIBRARIES} ${ANTLR_LIBRARY} pthread ${LIBAPR_LIBRARIES} ${LIBSVN_LIBRARIES} ${LIBGIT2_LIBRARIES} crypto dl CACHE INTERNAL "libsrcdiff Link Libraries")

# Set libsrcdiff libraries
set(SRCDIFF_LIBRARIES crypto CACHE INTERNAL "srcdiff Link Libraries")

# Do not use rpath on OSX
set(CMAKE_MACOSX_RPATH OFF)

if(NOT WIN32 AND NOT APPLE)
list(APPEND SRCDIFF_LIBRARIES rt)
endif()

# include needed includes
include_directories(${LIBSRCML_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${LIBAPR_INCLUDE_DIR} ${LIBSVN_INCLUDE_DIR} ${LIBGIT2_INCLUDE_DIR})

# The default configuration is to compile in DEBUG mode. These flags can be directly
# overridden by setting the property of a target you wish to change them for.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(${CMAKE_COMPILER_IS_GNUCXX})

    string(FIND ${CMAKE_CXX_COMPILER} "mingw32" IS_MINGW32)
    if(IS_MINGW32 EQUAL -1)
      set(USE_FPIC -fPIC)
    endif()

    set(GCC_WARNINGS "-Wno-long-long -Wall -Wextra  -Wall -pedantic -Wempty-body -Wignored-qualifiers -Wsign-compare -Wtype-limits -Wuninitialized -Wno-pragmas -Wno-variadic-macros")
    # Adding global compiler definitions.                                                                                      
    set(CMAKE_CXX_FLAGS "${USE_FPIC} -std=c++1y ${GCC_WARNINGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -DSTATIC_GLOBALS")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -DDEBUG --coverage -fprofile-arcs")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

    # Configuring the Clang compiler
    #set(CLANG_WARNINGS "-Wno-long-long -Wall -Wextra -Wshorten-64-to-32 -Wno-unknown-pragmas -Wno-int-to-void-pointer-cast")
    set(CMAKE_CXX_FLAGS "-fPIC -std=c++1y ${CLANG_WARNINGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -DSTATIC_GLOBALS")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -DDEBUG")
    
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

    message(FATAL_ERROR "Configuration Not Implemented: ${CMAKE_CXX_COMPILER_ID}. Build not configured for selected compiler.")
    
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") 

    message(FATAL_ERROR "Configuration Not Implemented: ${CMAKE_CXX_COMPILER_ID}. Build not configured for selected compiler.")

else()

    message(FATAL_ERROR "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}. Build not configured for selected compiler.")
    
endif()