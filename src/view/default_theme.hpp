// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file default_theme.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_DEFAULT_THEME_HPP
#define INCLUDED_DEFAULT_THEME_HPP

#include <theme.hpp>

#include <string>
#include <iostream>

class default_theme : public theme_t {

private: 
public:

    default_theme(const std::string & highlight_level, bool is_html);

};

#endif
