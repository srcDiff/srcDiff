/**
 *  @file monokai_theme.hpp
 *
 *  Specifies monokai color scheme.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_MONOKAI_THEME_HPP
#define INCLUDED_MONOKAI_THEME_HPP

#include <theme.hpp>

#include <string>
#include <iostream>

class monokai_theme : public theme_t {

private: 
public:

    monokai_theme(const std::string & highlight_level, bool is_html);
    std::string token2color(const std::string & token, const std::string & parent) const;

};

#endif
