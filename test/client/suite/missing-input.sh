#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file missing-input.sh
#
# @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define output <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	STDOUT

define missing_modified <<- 'STDERR'
	Error: Input source 'sub/missing.cpp' could not be opened
	STDERR

define missing_both <<- 'STDERR'
	Error: Input sources 'sub/missing.cpp' and 'sub/missing2.cpp' could not be opened
	STDERR

define missing_xml <<- 'STDERR'
	Error: Input source 'sub/missing.xml' could not be opened
	STDERR

createfile sub/a.cpp "a;\n"

srcdiff sub/a.cpp sub/missing.cpp
check "$output" "$missing_modified"

srcdiff sub/missing.cpp sub/a.cpp
check "$output" "$missing_modified"

srcdiff sub/missing.cpp sub/missing2.cpp
check "$output" "$missing_both"

srcdiff sub/missing.xml -u
check "" "$missing_xml"
