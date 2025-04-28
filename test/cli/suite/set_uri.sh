#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file new-prefix.sh
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

define new_uri_output <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" xmlns:prefix="clitest" revision="1.0.0" language="C++" filename="sub/a.cpp|sub/b.cpp"><diff:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:insert>
	</unit>
	STDOUT

define change_prefix_output <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:delta="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="sub/a.cpp|sub/b.cpp"><delta:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></delta:delete><delta:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></delta:insert>
	</unit>
	STDOUT

xmlcheck "$output"

createfile sub/a.cpp "$original"
createfile sub/b.cpp "$modified"

srcdiff sub/a.cpp sub/b.cpp --xmlns:prefix=clitest

check "$new_uri_output"

srcdiff sub/a.cpp sub/b.cpp --xmlns:delta=http://www.srcML.org/srcDiff

check "$new_uri_output"


