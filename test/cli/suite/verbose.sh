#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file verbose.sh
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

define output <<- 'STDOUT'
	1 sub/a.cpp|sub/b.cpp
	STDOUT

createfile sub/a.cpp "$original"
createfile sub/b.cpp "$modified"

srcdiff sub/a.cpp sub/b.cpp -v | head -n 1
check "$output"