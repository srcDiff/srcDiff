#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file files-from-one-sided.sh
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define output <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0">

	<unit revision="" language="C++|" filename="sub/a.cpp|"><diff:delete><expr_stmt><expr><name>a</name></expr>;</expr_stmt><diff:ws>
	</diff:ws></diff:delete></unit>

	<unit revision="" language="|C++" filename="|sub/b.cpp"><diff:insert><expr_stmt><expr><name>b</name></expr>;</expr_stmt><diff:ws>
	</diff:ws></diff:insert></unit>

	</unit>
	STDOUT

xmlcheck "$output"

createfile sub/a.cpp "a;\n"
createfile sub/b.cpp "b;\n"

createfile pairs.txt "sub/a.cpp|\n|sub/b.cpp\n"

srcdiff --files-from pairs.txt -q
check "$output"
