/**
 *  @file monokai.hpp
 *
 *  Specifies monokai color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <monokai_theme.hpp>

monokai_theme::monokai_theme() {

    background_color = "rgb(39 , 40, 34)";
    text_color       = "rgb(248 , 248, 242)";

    common_color_html = "<span style=\"background-color: transparent\">";
    delete_color_html = "<span style=\"color:grey; text-decoration: line-through;\"><span style=\"color: " + text_color + "; background-color: rgb(255, 94, 94); font-weight: bold;\">";
    insert_color_html = "<span style=\"background-color: rgb(0 , 187, 81)  ; font-weight: bold;\">";
    // delete_color_html = "<span style=\"color:grey; text-decoration: line-through;\"><span style=\"color: black; background-color: rgb(255,187,187); font-weight: bold;\">";
    // insert_color_html = "<span style=\"background-color: rgb(0 , 250, 108)  ; font-weight: bold;\">";

    keyword_color_html = "<span style=\"color: rgb(249, 38, 114);\">";
    storage_color_html = "<span style=\"color: rgb(166, 226, 46);\">";
    type_color_html    = "<span style=\"color: rgb(102, 217, 239);\">";

    comment_color_html = "<span style=\"color: rgb(117, 113, 94);\">";
    string_color_html  = "<span style=\"color: rgb(230, 219, 116);\">";
    number_color_html  = "<span style=\"color: rgb(174, 129, 255);\">";
    keywords = cpp_keywords(*this);

}

std::string monokai_theme::token2color(const std::string & token, const std::string & parent) const {

    try {

        return keywords.color(token);

    } catch(const std::out_of_range & e) {

        if(parent == "comment")        return comment_color_html;
        if(parent == "literal")        return number_color_html;
        if(parent == "literal_string") return string_color_html;

    }

    return "";

}
