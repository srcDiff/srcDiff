##
#  config.cmake
#
#  Copyright (C) 2011-2020 SDML (www.srcDiff.org)
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

option(TIMING                 "Build with and output timing information"  OFF)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    # Adding suspected windows include directory for ANTRL
    include_directories("C:/antlr/277/include/antlr")
    set(WINDOWS_DEP_PATH ${PROJECT_SOURCE_DIR}/dep)
    include_directories(${WINDOWS_DEP_PATH}/include)
    link_directories(${WINDOWS_DEP_PATH}/lib)
 
     # FIXME
    set(LIBXML2_LIBRARIES libxml2.lib iconv.lib)
    include_directories(C:/antlr/277/include)
    set(BOOST_DIR $ENV{BOOST_ROOT})
    include_directories(${BOOST_DIR})
    link_directories(${BOOST_DIR}/stage/lib)

else()

if(SVN_ENABLED)

find_package(LibApr REQUIRED)
find_package(LibSvn REQUIRED)

add_definitions("-DSVN")

endif()

if(GIT_ENABLED)

add_definitions("-DGIT")

endif()

set(LIBSRCML_INCLUDE_DIR /usr/local/include)

set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS OFF)

if(NOT TIMING)

find_package(Boost COMPONENTS program_options filesystem system thread regex date_time REQUIRED)

else()

add_definitions("-DTIMING")
find_package(Boost COMPONENTS program_options filesystem system thread regex date_time timer chrono REQUIRED)

endif()

find_package(LibXml2 REQUIRED)

find_package(Iconv REQUIRED)

# include needed includes
include_directories(${LIBSRCML_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${LIBAPR_INCLUDE_DIR} ${LIBSVN_INCLUDE_DIR} ${LIBGIT2_INCLUDE_DIR})

endif()

# find needed libraries
find_library(LIBSRCML_LIBRARY NAMES libsrcml.so libsrcml.dylib libsrcml.lib PATHS /usr/local/lib ${WINDOWS_DEP_PATH}/lib)

# Locating the antlr library.
find_library(ANTLR_LIBRARY NAMES libantlr-pic.a libantlr.a libantlr2-0.dll antlr.lib PATHS /usr/lib /usr/local/lib ${WINDOWS_DEP_PATH}/lib)

# Set libsrcdiff libraries
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
set(LIBSRCDIFF_LIBRARIES ${LIBSRCML_LIBRARY} ${Boost_LIBRARIES} ${LIBXML2_LIBRARIES} ${Iconv_LIBRARY} ${ANTLR_LIBRARY} ${LIBAPR_LIBRARIES} ${LIBSVN_LIBRARIES} dl CACHE INTERNAL "libsrcdiff Link Libraries")
else()
set(LIBSRCDIFF_LIBRARIES ${LIBSRCML_LIBRARY} ${Boost_LIBRARIES} ${LIBXML2_LIBRARIES} ${Iconv_LIBRARY} ${ANTLR_LIBRARY} ${LIBAPR_LIBRARIES} ${LIBSVN_LIBRARIES} CACHE INTERNAL "libsrcdiff Link Libraries")
endif()

# Set srcdiff libraries
if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
set(SRCDIFF_LIBRARIES CACHE INTERNAL "srcdiff Link Libraries")
else()
set(SRCDIFF_LIBRARIES CACHE INTERNAL "srcdiff Link Libraries")
endif()

# Do not use rpath on OSX
set(CMAKE_MACOSX_RPATH OFF)

if(NOT WIN32 AND NOT APPLE)
list(APPEND SRCDIFF_LIBRARIES rt pthread m)
endif()

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

    set(GCC_WARNINGS "-Wno-long-long -Wall -Wextra -pedantic -Wempty-body -Wignored-qualifiers -Wsign-compare -Wtype-limits -Wuninitialized -Wno-pragmas -Wno-variadic-macros")
    # Adding global compiler definitions.                                                                                      
    set(CMAKE_CXX_FLAGS "${USE_FPIC} -std=c++1y ${GCC_WARNINGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -DSTATIC_GLOBALS")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -DDEBUG --coverage -fprofile-arcs")

    set(CMAKE_C_FLAGS "${USE_FPIC} ${GCC_WARNINGS}")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -DSTATIC_GLOBALS")
    set(CMAKE_C_FLAGS_DEBUG   "-O0 -g -DDEBUG")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")

    # Configuring the Clang compiler
    #set(CLANG_WARNINGS "-Wno-long-long -Wall -Wextra -Wshorten-64-to-32 -Wno-unknown-pragmas -Wno-int-to-void-pointer-cast")
    set(CMAKE_CXX_FLAGS "-fPIC -std=c++1y -fvisibility=default ${CLANG_WARNINGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -DSTATIC_GLOBALS")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -DDEBUG")

    set(CMAKE_C_FLAGS "${USE_FPIC} ${CLANG_WARNINGS}")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -DSTATIC_GLOBALS")
    set(CMAKE_C_FLAGS_DEBUG   "-O0 -g -DDEBUG")
    
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

    message(FATAL_ERROR "Configuration Not Implemented: ${CMAKE_CXX_COMPILER_ID}. Build not configured for selected compiler.")
    
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") 

    # message(STATUS "MSVC Compiler not completely configured yet")
    set(MSVC_WARNINGS "/W3 /wd4068 /wd4101 /D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${MSVC_WARNINGS} /DSTATIC_GLOBALS  /Od /ZI /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MSVC_WARNINGS} /Ox")

else()

    message(FATAL_ERROR "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}. Build not configured for selected compiler.")
    
endif()