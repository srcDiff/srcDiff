##
#  FindLibSvn.cmake
#
#  Copyright (C) 2014 SDML (www.sdml.info)
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
# Based off of FindLibXml2.cmake by Alexander Neundorf
#
# LIBSVN_FOUND       indicates if libsvn is found
# LIBSVN_INCLUDE_DIR libsvn include directory
# LIBSVN_LIBRARIES   libsvn libraries

find_path(LIBSVN_INCLUDE_DIR NAMES svn_types.h HINTS /usr/local/include/subversion-1 /usr/include/subversion-1)

find_library(LIBSVN_CLIENT   NAMES svn_client-1   HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_FS_UTIL  NAMES svn_fs_util-1  HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_REPOS    NAMES svn_repos-1    HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_DELTA    NAMES svn_delta-1    HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_RA       NAMES svn_ra-1       HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_SUBR     NAMES svn_subr-1     HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_DIFF     NAMES svn_diff-1     HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_RA_LOCAL NAMES svn_ra_local-1 HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_WC       NAMES svn_wc-1       HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_FS       NAMES svn_fs-1       HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_RA_SERF  NAMES svn_ra_serf-1  HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_FS_FS    NAMES svn_fs_fs-1    HINTS /usr/local/lib /usr/lib /usr/lib64)
find_library(LIBSVN_RA_SVN   NAMES svn_ra_svn-1   HINTS /usr/local/lib /usr/lib /usr/lib64)

set(LIBSVN_LIBRARIES "")

if(LIBSVN_CLIENT)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_CLIENT})
endif()

if(LIBSVN_FS_UTIL)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_FS_UTIL})
endif()

if(LIBSVN_REPOS)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_REPOS})
endif()

if(LIBSVN_DELTA)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_DELTA})
endif()

if(LIBSVN_RA)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_RA})
endif()

if(LIBSVN_SUBR)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_SUBR})
endif()

if(LIBSVN_DIFF)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_DIFF})
endif()

if(LIBSVN_RA_LOCAL)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_RA_LOCAL})
endif()

if(LIBSVN_WC)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_WC})
endif()

if(LIBSVN_FS)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_FS})
endif()

if(LIBSVN_RA_SERF)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_RA_SERF})
endif()

if(LIBSVN_FS_FS)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_FS_FS})
endif()

if(LIBSVN_RA_SVN)
    list(APPEND LIBSVN_LIBRARIES ${LIBSVN_RA_SVN})
endif()

find_path(FIND_PACKAGE_HANDLE_STANDARD_ARGS_PATH NAMES FindPackageHandleStandardArgs.cmake 
    HINTS /usr/local/share/cmake/Modules /usr/local/share/cmake-2.8/Modules /usr/share/cmake/Modules /usr/share/cmake-2.8/Modules)

include(${FIND_PACKAGE_HANDLE_STANDARD_ARGS_PATH}/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(LibSvn REQUIRED_VARS LIBSVN_LIBRARIES LIBSVN_INCLUDE_DIR)

mark_as_advanced(LIBSVN_LIBRARIES LIBSVN_INCLUDE_DIR)