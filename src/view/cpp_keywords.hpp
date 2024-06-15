// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file cpp_keywords.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CPP_KEYWORDS_HPP
#define INCLUDED_CPP_KEYWORDS_HPP

#include <keywords.hpp>

#include <unordered_map>
#include <string>

class theme_t;

class cpp_keywords : public keywords_t {

public:

    cpp_keywords(const theme_t & theme);

};

#endif
