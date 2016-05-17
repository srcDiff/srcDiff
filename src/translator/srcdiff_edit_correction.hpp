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
	class shortest_edit_script & ses;

private:
	void split_change(edit * delete_edit, edit * insert_edit,
	                    int original_pos, int modified_pos,
	                    edit *& start_edits,
	                    edit *& last_edits);
	edit * correct_common_inner(edit * change_edit);
	edit * correct_common(edit * start_edit);

public:
	srcdiff_edit_correction(const node_sets & sets_original,
							const node_sets & sets_modified,
							class shortest_edit_script & ses);
	void correct();

};


#endif