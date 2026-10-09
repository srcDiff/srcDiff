#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file directory.sh
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define progress <<- 'STDOUT'
	- original|modified
	1 original/A.java|modified/A.java
	2 original/a.cpp|modified/a.cpp
	3 original/deleted.cpp|
	4 |modified/inserted.cpp
	- original/notes.txt|modified/notes.txt
	- |modified/only_modified
	5 |modified/only_modified/r.cpp
	- original/only_original|
	6 original/only_original/q.cpp|
	- original/sub|modified/sub
	7 original/sub/c.cpp|modified/sub/c.cpp
	STDOUT

define units <<- 'STDOUT'
	<unit revision="1.0.0" language="Java" filename="original/A.java|modified/A.java"><class>class <name><diff:delete type="replace">A</diff:delete><diff:insert type="replace">B</diff:insert></name> <block>{}</block></class>
	</unit>

	<unit revision="1.0.0" language="C++" filename="original/a.cpp|modified/a.cpp"><diff:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:insert>
	</unit>

	<unit revision="" language="C++|" filename="original/deleted.cpp|"><diff:delete><expr_stmt><expr><name>x</name></expr>;</expr_stmt><diff:ws>
	</diff:ws></diff:delete></unit>

	<unit revision="" language="|C++" filename="|modified/inserted.cpp"><diff:insert><expr_stmt><expr><name>y</name></expr>;</expr_stmt><diff:ws>
	</diff:ws></diff:insert></unit>

	<unit revision="" language="|C++" filename="|modified/only_modified/r.cpp"><diff:insert><expr_stmt><expr><name>r</name></expr>;</expr_stmt><diff:ws>
	</diff:ws></diff:insert></unit>

	<unit revision="" language="C++|" filename="original/only_original/q.cpp|"><diff:delete><expr_stmt><expr><name>q</name></expr>;</expr_stmt><diff:ws>
	</diff:ws></diff:delete></unit>

	<unit revision="1.0.0" language="C++" filename="original/sub/c.cpp|modified/sub/c.cpp"><diff:delete type="replace"><expr_stmt><expr><name>c</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>d</name></expr>;</expr_stmt></diff:insert>
	</unit>

	</unit>
	STDOUT

define header <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" url="original|modified">

	STDOUT

define header_slash <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" url="original/|modified/">

	STDOUT

output="${header}${units}"
output_slash="${header_slash}${units}"

xmlcheck "$output"
xmlcheck "$output_slash"

createfile original/a.cpp "a;\n"
createfile modified/a.cpp "b;\n"
createfile original/A.java "class A {}\n"
createfile modified/A.java "class B {}\n"
createfile original/deleted.cpp "x;\n"
createfile modified/inserted.cpp "y;\n"
createfile original/notes.txt "z\n"
createfile modified/notes.txt "w\n"
createfile original/only_original/q.cpp "q;\n"
createfile modified/only_modified/r.cpp "r;\n"
createfile original/sub/c.cpp "c;\n"
createfile modified/sub/c.cpp "d;\n"

srcdiff original modified
check "${progress}${output}"

srcdiff original modified -q
check "$output"

srcdiff original/ modified/ -q
check "$output_slash"

srcdiff original modified -o original/out.xml
check original/out.xml "$output"
rmfile original/out.xml
