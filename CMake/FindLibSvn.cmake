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

find_path(LIBSVN_INCLUDE_DIR NAMES subversion-1/svn_types.h PATH_SUFFIXES subversion-1 HINTS /usr/local/include /usr/include)

find_library(LIBSVN_CLIENT   NAMES svn_client-1   HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_FS_UTIL  NAMES svn_fs_util-1  HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_REPOS    NAMES svn_repos-1    HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_DELTA    NAMES svn_delta-1    HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_RA       NAMES svn_ra-1       HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_SUBR     NAMES svn_subr-1     HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_DIFF     NAMES svn_diff-1     HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_RA_LOCAL NAMES svn_ra_local-1 HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_WC       NAMES svn_wc-1       HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_FS       NAMES svn_fs-1       HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_RA_SERF  NAMES svn_ra_serf-1  HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_FS_FS    NAMES svn_fs_fs-1    HINTS /usr/local/lib /usr/lib)
find_library(LIBSVN_RA_SVN   NAMES svn_ra_svn-1   HINTS /usr/local/lib /usr/lib)

set(LIBSVN_LIBRARIES ${LIBSVN_CLIENT} ${LIBSVN_FS_UTIL} ${LIBSVN_REPOS} ${LIBSVN_DELTA} ${LIBSVN_RA} ${LIBSVN_SUBR} ${LIBSVN_DIFF}
 ${LIBSVN_RA_LOCAL} ${LIBSVN_WC} ${LIBSVN_FS} ${LIBSVN_RA_SERF} ${LIBSVN_FS_FS} ${LIBSVN_RA_SVN})

find_path(FIND_PACKAGE_HANDLE_STANDARD_ARGS_PATH NAMES FindPackageHandleStandardArgs.cmake HINTS /usr/local/share/cmake/Modules /usr/share/cmake/Modules)

include(${FIND_PACKAGE_HANDLE_STANDARD_ARGS_PATH}/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(LibSvn REQUIRED_VARS LIBSVN_LIBRARIES LIBSVN_INCLUDE_DIR)

mark_as_advanced(LIBSVN_LIBRARIES LIBSVN_INCLUDE_DIR)