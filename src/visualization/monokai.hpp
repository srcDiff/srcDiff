/**
 *  @file monokai.hpp
 *
 *  Specifies monokai color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_MONOKAI_HPP
#define INCLUDED_MONOKAI_HPP

#include <unordered_map>

class monokai {

    std::string function_name_color_html;
    std::string class_name_color_html;
    std::string storage_type_color_html;
    std::string storage_color_html;
    std::string keyword_color_html;
    std::string constant_color_html;
    std::string number_color_html;
    std::string string_color_html;
    std::string comment_color_html;

    std::unordered_map<std::string, std::string>  token2color_map = {

        { "if",       keyword_color_html },
        { "else",     keyword_color_html },
        { "while",    keyword_color_html },
        { "for",      keyword_color_html },
        { "do",       keyword_color_html },
        { "switch",   keyword_color_html },

        { "case",     keyword_color_html },
        { "default",  keyword_color_html },

        { "try",      keyword_color_html },
        { "catch",    keyword_color_html },

        { "break",    keyword_color_html },
        { "return",   keyword_color_html },
        { "continue", keyword_color_html },
        { "goto",     keyword_color_html }

    };

public:

    monokai() {

         keyword_color_html       = "<span style=\"color: rgb(249, 38, 114);\">";

         comment_color_html       = "<span style=\"color: rgb(117, 113, 94);\">";
         string_color_html        = "<span style=\"color: rgb(230, 219, 116);\">";
         number_color_html        = "<span style=\"color: rgb(174, 129, 255);\">";
         constant_color_html      = "<span style=\"color: rgb(174, 129, 255);\">";
         storage_color_html       = "<span style=\"color: rgb(249, 38, 114);\">";
         storage_type_color_html  = "<span style=\"color: rgb(102, 217, 239);\">";
         class_name_color_html    = "<span style=\"color: rgb(249, 38, 114);\">";
         function_name_color_html = "<span style=\"color: rgb(249, 38, 114);\">";

    }

    std::string token2color(const std::string & token) {


        try {

            return token2color_map.at(token);

        } catch(const std::out_of_range & e) {

            return "";

        }

    }

};

#endif
