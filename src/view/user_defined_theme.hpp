// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file user_defined_theme.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_USER_DEFINED_THEME_HPP
#define INCLUDED_USER_DEFINED_THEME_HPP

#include <theme.hpp>

#include <string>
#include <iostream>

class user_defined_theme : public theme_t {

private:

public:

    user_defined_theme(const std::string & highlight_level,
                       bool is_html,
                       const std::string & theme_filename);

};

#endif
