/**
 *  @file user_defined_theme.hpp
 *
 *  Specifies user defined color scheme.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_USER_DEFINED_THEME_HPP
#define INCLUDED_USER_DEFINED_THEME_HPP

#include <theme.hpp>

#include <string>
#include <iostream>

class user_defined_theme : public theme_t {

private:

public:

    user_defined_theme(const std::string & highlight_level,
    				   bool is_html,
    				   const std::string & theme_filename);

};

#endif
