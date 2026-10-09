#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file odd-inputs.sh
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define odd <<- 'STDERR'
	Odd number of input files.
	Run with --help for more information.
	STDERR

createfile sub/a.cpp "a;\n"
createfile sub/b.cpp "b;\n"

srcdiff sub/a.cpp
check_exit 105 "$odd"

srcdiff sub/a.cpp sub/b.cpp sub/a.cpp
check_exit 105 "$odd"
