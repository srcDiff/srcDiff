## SPDX-License-Identifier: GPL-3.0-only
#
#  Copyright (C) 2011-2024  SDML (www.srcDiff.org)
#
#  This file is part of the srcDiff translator.
# copied from the official x86-windows triplet:
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

# on windows, vcpkg builds dependencies in an isolated environment where they
# cannot access things like the location of Java. libsrcml needs Java to build.
# this lets our dependencies access java:
set(VCPKG_ENV_PASSTHROUGH_UNTRACKED JAVA_HOME PATH)
