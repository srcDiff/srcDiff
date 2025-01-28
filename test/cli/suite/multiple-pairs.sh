#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file multiple-pairs.sh
#
# @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
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
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0">

	<unit revision="1.0.0" language="C++" filename="sub/a.cpp|sub/b.cpp"><diff:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:insert>
	</unit>

	<unit revision="1.0.0" language="C++" filename="sub/b.cpp|sub/a.cpp"><diff:delete type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:insert>
	</unit>

	</unit>
	STDOUT

xmlcheck "$output"

createfile sub/a.cpp "$original"
createfile sub/b.cpp "$modified"

srcdiff sub/a.cpp sub/b.cpp sub/b.cpp sub/a.cpp
check "$output"

srcdiff sub/a.cpp sub/b.cpp sub/b.cpp sub/a.cpp -o sub/ab.xml
check sub/ab.xml "$output"