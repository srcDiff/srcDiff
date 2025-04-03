// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file edit_corrector.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_EDIT_CORRECTOR_HPP
#define INCLUDED_EDIT_CORRECTOR_HPP

#include <shortest_edit_script.hpp>
#include <construct.hpp>
#include <text_measurer.hpp>
#include <memory>

namespace srcdiff {

class edit_corrector {

private:
    const construct::construct_list_view sets_original;
    const construct::construct_list_view sets_modified;
    edit_t*& edits;

private:
    void split_change(edit_t * delete_edit, edit_t * insert_edit,
                        int original_pos, int modified_pos,
                        edit_t *& start_edits,
                        edit_t *& last_edits);
    edit_t * correct_common_inner(edit_t * change_edit);
    edit_t * correct_common(edit_t * start_edit);
    std::shared_ptr<srcdiff::text_measurer> edit2measure(int original_offset, int modified_offset);

public:
    edit_corrector(const construct::construct_list_view sets_original,
                   const construct::construct_list_view sets_modified,
                   edit_t*& edits);
    void correct();

};

}

#endif
