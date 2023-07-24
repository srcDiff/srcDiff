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
#include <element_list.hpp>
#include <srcdiff_text_measure.hpp>
#include <memory>

class srcdiff_edit_correction {

private:
	const element_list & sets_original;
	const element_list & sets_modified;
	shortest_edit_script_t & ses;

private:
	void split_change(edit_t * delete_edit, edit_t * insert_edit,
	                    int original_pos, int modified_pos,
	                    edit_t *& start_edits,
	                    edit_t *& last_edits);
	edit_t * correct_common_inner(edit_t * change_edit);
	edit_t * correct_common(edit_t * start_edit);
	std::shared_ptr<srcdiff_text_measure> edit2measure(int original_offset, int modified_offset);

public:
	srcdiff_edit_correction(const element_list & sets_original,
							const element_list & sets_modified,
							shortest_edit_script_t & ses);
	void correct();

};


#endif
