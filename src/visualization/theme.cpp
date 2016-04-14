/**
 *  @file monokai.hpp
 *
 *  Specifies color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <theme.hpp>

theme_t::theme_t(bool is_html) {}

std::string theme_t::token2color(const std::string & token, const std::string & parent) const {

    try {

        return keywords.color(token);

    } catch(const std::out_of_range & e) {

        if(parent == "comment")        return comment_color;
        if(parent == "literal")        return number_color;
        if(parent == "literal_string") return string_color;

    }

    return "";

}
