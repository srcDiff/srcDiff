#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file unit-attributes.sh
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define versioned_filename <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="x.cpp|y.cpp"><diff:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:insert>
	</unit>
	STDOUT

define versioned_version <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" language="C++" filename="sub/a.cpp|sub/b.cpp" version="1|2"><diff:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:insert>
	</unit>
	STDOUT

define archive_filename <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0">

	<unit revision="1.0.0" language="C++" filename="c.cpp"><diff:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:insert>
	</unit>

	</unit>
	STDOUT

define multiple_attributes <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" xmlns:diff="http://www.srcML.org/srcDiff" revision="1.0.0" version="3">

	<unit revision="1.0.0" language="C++" filename="c.cpp" version="3"><diff:delete type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:insert>
	</unit>

	<unit revision="1.0.0" language="C++" filename="c.cpp" version="3"><diff:delete type="replace"><expr_stmt><expr><name>b</name></expr>;</expr_stmt></diff:delete><diff:insert type="replace"><expr_stmt><expr><name>a</name></expr>;</expr_stmt></diff:insert>
	</unit>

	</unit>
	STDOUT

xmlcheck "$versioned_filename"
xmlcheck "$versioned_version"
xmlcheck "$archive_filename"
xmlcheck "$multiple_attributes"

createfile sub/a.cpp "a;\n"
createfile sub/b.cpp "b;\n"

srcdiff sub/a.cpp sub/b.cpp -f 'x.cpp|y.cpp'
check "$versioned_filename"

srcdiff sub/a.cpp sub/b.cpp -s '1|2'
check "$versioned_version"

srcdiff sub/a.cpp sub/b.cpp -f c.cpp -n
check "$archive_filename"

srcdiff sub/a.cpp sub/b.cpp sub/b.cpp sub/a.cpp -f c.cpp -s 3 -q
check "$multiple_attributes"
