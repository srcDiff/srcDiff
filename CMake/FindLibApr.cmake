##
#  FindLibApr.cmake
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
# LIBAPR_FOUND       indicates if libsvn is found
# LIBAPR_INCLUDE_DIR libsvn include directory
# LIBAPR_LIBRARIES   libsvn libraries

find_path(LIBAPR_INCLUDE_DIR NAMES apr-1/apr.h PATH_SUFFIXES apr-1 HINTS /usr/local/include /usr/include)

find_library(LIBAPR_APR     NAMES apr-1     HINTS /usr/local/lib /usr/lib)
find_library(LIBAPR_APRUTIL NAMES aprutil-1 HINTS /usr/local/lib /usr/lib)

set(LIBAPR_LIBRARIES ${LIBAPR_APR} ${LIBAPR_APRUTIL})

find_path(FIND_PACKAGE_HANDLE_STANDARD_ARGS_PATH NAMES FindPackageHandleStandardArgs.cmake HINTS /usr/local/share/cmake/Modules /usr/share/cmake/Modules)

include(${FIND_PACKAGE_HANDLE_STANDARD_ARGS_PATH}/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(LibApr REQUIRED_VARS LIBAPR_LIBRARIES LIBAPR_INCLUDE_DIR)

mark_as_advanced(LIBAPR_LIBRARIES LIBSVN_INCLUDE_DIR)