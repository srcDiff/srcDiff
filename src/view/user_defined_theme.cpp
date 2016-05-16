/**
 *  @file user_defined_theme.cpp
 *
 *  Specifies monokai color scheme.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <user_defined_theme.hpp>

#include <boost/program_options.hpp>

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>

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

    color_t(const color_t & that)
        : red(that.red),
          green(that.green),
          blue(that.blue),
          alpha(that.alpha) {}

    color_t(const std::string hex_string) : alpha(255) {

        if((hex_string.size() != 6 && hex_string.size() != 8)
          || !(std::isdigit(hex_string[0])
                || (hex_string[0] >= 'a' && hex_string[0] <='f')
                || (hex_string[0] >= 'A' && hex_string[0] <='F')))
            throw boost::program_options::error("invalid value for color '" + hex_string + "', must be hex: e.g., xxxxxx");

        red = std::stoi(hex_string.substr(0, 2), 0, 16);
        green = std::stoi(hex_string.substr(2, 2), 0, 16);
        blue = std::stoi(hex_string.substr(4, 2), 0, 16);

        if(hex_string.size() == 8 )
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
        color = color_t(hex_string);

        return in;

    }

};

boost::program_options::variables_map parse_user_definition_file(const std::string & theme_filename) {

    std::ifstream theme_file(theme_filename.c_str());
    if(!theme_file)
        throw std::string("unable to open '" + theme_filename + '\'');

    // Declare the supported options.
    boost::program_options::options_description desc("Theme options");
    desc.add_options()
        ("background-color" ,   boost::program_options::value<color_t>(), "" )
        ("text-color"       ,   boost::program_options::value<color_t>(), "" )
        ("line-number-color",   boost::program_options::value<color_t>(), "" )

        ("common-color",        boost::program_options::value<color_t>(), "" )
        ("delete-color",        boost::program_options::value<color_t>(), "" )
        ("insert-color",        boost::program_options::value<color_t>(), "" )

        ("keyword-color",       boost::program_options::value<color_t>(), "" )
        ("storage-color",       boost::program_options::value<color_t>(), "" )
        ("type-color"   ,       boost::program_options::value<color_t>(), "" )

        ("comment-color",       boost::program_options::value<color_t>(), "" )
        ("number-color" ,       boost::program_options::value<color_t>(), "" )
        ("string-color" ,       boost::program_options::value<color_t>(), "" )

        ("function-name-color", boost::program_options::value<color_t>(), "" )
        ("class-name-color"   , boost::program_options::value<color_t>(), "" )
        ("call-name-color"    , boost::program_options::value<color_t>(), "" )
    ;

    boost::program_options::variables_map vm;

try {

    boost::program_options::store(boost::program_options::parse_config_file<char>(theme_file, desc), vm);
    boost::program_options::notify(vm);

  } catch(const boost::program_options::error & e) {

    throw std::string("Exception: ") + e.what();

  }

  return vm;

}

user_defined_theme::user_defined_theme(const std::string & highlight_level,
                                       bool is_html,
                                       const std::string & theme_filename)
    : theme_t(highlight_level, is_html) {

    boost::program_options::variables_map vm = parse_user_definition_file(theme_filename);

    if(is_html) {

        background_color  = vm["background-color"].as<color_t>().to_html();
        text_color        = vm["text-color"].as<color_t>().to_html();
        line_number_color = vm["line-number-color"].as<color_t>().to_html();

        common_color = "<span style=\"background-color: " + vm["common-color"].as<color_t>().to_html() + ";\">";
        delete_color = "<span style=\"color: rgb(160, 160, 160); text-decoration: line-through;\"><span style=\"color: black; background-color: " + vm["delete-color"].as<color_t>().to_html() + "; font-weight: bold;\">";
        insert_color = "<span style=\"background-color: " + vm["insert-color"].as<color_t>().to_html() + "; font-weight: bold;\">";

        keyword_color = "<span style=\"color: " + vm["keyword-color"].as<color_t>().to_html() + ";\">";
        storage_color = "<span style=\"color: " + vm["storage-color"].as<color_t>().to_html() + ";\">";
        type_color    = "<span style=\"color: " + vm["type-color"].as<color_t>().to_html() + ";\">";

        comment_color = "<span style=\"color: " + vm["comment-color"].as<color_t>().to_html() + ";\">";
        number_color  = "<span style=\"color: " + vm["number-color"].as<color_t>().to_html() + ";\">";
        string_color  = "<span style=\"color: " + vm["string-color"].as<color_t>().to_html() + ";\">";

        function_name_color = "<span style=\"color: " + vm["function-name-color"].as<color_t>().to_html() + ";\">";
        class_name_color    = "<span style=\"color: " + vm["class-name-color"].as<color_t>().to_html() + ";\">";
        call_name_color     = "<span style=\"color: " + vm["call-name-color"].as<color_t>().to_html() + ";\">";

    } else {

        line_number_color = "\x1b[" + vm["line-number-color"].as<color_t>().to_ansi() + 'm';

        common_color = "\x1b[38;5;" + vm["common-color"].as<color_t>().to_ansi() + 'm';
        delete_color = "\x1b[9;48;5;" + vm["delete-color"].as<color_t>().to_ansi() + ";1m";
        insert_color = "\x1b[48;5;" + vm["insert-color"].as<color_t>().to_ansi() + ";1m";

        keyword_color = "\x1b[38;5;" + vm["keyword-color"].as<color_t>().to_ansi() + 'm';
        storage_color = "\x1b[38;5;" + vm["storage-color"].as<color_t>().to_ansi() + 'm';
        type_color    = "\x1b[38;5;" + vm["type-color"].as<color_t>().to_ansi() + 'm';

        comment_color = "\x1b[38;5;" + vm["comment-color"].as<color_t>().to_ansi() + 'm';
        number_color  = "\x1b[38;5;" + vm["number-color"].as<color_t>().to_ansi() + 'm';
        string_color  = "\x1b[38;5;" + vm["string-color"].as<color_t>().to_ansi() + 'm';

        function_name_color = "\x1b[38;5;" + vm["function-name-color"].as<color_t>().to_ansi() + 'm';
        class_name_color    = "\x1b[38;5;" + vm["class-name-color"].as<color_t>().to_ansi() + 'm';
        call_name_color     = "\x1b[38;5;" + vm["call-name-color"].as<color_t>().to_ansi() + 'm';

    }

}
