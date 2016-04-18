/**
 *  @file default_theme.cpp
 *
 *  Specifies monokai color scheme.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <default_theme.hpp>

default_theme::default_theme(bool is_html) : theme_t(is_html) {

    if(is_html) {

        background_color  = "transparent";
        text_color        = "black";
        line_number_color = "grey";

        common_color = "<span style=\"background-color: transparent;\">";
        delete_color = "<span style=\"color:grey; text-decoration: line-through;\"><span style=\"color: black; background-color: rgb(255,187,187); font-weight: bold;\">";
        insert_color = "<span style=\"background-color: rgb(0 , 250, 108); font-weight: bold;\">";

        keyword_color = "<span style=\"color: rgb(249, 38, 114);\">";
        storage_color = "<span style=\"color: rgb(166, 226, 46);\">";
        type_color    = "<span style=\"color: rgb(62, 177, 1999);\">";

        comment_color = "<span style=\"color: rgb(117, 113, 94);\">";
        number_color  = "<span style=\"color: rgb(174, 129, 255);\">";
        string_color  = "<span style=\"color: rgb(205, 145, 0);\">";

        function_name_color = "<span style=\"color: rgb(166, 226, 46);\">";
        class_name_color    = "<span style=\"color: rgb(166, 226, 46);\">";
        call_name_color     = "<span style=\"color: rgb(102, 217, 239);\">";

    } else {

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

    keywords = cpp_keywords(*this);

}
