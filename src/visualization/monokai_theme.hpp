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

    std::string common_color;
    std::string delete_color;
    std::string insert_color;

    std::string keyword_color;
    std::string storage_color;
    std::string type_color;
    std::string comment_color;
    std::string number_color;
    std::string string_color;

private:
    cpp_keywords keywords;

public:

    monokai_theme(bool is_html);
    std::string token2color(const std::string & token, const std::string & parent) const;

};

#endif
