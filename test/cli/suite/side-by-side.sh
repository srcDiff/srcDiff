#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file side-by-side.sh
#
# @copyright Copyright (C) 2024-2025 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define original <<- 'SOURCE'
	a;
	SOURCE

define modified <<- 'SOURCE'
	b;
	SOURCE

define_no_eof_nl output <<- 'STDOUT'
	\x1b[0m\x1b[0m\x1b[9;48;5;217;1ma;\x1b[0m | \x1b[48;5;120;1mb;\x1b[0m\x1b[0m\n\x1b[0m  
	\x1b[0m\x1b[0m
	STDOUT

createfile sub/a.cpp "$original"
createfile sub/b.cpp "$modified"
createfile sub/out.txt "$output"

srcdiff sub/a.cpp sub/b.cpp -y 1

check "$output"
