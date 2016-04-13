/**
 *  @file cpp_keywords.hpp
 *
 *  Specifies cpp_keywords color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <cpp_keywords.hpp>
#include <monokai.hpp>

cpp_keywords::cpp_keywords(const monokai & theme) {

    color_map = {

        { "if",       theme.keyword_color_html },
        { "else",     theme.keyword_color_html },
        { "while",    theme.keyword_color_html },
        { "for",      theme.keyword_color_html },
        { "do",       theme.keyword_color_html },
        { "switch",   theme.keyword_color_html },

        { "case",     theme.keyword_color_html },
        { "default",  theme.keyword_color_html },

        { "try",      theme.keyword_color_html },
        { "catch",    theme.keyword_color_html },

        { "break",    theme.keyword_color_html },
        { "return",   theme.keyword_color_html },
        { "continue", theme.keyword_color_html },
        { "goto",     theme.keyword_color_html },

        { "and",      theme.keyword_color_html },
        { "and_eq",   theme.keyword_color_html },
        { "bitand",   theme.keyword_color_html },
        { "bitor",    theme.keyword_color_html },
        { "compl",    theme.keyword_color_html },
        { "not",      theme.keyword_color_html },
        { "not_eq",   theme.keyword_color_html },
        { "or",       theme.keyword_color_html },
        { "or_eq",    theme.keyword_color_html },
        { "xor",      theme.keyword_color_html },
        { "xor_eq",   theme.keyword_color_html }

    };

}

std::string cpp_keywords::color(const std::string & token) const {

    try {

        return color_map.at(token);

    } catch(const std::out_of_range & e) {

        return "";

    }

}

