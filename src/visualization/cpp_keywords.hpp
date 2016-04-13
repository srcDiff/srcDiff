/**
 *  @file cpp_keywords.hpp
 *
 *  Specifies cpp_keywords color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_CPP_KEYWORDS_HPP
#define INCLUDED_CPP_KEYWORDS_HPP

#include <unordered_map>
#include <string>

class monokai;

class cpp_keywords {

    std::unordered_map<std::string, std::string>  color_map;

public:

    cpp_keywords() {}
    cpp_keywords(const monokai & theme);

    std::string color(const std::string & token) const;

};

#endif
