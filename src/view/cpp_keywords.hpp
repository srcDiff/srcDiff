/**
 *  @file cpp_keywords.hpp
 *
 *  Specifies cpp_keywords color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_CPP_KEYWORDS_HPP
#define INCLUDED_CPP_KEYWORDS_HPP

#include <keywords.hpp>

#include <unordered_map>
#include <string>

class theme_t;

class cpp_keywords : public keywords_t {

public:

    cpp_keywords(const theme_t & theme);

};

#endif
