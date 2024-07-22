// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file java_keywords.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_JAVA_KEYWORDS_HPP
#define INCLUDED_JAVA_KEYWORDS_HPP

#include <keywords.hpp>

#include <unordered_map>
#include <string>

class theme_t;

class java_keywords : public keywords_t {

public:

    java_keywords(const theme_t & theme);

};

#endif
