#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file files-from-errors.sh
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define no_separator <<- 'STDERR'
	The following input line did not consist of two filenames separated by '|':
	sub/a.cpp sub/b.cpp
	Run with --help for more information.
	STDERR

define no_pairs <<- 'STDERR'
	No input file pairs could be obtained from comments-only.txt
	Run with --help for more information.
	STDERR

define missing <<- 'STDERR'
	missing.txt was not readable (missing?)
	Run with --help for more information.
	STDERR

createfile sub/a.cpp "a;\n"
createfile sub/b.cpp "b;\n"
createfile no-separator.txt "sub/a.cpp sub/b.cpp\n"
createfile comments-only.txt "# nothing here\n\n"

srcdiff --files-from no-separator.txt
check_exit 105 "$no_separator"

srcdiff --files-from comments-only.txt
check_exit 105 "$no_pairs"

srcdiff --files-from missing.txt
check_exit 103 "$missing"
