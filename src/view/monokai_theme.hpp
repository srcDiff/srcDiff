// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file monokai_theme.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_MONOKAI_THEME_HPP
#define INCLUDED_MONOKAI_THEME_HPP

#include <theme.hpp>

#include <string>
#include <iostream>

class monokai_theme : public theme_t {

private: 
public:

    monokai_theme(const std::string & highlight_level, bool is_html);

};

#endif
