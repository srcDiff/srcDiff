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
#include <iostream>
#include <cpp_keywords.hpp>

class monokai {

public:
    std::string function_name_color_html;
    std::string class_name_color_html;
    std::string storage_type_color_html;
    std::string storage_color_html;
    std::string keyword_color_html;
    std::string constant_color_html;
    std::string number_color_html;
    std::string string_color_html;
    std::string comment_color_html;

private:
    cpp_keywords keywords;

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
        keywords = cpp_keywords(*this);

    }

    std::string token2color(const std::string & token) {

        try {

            return keywords.color(token);

        } catch(const std::out_of_range & e) {

            return "";

        }

    }

};

#endif
