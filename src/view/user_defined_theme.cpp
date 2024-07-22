// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file user_defined_theme.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <user_defined_theme.hpp>

#include <CLI/CLI.hpp>

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <array>

class color_t {

private:
    int red;
    int green;
    int blue;
    int alpha;

public:

    color_t()
        : red(0xff),
          green(0xff),
          blue(0xff),
          alpha(0xff) {}

    color_t(const std::string hex_string) : alpha(255) {

        if((hex_string.size() != 6 && hex_string.size() != 8)
          || !(std::isdigit(hex_string[0])
                || (hex_string[0] >= 'a' && hex_string[0] <='f')
                || (hex_string[0] >= 'A' && hex_string[0] <='F')))
            throw CLI::ValidationError("invalid value for color '" + hex_string + "', must be hex: e.g., xxxxxx");

        red = std::stoi(hex_string.substr(0, 2), 0, 16);
        green = std::stoi(hex_string.substr(2, 2), 0, 16);
        blue = std::stoi(hex_string.substr(4, 2), 0, 16);

        if(hex_string.size() == 8)
            alpha = std::stoi(hex_string.substr(6, 2), 0, 16);


    }

    std::string to_html() const {

        if(alpha == 0)
            return "transparent";

        return std::string("rgba(") + std::to_string(red) + ", "
            + std::to_string(green) + ", "
            + std::to_string(blue) + ", "
            + std::to_string(alpha) + ')';

    }

    std::string to_ansi() const {

        int ansi_color = 16 + 36 * (red / 43) + 6 * (green / 43) + (blue / 43);

        return std::to_string(ansi_color);

    }

    friend std::ostream & operator<<(std::ostream & out, const color_t & color) {

        out << '#' << std::hex << color.red << color.green << color.    blue;

        return out;

    }

    friend std::istream & operator>>(std::istream & in, color_t & color) {

        std::string hex_string;
        in >> hex_string;
        // throws a warning about a deprecated copy
        color = color_t(hex_string);

        return in;

    }

};

std::map<std::string, color_t> parse_user_definition_file(const std::string & theme_filename) {

    // Using CLI11 to parse the config file instead of installing another
    // parser. it supports TOML and INI formats (which are identical for our
    // purposes apart from comments, since we just have strings as option
    // values)

    CLI::App custom_theme("Theme options");
    // set up one command-line-style option, which is the name of the file that
    // will contain our actual options
    custom_theme.set_config("config", "", "", true);

    std::map<std::string, color_t> colors;
    
    std::array option_names = {
        "text-color", "line-number-color", "common-color", "delete-color", "insert-color",
        "keyword-color", "storage-color", "type-color", "comment-color", "number-color",
        "string-color", "function-name-color", "class-name-color", "call-name-color"
    };

    for (const auto & option_name : option_names) {
        custom_theme.add_option<color_t>(option_name, colors[option_name]);
    }

    try {
        // pass the theme's filename as an argument to CLI11 as if it were the
        // only thing received on the command line. this is probably not an
        // intended way to tell CLI11 to read a config file, but it works
        std::string args = "\""+theme_filename+"\"";
        custom_theme.parse(args);
    } catch (const CLI::ParseError &e) {
        std::cerr << "Error using " << theme_filename << " as a theme. Details:" << std::endl;
        exit(custom_theme.exit(e));
    }

    return colors;
}

user_defined_theme::user_defined_theme(const std::string & highlight_level,
                                       bool is_html,
                                       const std::string & theme_filename)
    : theme_t(highlight_level, is_html) {

    std::map<std::string, color_t> vm = parse_user_definition_file(theme_filename);

    if(is_html) {

        background_color  = vm["common-color"].to_html();
        text_color        = vm["text-color"].to_html();
        line_number_color = vm["line-number-color"].to_html();

        common_color = "<span style=\"background-color: " + vm["common-color"].to_html() + ";\">";
        delete_color = "<span style=\"color: rgb(160, 160, 160); text-decoration: line-through;\"><span style=\"color: black; background-color: " + vm["delete-color"].to_html() + "; font-weight: bold;\">";
        insert_color = "<span style=\"background-color: " + vm["insert-color"].to_html() + "; font-weight: bold;\">";

        keyword_color = "<span style=\"color: " + vm["keyword-color"].to_html() + ";\">";
        storage_color = "<span style=\"color: " + vm["storage-color"].to_html() + ";\">";
        type_color    = "<span style=\"color: " + vm["type-color"].to_html() + ";\">";

        comment_color = "<span style=\"color: " + vm["comment-color"].to_html() + ";\">";
        number_color  = "<span style=\"color: " + vm["number-color"].to_html() + ";\">";
        string_color  = "<span style=\"color: " + vm["string-color"].to_html() + ";\">";

        function_name_color = "<span style=\"color: " + vm["function-name-color"].to_html() + ";\">";
        class_name_color    = "<span style=\"color: " + vm["class-name-color"].to_html() + ";\">";
        call_name_color     = "<span style=\"color: " + vm["call-name-color"].to_html() + ";\">";

    } else {

        background_color  = "\x1b[38;5;" + vm["common-color"].to_ansi() + 'm';
        text_color        = "\x1b[38;5;" + vm["text-color"].to_ansi() + 'm';
        line_number_color = "\x1b[38;5;" + vm["line-number-color"].to_ansi() + 'm';

        common_color = "\x1b[0m\x1b[48;5;" + vm["common-color"].to_ansi() + 'm' + text_color;
        delete_color = "\x1b[9;48;5;" + vm["delete-color"].to_ansi() + ";1m";
        insert_color = "\x1b[48;5;" + vm["insert-color"].to_ansi() + ";1m";

        keyword_color = "\x1b[38;5;" + vm["keyword-color"].to_ansi() + 'm';
        storage_color = "\x1b[38;5;" + vm["storage-color"].to_ansi() + 'm';
        type_color    = "\x1b[38;5;" + vm["type-color"].to_ansi() + 'm';

        comment_color = "\x1b[38;5;" + vm["comment-color"].to_ansi() + 'm';
        number_color  = "\x1b[38;5;" + vm["number-color"].to_ansi() + 'm';
        string_color  = "\x1b[38;5;" + vm["string-color"].to_ansi() + 'm';

        function_name_color = "\x1b[38;5;" + vm["function-name-color"].to_ansi() + 'm';
        class_name_color    = "\x1b[38;5;" + vm["class-name-color"].to_ansi() + 'm';
        call_name_color     = "\x1b[38;5;" + vm["call-name-color"].to_ansi() + 'm';

    }

}
