/**
 *  @file monokai.hpp
 *
 *  Specifies monokai color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_MONOKAI_THEME_HPP
#define INCLUDED_MONOKAI_THEME_HPP

#include <cpp_keywords.hpp>

#include <string>
#include <iostream>

class monokai_theme {

public:

    std::string background_color;
    std::string text_color;

    std::string common_color_html;
    std::string delete_color_html;
    std::string insert_color_html;

    std::string keyword_color_html;
    std::string storage_color_html;
    std::string function_name_color_html;
    std::string class_name_color_html;
    std::string type_color_html;
    std::string call_color_html;
    std::string number_color_html;
    std::string string_color_html;
    std::string comment_color_html;

private:
    cpp_keywords keywords;

public:

    monokai_theme();
    std::string token2color(const std::string & token, const std::string & parent) const;

};

#endif
