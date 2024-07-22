// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file theme.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_THEME_HPP
#define INCLUDED_THEME_HPP

#include <keywords.hpp>

#include <vector>
#include <string>
#include <iostream>

class theme_t {

public:

    std::string background_color;
    std::string text_color;
    std::string line_number_color;

    std::string common_color;
    std::string delete_color;
    std::string insert_color;

    std::string keyword_color;
    std::string storage_color;
    std::string type_color;

    std::string comment_color;
    std::string number_color;
    std::string string_color;

    std::string function_name_color;
    std::string class_name_color;
    std::string call_name_color;

protected:

    keywords_t * keywords;

    enum level_t { NONE, PARTIAL, FULL } level;

public:

    theme_t(const std::string & highlight_level, bool is_html);
    ~theme_t();

    void set_langauge(const std::string & language);
    bool is_keyword(const std::string & token) const;
    std::string token2color(const std::string & token,
                            bool in_comment,
                            bool in_literal,
                            bool in_string,
                            bool in_function_name,
                            bool in_class_name,
                            bool in_call_name,
                            bool in_preprocessor_directive) const;

};

#endif
