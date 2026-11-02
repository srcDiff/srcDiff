// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file python_keywords.hpp
 *
 * @copyright Copyright (C) 2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_PYTHON_KEYWORDS_HPP
#define INCLUDED_PYTHON_KEYWORDS_HPP

#include <keywords.hpp>

#include <unordered_map>
#include <string>

class theme_t;

class python_keywords : public keywords_t {

public:

    python_keywords(const theme_t & theme);

};

#endif
