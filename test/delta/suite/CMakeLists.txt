## SPDX-License-Identifier: GPL-3.0-only
#
# @file CMakeLists.txt
#
# @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

file(GLOB TEST_FILES *.xml)

foreach(PATH IN ITEMS ${TEST_FILES})
    get_filename_component(FILENAME ${PATH} NAME)
    configure_file(${FILENAME} ${FILENAME} NEWLINE_STYLE LF)
endforeach()
