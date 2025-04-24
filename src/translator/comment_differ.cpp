// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file comment_differ.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <comment_differ.hpp>

#include <shortest_edit_script_utility.hpp>

#include <common_stream.hpp>
#include <operation.hpp>

namespace srcdiff {

comment_differ::comment_differ(std::shared_ptr<srcdiff::output_stream> out, const construct::construct_list & original, const construct::construct_list & modified) 
  : srcdiff::differ(out, original, modified) {}

/*

  Outputs differences in a comment on a word bases.

  Whitespace is not included with changes, but marked up
  where in common.

*/
void comment_differ::output() {

  ses::edit_list edits = shortest_edit_script_utility::compute(original, modified);

  std::size_t last_diff_original = 0;
  std::size_t diff_end_original = out->last_output_original();
  std::size_t diff_end_modified = out->last_output_modified();

  for(const struct ses::edit& edit : edits) {

    // determine ending position to output
    diff_end_original = out->last_output_original();
    diff_end_modified = out->last_output_modified();
    if((edit.operation == ses::DELETE || edit.operation == ses::CHANGE) && last_diff_original < edit.original_offset) {

      diff_end_original = original[edit.original_offset - 1]->end_position() + 1;
      diff_end_modified = modified[edit.modified_offset - 1]->end_position() + 1;

    } else if(edit.operation == ses::INSERT && edit.original_offset != 0 && last_diff_original <= edit.original_offset)  {

      diff_end_original = original[edit.original_offset - 1]->end_position() + 1;
      diff_end_modified = modified[edit.modified_offset - 1]->end_position() + 1;

    }

    // output area in common
    srcdiff::common_stream::output_common(out, diff_end_original, diff_end_modified);

    // handle pure delete or insert
    switch (edit.operation) {

      case ses::CHANGE: {

        // TODO: simplify unless plan to handle many to many different // 1-1
        if(edit.original_length == edit.modified_length && edit.original_length == 1
           && (original[edit.original_offset]->size() > 1
               || original[edit.original_offset]->size() > 1)) {

          output_change_whitespace(original[edit.original_offset]->end_position() + 1, modified[edit.modified_offset]->end_position() + 1);

        } else {

          // many to many
          output_change_whitespace(original[edit.original_offset + edit.original_length - 1]->end_position() + 1,
                                   modified[edit.modified_offset + edit.modified_length - 1]->end_position() + 1);

        }
        break;

        // update for common
        last_diff_original = edit.original_offset + edit.original_length;
      }

      case ses::INSERT:

        output_pure(0, modified[edit.modified_offset + edit.modified_length - 1]->end_position() + 1);

        // update for common
        last_diff_original = edit.original_offset;

        break;

      case ses::DELETE:

        output_pure(original[edit.original_offset + edit.original_length - 1]->end_position() + 1, 0);

        // update for common
        last_diff_original = edit.original_offset + edit.original_length;

        break;

      default:
        break;

      }

  }

  // determine ending position to output
  diff_end_original = out->last_output_original();
  diff_end_modified = out->last_output_modified();
  if(last_diff_original < original.size()) {

    diff_end_original = original.back()->end_position() + 1;
    diff_end_modified = modified.back()->end_position() + 1;

  }

  // output area in common
  srcdiff::common_stream::output_common(out, diff_end_original, diff_end_modified);

}

}
