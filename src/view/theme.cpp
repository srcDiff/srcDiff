// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file theme.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <theme.hpp>

#include <cpp_keywords.hpp>
#include <java_keywords.hpp>

theme_t::theme_t(const std::string & highlight_level, bool is_html [[maybe_unused]]) : keywords(nullptr) {

    if(highlight_level == "full") {
        level = FULL;
    }
    else if(highlight_level == "partial") {
        level = PARTIAL;
    }
    else {
        level = NONE;
    }

}


theme_t::~theme_t() {

    if(keywords) delete keywords;

}

void theme_t::set_langauge(const std::string & language) {

    if(language == "Java") keywords = new java_keywords(*this);
    else                   keywords = new cpp_keywords(*this);

}

bool theme_t::is_keyword(const std::string & token) const {

    try {

        keywords->color(token);
        return true;

    } catch(const std::out_of_range & e) {}

    return false;


}

std::string theme_t::token2color(const std::string & token,
                                 bool in_comment,
                                 bool in_literal,
                                 bool in_string,
                                 bool in_function_name,
                                 bool in_class_name,
                                 bool in_call_name,
                                 bool in_preprocessor_directive) const {
    
    if(in_comment)       return comment_color;
    if(in_literal)       return number_color;
    if(in_string)        return string_color;

    if(level == FULL) {

        if(in_function_name) return function_name_color;
        if(in_class_name)    return class_name_color;
        if(in_call_name)     return call_name_color;

    }

    if(in_preprocessor_directive) return keyword_color;

    try {

        return keywords->color(token);

    } catch(const std::out_of_range & e) {}

    return "";

}
