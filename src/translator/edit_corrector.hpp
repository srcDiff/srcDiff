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

#include <shortest_edit_script_t.hpp>
#include <edit_list.hpp>

#include <construct.hpp>
#include <text_measurer.hpp>
#include <memory>

namespace srcdiff {

class edit_corrector {

private:
    const construct::construct_list_view sets_original;
    const construct::construct_list_view sets_modified;
    ses::edit_list& edits;

private:
    void split_change(struct ses::edit subject_edits,
                      int original_pos, int modified_pos,
                      ses::edit_iterator& start_edits,
                      ses::edit_iterator& last_edits);
    ses::edit_iterator correct_common_inner(ses::edit_iterator change_edit);
    ses::edit_iterator correct_common(ses::edit_iterator start_edit);
    std::shared_ptr<srcdiff::text_measurer> edit2measure(int original_offset, int modified_offset);

public:
    edit_corrector(const construct::construct_list_view sets_original,
                   const construct::construct_list_view sets_modified,
                   ses::edit_list& edits);
    void correct();

};

}

#endif
