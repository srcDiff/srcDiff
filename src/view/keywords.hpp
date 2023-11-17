/**
 *  @file keywords.hpp
 *
 *  Specifies cpp_keywords color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_KEYWORDS_HPP
#define INCLUDED_KEYWORDS_HPP

#include <unordered_map>
#include <string>

class theme_t;

class keywords_t {

protected:
    std::unordered_map<std::string, std::string> color_map;

public:

    keywords_t(const theme_t & theme);

    std::string color(const std::string & token) const;

};

#endif
