/**
 *  @file monokai.hpp
 *
 *  Specifies color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <theme.hpp>

theme_t::theme_t(bool is_html) {

    if(!is_html) {

        line_number_color = "\x1b[36m";

        common_color = "\x1b[0m";
        delete_color = "\x1b[9;48;5;210;1m";
        insert_color = "\x1b[48;5;120;1m";

        keyword_color = "\x1b[38;5;204m";
        storage_color = "\x1b[38;5;204m";
        type_color    = "\x1b[38;5;117m";

        comment_color = "\x1b[38;5;102m";
        number_color  = "\x1b[38;5;141m";
        string_color  = "\x1b[38;5;222m";

    }

}

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
