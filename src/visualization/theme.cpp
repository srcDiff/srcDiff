/**
 *  @file monokai.hpp
 *
 *  Specifies color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <theme.hpp>

theme_t::theme_t(bool is_html) {}

std::string theme_t::token2color(const std::string & token,
                                 bool in_comment,
                                 bool in_literal,
                                 bool in_string,
                                 bool in_function_name,
                                 bool in_class_name,
                                 bool in_call_name) const {
    
    if(in_comment)       return comment_color;
    if(in_literal)       return number_color;
    if(in_string)        return string_color;
    if(in_function_name) return function_name_color;
    if(in_class_name)    return class_name_color;
    if(in_call_name)     return call_name_color;

    try {

        return keywords.color(token);

    } catch(const std::out_of_range & e) {}

    return "";

}
