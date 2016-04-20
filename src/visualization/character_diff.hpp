/**
 *  @file character_diff.hpp
 *
 *  This simple class to do a character diff and output to a stream.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_CHARACTER_DIFF_HPP
#define INCLUDED_CHARACTER_DIFF_HPP

#include <shortest_edit_script.hpp>

class bash_view;

class character_diff {

private:

    class shortest_edit_script ses;
    const std::string & original_str;
    const std::string & modified_str;

public:

    character_diff(const std::string & original, const std::string & modified);

    void compute();
    void output(bash_view & view, const std::string & type);

};


#endif