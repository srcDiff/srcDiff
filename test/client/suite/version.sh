#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file version.sh
#
# @copyright Copyright (C) 2024-2025 SDML (www.srcDiff.org)
#
# This file is part of the srcDiff Infrastructure.
#

# test framework
source $(dirname "$0")/../framework.sh

define output <<- 'STDOUT'
	srcdiff REVISION
	STDOUT

srcdiff --version | head -n 1
check "$output"

srcdiff -V | head -n 1
check "$output"
