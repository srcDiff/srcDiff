/**
 *  @file java_keywords.hpp
 *
 *  Specifies java_keywords color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_JAVA_KEYWORDS_HPP
#define INCLUDED_JAVA_KEYWORDS_HPP

#include <keywords.hpp>

#include <unordered_map>
#include <string>

class theme_t;

class java_keywords : public keywords_t {

public:

    java_keywords(const theme_t & theme);

};

#endif
