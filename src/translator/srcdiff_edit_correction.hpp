/**
 *  @file srcdiff_edit_corretion.cpp
 *
 *  This is a generic C++ main file.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#ifndef INCLUDED_SRCDIFF_EDIT_CORRECTION_HPP
#define INCLUDED_SRCDIFF_EDIT_CORRECTION_HPP

#include <shortest_edit_script.hpp>

class srcdiff_edit_correction {

private:
	edit * edits;

public:
	srcdiff_edit_correction(edit * edit_script);
	void correct();

};


#endif