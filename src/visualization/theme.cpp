/**
 *  @file theme.hpp
 *
 *  Specifies color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <theme.hpp>

#include <cpp_keywords.hpp>

theme_t::theme_t(bool is_html) : keywords(nullptr) {}
theme_t::~theme_t() {

    if(keywords) delete keywords;

}

void theme_t::set_langauge(const std::string & language) {

    if(language == "C++") keywords = new cpp_keywords(*this);
    // if(language == "Java") keywords = new jvaa_keywords(*this);

}

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

        return keywords->color(token);

    } catch(const std::out_of_range & e) {}

    return "";

}
