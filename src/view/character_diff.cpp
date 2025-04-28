// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file character_diff.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <character_diff.hpp>

#include <view.hpp>
#include <shortest_edit_script_utility.hpp>

character_diff::character_diff(const versioned_string & str)
    : str(str) {}


void character_diff::output(view_t & view, const std::string & type) {

    ses::edit_list edits = srcdiff::shortest_edit_script_utility::compute(str.original(), str.modified());

    int difference = 0;
    int num_consecutive_edits = 0;
    for(const struct ses::edit& edit : edits) {

      num_consecutive_edits += 1;
      difference += edit.original_length + edit.modified_length;

    }

    int min_size = std::min(str.original().size(), str.modified().size());
    bool is_diff_name = type == "name" && 4 * difference < min_size;
    bool is_diff_operator = type == "operator" && difference <= min_size;

    if(is_diff_name || is_diff_operator || num_consecutive_edits == 1) {

        std::size_t last_diff_original = 0;
        std::size_t last_diff_modified = 0;
        for(const struct ses::edit& edit : edits) {

            if((edit.operation == ses::DELETE || edit.original_offset == ses::CHANGE)
               && last_diff_original < edit.original_offset) {
                view.output_characters(str.original().substr(last_diff_original, edit.original_offset),
                                     view_t::COMMON);
            } else if(edit.operation == ses::INSERT && last_diff_modified < edit.modified_offset) {
                view.output_characters(str.original().substr(last_diff_modified, edit.modified_offset),
                                     view_t::COMMON);
            }

            last_diff_original = edit.original_offset + edit.original_length;
            last_diff_modified = edit.modified_offset + edit.original_length;

            // handle pure delete or insert
            switch (edit.operation) {

                case ses::CHANGE:
                  view.output_characters(str.original().substr(edit.original_offset, edit.original_length),     view_t::DELETE);
                  view.output_characters(str.modified().substr(edit.modified_offset, edit.modified_length), view_t::INSERT);

                  last_diff_original = edit.original_offset + edit.original_length;
                  last_diff_modified = edit.modified_offset + edit.modified_length;

                  break;

                case ses::INSERT:
                  view.output_characters(str.modified().substr(edit.modified_offset, edit.modified_length), view_t::INSERT);

                  // update for common
                  last_diff_original = edit.original_offset;
                  last_diff_modified = edit.modified_offset + edit.modified_length;

                  break;

                case ses::DELETE:
                  view.output_characters(str.original().substr(edit.original_offset, edit.original_length), view_t::DELETE);

                  // update for common
                  last_diff_original = edit.original_offset + edit.original_length;
                  last_diff_modified = edit.modified_offset;

                  break;

                default:
                  break;


                }

          }

      if(last_diff_original < str.original().size()) {
        view.output_characters(str.original().substr(last_diff_original), view_t::COMMON);
      }


    } else {

      view.output_characters(str.original(), view_t::DELETE);
      view.output_characters(str.modified(), view_t::INSERT);

    }

}
