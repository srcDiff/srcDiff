#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file unknown-extension.sh
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define skipped <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	STDOUT

define output <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="sub/a.txt|sub/b.txt"><diff:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:insert>
	</unit>
	STDOUT

xmlcheck "$output"

createfile sub/a.txt "a;\n"
createfile sub/b.txt "b;\n"

srcdiff sub/a.txt sub/b.txt
check "$skipped"

srcdiff sub/a.txt sub/b.txt --register-ext txt=C++
check "$output"
