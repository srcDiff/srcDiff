##
#  FindLibGit2.cmake
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
# LIBGIT2_FOUND       indicates if libgit is found
# LIBGIT2_INCLUDE_DIR libgit include directory
# LIBGIT2_LIBRARIES   libgit libraries

find_path(LIBGIT2_INCLUDE_DIR NAMES git2.h HINTS /usr/local/include /usr/include)

find_library(LIBGIT2_GIT2     NAMES git2   HINTS /usr/local/lib /usr/lib /usr/lib64)

set(LIBGIT2_LIBRARIES "")

if(LIBGIT2_GIT2)
    list(APPEND LIBGIT2_LIBRARIES ${LIBGIT2_GIT2})
endif()

find_path(FIND_PACKAGE_HANDLE_STANDARD_ARGS_PATH NAMES FindPackageHandleStandardArgs.cmake 
	HINTS /usr/local/share/cmake/Modules /usr/local/share/cmake-2.8/Modules /usr/local/share/cmake-3.2/Modules /usr/share/cmake/Modules /usr/share/cmake-2.8/Modules /usr/share/cmake-3.2/Modules)
include(${FIND_PACKAGE_HANDLE_STANDARD_ARGS_PATH}/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(LibGit2 REQUIRED_VARS LIBGIT2_LIBRARIES LIBGIT2_INCLUDE_DIR)

mark_as_advanced(LIBGIT2_LIBRARIES LIBGIT2_INCLUDE_DIR)