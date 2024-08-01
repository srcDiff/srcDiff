// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_correction.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_EDIT_CORRECTION_HPP
#define INCLUDED_SRCDIFF_EDIT_CORRECTION_HPP

#include <srcdiff_shortest_edit_script.hpp>
#include <construct.hpp>
#include <srcdiff_text_measure.hpp>
#include <memory>

class srcdiff_edit_correction {

private:
    const construct::construct_list_view sets_original;
    const construct::construct_list_view sets_modified;
    srcdiff_shortest_edit_script & ses;

private:
    void split_change(edit_t * delete_edit, edit_t * insert_edit,
                        int original_pos, int modified_pos,
                        edit_t *& start_edits,
                        edit_t *& last_edits);
    edit_t * correct_common_inner(edit_t * change_edit);
    edit_t * correct_common(edit_t * start_edit);
    std::shared_ptr<srcdiff_text_measure> edit2measure(int original_offset, int modified_offset);

public:
    srcdiff_edit_correction(const construct::construct_list_view sets_original,
                            const construct::construct_list_view sets_modified,
                            srcdiff_shortest_edit_script & ses);
    void correct();

};


#endif
