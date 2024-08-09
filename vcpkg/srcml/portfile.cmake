## SPDX-License-Identifier: GPL-3.0-only
#
# @file portfile.cmake
#
# @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

vcpkg_from_github(
  OUT_SOURCE_PATH SOURCE_PATH
  REPO srcML/srcML
  REF b5ac74e76d9bc5df55805ae01f1ec6765feb523f
  SHA512 2800ccb4781e6a8cf409d229a49f03ab8a31f327946c619052f043fdd7af2e8293da687df2cdfc31cd908eed915ad98f1cd7ad0487e50227ccbeacd41ff01068
  HEAD_REF main
  PATCHES "skip-client-and-package.patch" "fix-free.patch"
)

vcpkg_cmake_configure(
  SOURCE_PATH "${SOURCE_PATH}"
  OPTIONS
    # fix the fact that vcpkg disables FetchContent in packages' build scripts
    # by default (https://github.com/microsoft/vcpkg/issues/28386)
    -DFETCHCONTENT_FULLY_DISCONNECTED=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup()

# fix warning about missing license
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/COPYING.txt")

# fix warnings about unexpected files
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
