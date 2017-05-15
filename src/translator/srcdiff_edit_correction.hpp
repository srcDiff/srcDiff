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
#include <srcdiff_text_measure.hpp>
#include <boost/optional.hpp>

class srcdiff_edit_correction {

private:
	const node_sets & sets_original;
	const node_sets & sets_modified;
	shortest_edit_script_t & ses;

private:
	void split_change(edit_t * delete_edit, edit_t * insert_edit,
	                    int original_pos, int modified_pos,
	                    edit_t *& start_edits,
	                    edit_t *& last_edits);
	edit_t * correct_common_inner(edit_t * change_edit);
	edit_t * correct_common(edit_t * start_edit);
	boost::optional<srcdiff_text_measure> edit2measure(int original_offset, int modified_offset);

public:
	srcdiff_edit_correction(const node_sets & sets_original,
							const node_sets & sets_modified,
							shortest_edit_script_t & ses);
	void correct();

};


#endif
