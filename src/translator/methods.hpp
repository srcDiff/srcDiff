// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file methods.hpp
 *
 * @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_METHOD_HPP
#define INCLUDED_METHOD_HPP

#include <string>

#ifdef __GNUC__
  typedef unsigned long long METHOD_TYPE;
#else
  typedef unsigned __int64 METHOD_TYPE;
#endif

#ifdef __GNUC__
#define ull(a) a##ULL
#else
#define ull(a) a##i64
#endif

inline bool ismethod(METHOD_TYPE methods, METHOD_TYPE flag) {
  return (flag & methods) > 0;
}

const std::string COLLECT_METHOD = "collect";
const std::string RAW_METHOD = "raw";

// @srcdiff
const METHOD_TYPE METHOD_RAW       = ull(1) << 0;

const std::string GROUP_DIFF_METHOD = "group-diff";
const std::string NO_GROUP_DIFF_METHOD = "no-group-diff";

// @srcdiff
const METHOD_TYPE METHOD_GROUP       = ull(1) << 1;

#endif
