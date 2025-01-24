#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file version.sh
#
# @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define output <<- 'STDOUT'
	Translates C, C++, and Java source code into the XML source-code representation srcDiff.
	STDOUT

srcdiff --help | head -n 1
check "$output"

srcdiff -h | head -n 1
check "$output"
