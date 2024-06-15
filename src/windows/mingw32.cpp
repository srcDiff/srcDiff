// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file mingw32.cpp
 *
 * @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <mingw32.hpp>

char * strndup(const char * s1, size_t n) {

  char * dest = (char *)malloc((n + 1) * sizeof(char));

  memcpy(dest, s1, n);

  dest[n] = 0;

  return dest;
}
