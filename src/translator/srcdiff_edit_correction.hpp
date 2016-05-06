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
#include <node_sets.hpp>

class srcdiff_edit_correction {

private:
	const node_sets & sets_original;
	const node_sets & sets_modified;
	edit * edits;

public:
	srcdiff_edit_correction(const node_sets & sets_original,
							const node_sets & sets_modified,
							edit * edit_script);
	edit * correct();

};


#endif