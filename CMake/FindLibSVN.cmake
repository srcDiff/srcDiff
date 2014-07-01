##
#  FindLibSVN.cmake
#
#  Copyright (C) 2011-2011  SDML (www.sdml.info)
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

find_path(LIBSVN_INCLUDE_DIR NAMES subvesion-1/svn_types.h PATH_SUFFIXES/subversion-1)

find_library(LIBSVN_CLIENT   NAMES libsvn_client-1)
find_library(LIBSVN_FS_UTIL  NAMES libsvn_fs_util-1)
find_library(LIBSVN_REPOS    NAMES libsvn_repos-1)
find_library(LIBSVN_DELTA    NAMES libsvn_delta-1)
find_library(LIBSVN_RA       NAMES libsvn_ra-1)
find_library(LIBSVN_SUBR     NAMES libsvn_subr-1)
find_library(LIBSVN_DIFF     NAMES libsvn_diff-1)
find_library(LIBSVN_RA_LOCAL NAMES libsvn_ra_local-1)
find_library(LIBSVN_WC       NAMES libsvn_wc-1)
find_library(LIBSVN_FS       NAMES libsvn_fs-1)
find_library(LIBSVN_RA_SERF  NAMES libsvn_ra_serf-1)
find_library(LIBSVN_FS_FS    NAMES libsvn_fs_fs-1)
find_library(LIBSVN_RA_SVN   NAMES libsvn_ra_svn-1)

set(LIBSVN_LIBRARIES ${LIBSVN_CLIENT} ${LIBSVN_FS_UTIL} ${LIBSVN_REPOS} ${LIBSVN_DELTA} ${LIBSVN_RA} ${LIBSVN_SUBR} ${LIBSVN_DIFF}
 ${LIBSVN_RA_LOCAL} ${LIBSVN_WC} ${LIBSVN_FS} ${LIBSVN_RA_SERF} ${LIBSVN_FS_FS} ${LIBSVN_RA_SVN})

include(FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibSVN REQUIRED_VARS LIBSVN_LIBRARIES LIBSVN_INCLUDE_DIR)

mark_as_advanced(LIBSVN_LIBRARIES LIBSVN_INCLUDE_DIR)