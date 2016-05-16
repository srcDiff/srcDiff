/**
 *  @file user_defined_theme.cpp
 *
 *  Specifies monokai color scheme.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <user_defined_theme.hpp>

void parse_user_definition_file(const std::string & theme_file) {


}

user_defined_theme::user_defined_theme(const std::string & highlight_level, bool is_html)
    : theme_t(highlight_level, is_html) {

    if(is_html) {

        background_color  = "transparent";
        text_color        = "black";
        line_number_color = "grey";

        common_color = "<span style=\"background-color: transparent;\">";
        delete_color = "<span style=\"color: rgb(160, 160, 160); text-decoration: line-through;\"><span style=\"color: black; background-color: rgb(255,187,187); font-weight: bold;\">";
        insert_color = "<span style=\"background-color: rgb(0 , 250, 108); font-weight: bold;\">";

        keyword_color = "<span style=\"color: rgb(249, 38, 114);\">";
        storage_color = "<span style=\"color: rgb(249, 38, 114);\">";
        type_color    = "<span style=\"color: rgb(50, 150, 225);\">";

        comment_color = "<span style=\"color: rgb(110, 110, 110);\">";
        number_color  = "<span style=\"color: rgb(255, 0, 255);\">";
        string_color  = "<span style=\"color: rgb(255, 128, 0);\">";

        function_name_color = "<span style=\"color: rgb(0, 160, 64);\">";
        class_name_color    = "<span style=\"color: rgb(0, 160, 64);\">";
        call_name_color     = "<span style=\"color: rgb(50, 140, 215);\">";

    } else {

        line_number_color = "\x1b[36m";

        common_color = "\x1b[0m";
        delete_color = "\x1b[9;48;5;217;1m";
        insert_color = "\x1b[48;5;120;1m";

        keyword_color = "\x1b[38;5;198m";
        storage_color = "\x1b[38;5;198m";
        type_color    = "\x1b[38;5;68m";

        comment_color = "\x1b[38;5;102m";
        number_color  = "\x1b[38;5;201m";
        string_color  = "\x1b[38;5;208m";

        function_name_color = "\x1b[38;5;28m";
        class_name_color    = "\x1b[38;5;28m";
        call_name_color     = "\x1b[38;5;68m";

    }

}
