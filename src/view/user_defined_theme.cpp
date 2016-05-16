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

class color_t {

private:
    int red;
    int green;
    int blue;

public:

    color_t()
        : red(0xff),
          green(0xff),
          blue(0xff) {}

    color_t(const std::string color_hex) {

        if(color_hex.size() != 7 || color_hex[0] != '#')
            throw boost::program_options::error("invalid value for color, must be hex #ffffff");

        red = std::stoi(color_hex.substr(1, 2), 0, 16);
        green = std::stoi(color_hex.substr(3, 2), 0, 16);
        blue = std::stoi(color_hex.substr(5, 2), 0, 16);

    }

    color_t(const color_t & that)
        : red(that.red),
          green(that.green),
          blue(that.blue) {}

    friend std::ostream & operator>>(std::ostream & out, const color_t & color) {

        out << '#' << std::hex << color.red << color.green << color.blue;

        return out;

    }

    friend std::istream & operator>>(std::istream & in, color_t & color) {

        return in;

    }

};

void parse_user_definition_file(const std::string & theme_file) {

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

    boost::program_options::store(boost::program_options::parse_config_file<char>(theme_file.c_str(), desc), vm);
    boost::program_options::notify(vm);

  } catch(const boost::program_options::error & e) {

    std::cerr << "Exception: " << e.what() << '\n';
    exit(1);

  }

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
