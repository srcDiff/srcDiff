#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file output-is-input.sh
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define original <<- 'SOURCE'
	a;
	SOURCE

define error <<- 'STDERR'
	Error: Input source 'sub/a.cpp' same as output filename
	STDERR

createfile sub/a.cpp "$original"
createfile sub/b.cpp "b;\n"

srcdiff sub/a.cpp sub/b.cpp -o sub/a.cpp
check_exit 1 "$error"

check_file sub/a.cpp <(echo -n "$original")
