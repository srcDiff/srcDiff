#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file xml-input.sh
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define delta <<- 'SOURCE'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="sub/a.cpp|sub/b.cpp"><diff:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:insert>
	</unit>
	SOURCE

define_no_eof_nl unified <<- 'STDOUT'
	\x1b[0m\x1b[0m\x1b[36m@@ -1 +1 @@\x1b[0m
	\x1b[9;48;5;217;1ma;\x1b[48;5;120;1mb;\x1b[0m
	\x1b[0m
	STDOUT

define_no_eof_nl side_by_side <<- 'STDOUT'
	\x1b[0m\x1b[0m\x1b[9;48;5;217;1ma;\x1b[0m | \x1b[48;5;120;1mb;\x1b[0m
	\x1b[0m\x1b[0m     \x1b[0m
	\x1b[0m
	STDOUT

define no_view <<- 'STDERR'
	XML input requires either --unified or --side-by-side to be set.
	Run with --help for more information.
	STDERR

xmlcheck "$delta"

createfile sub/delta.xml "$delta"

srcdiff sub/delta.xml -u
check "$unified"

srcdiff sub/delta.xml -y 4
check "$side_by_side"

srcdiff sub/delta.xml
check_exit 105 "$no_view"
