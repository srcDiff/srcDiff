/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
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
