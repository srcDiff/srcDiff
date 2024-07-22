// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file keywords.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_KEYWORDS_HPP
#define INCLUDED_KEYWORDS_HPP

#include <unordered_map>
#include <string>

class theme_t;

class keywords_t {

protected:
    std::unordered_map<std::string, std::string> color_map;

public:

    keywords_t(const theme_t & theme);

    std::string color(const std::string & token) const;

};

#endif
