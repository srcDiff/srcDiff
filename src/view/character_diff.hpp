/**
 *  @file character_diff.hpp
 *
 *  This simple class to do a character diff and output to a stream.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_CHARACTER_DIFF_HPP
#define INCLUDED_CHARACTER_DIFF_HPP

#include <srcdiff_shortest_edit_script.hpp>
#include <versioned_string.hpp>

class view_t;

class character_diff {

private:

    srcdiff_shortest_edit_script ses;
    const versioned_string & str;

public:

    character_diff(const versioned_string & original);

    void compute();
	void output(view_t & view, const std::string & type);

};


#endif
