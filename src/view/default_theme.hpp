/**
 *  @file default_theme.hpp
 *
 *  Specifies default color scheme.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_DEFAULT_THEME_HPP
#define INCLUDED_DEFAULT_THEME_HPP

#include <theme.hpp>

#include <string>
#include <iostream>

class default_theme : public theme_t {

private: 
public:

    default_theme(const std::string & highlight_level, bool is_html);

};

#endif
