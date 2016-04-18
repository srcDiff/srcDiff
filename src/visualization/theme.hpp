/**
 *  @file theme.hpp
 *
 *  Specifies color scheme.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_THEME_HPP
#define INCLUDED_THEME_HPP

#include <cpp_keywords.hpp>

#include <vector>
#include <string>
#include <iostream>

class theme_t {

public:

    std::string background_color;
    std::string text_color;
    std::string line_number_color;

    std::string common_color;
    std::string delete_color;
    std::string insert_color;

    std::string keyword_color;
    std::string storage_color;
    std::string type_color;
    std::string comment_color;
    std::string number_color;
    std::string string_color;

protected:

    cpp_keywords keywords;

public:

    theme_t(bool is_html);
    std::string token2color(const std::string & token, const std::vector<std::string> & element_stack) const;

};

#endif
