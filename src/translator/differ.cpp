// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file differ.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <differ.hpp>

#include <match_differ.hpp>
#include <many_differ.hpp>
#include <change_stream.hpp>
#include <common_stream.hpp>
#include <move_detector.hpp>
#include <measurer.hpp>
#include <operation.hpp>

#include <shortest_edit_script.hpp>
#include <edit_corrector.hpp>

#include <cstring>
#include <methods.hpp>

namespace srcdiff {

differ::differ(std::shared_ptr<output_stream> out, const construct::construct_list_view original, const construct::construct_list_view modified) 
  : out(out), original(original), modified(modified) {}

/*

  Outputs diff on each level.  First, Common areas as well as inserts and deletes
  are output directly.  One-to-one matches result in recursion on syntax match and
  direct output otherwise.  Many-to-many decides to treat parts or all of the
  change as multiple one-one or deletions followed by insert.

  Whitespace is dis-regarded during the entire process, and is incorporated on
  output before and after changes/common sections.

*/
void differ::output() {

  shortest_edit_script ses;

  /** O(CND) */
  ses::edit_list edits = ses.compute(original, modified);

  // edit_corrector corrector(original, modified, edits);
  // corrector.correct();

  /** O(CD^2) */
  move_detector::mark_moves(original, modified, edits);

  std::size_t last_diff_original = 0;
  std::size_t last_diff_modified = 0;
  std::size_t diff_end_original = out->last_output_original();
  std::size_t diff_end_modified = out->last_output_modified();

  for(const struct ses::edit& edit : edits) {

    // determine ending position to output
    diff_end_original = out->last_output_original();
    diff_end_modified = out->last_output_modified();

    if((edit.operation == ses::DELETE || edit.operation == ses::CHANGE) && last_diff_original < edit.original_offset) {

      diff_end_original = original[edit.original_offset - 1]->end_position() + 1;
      diff_end_modified = modified[edit.modified_offset - 1]->end_position() + 1;

    } else if(edit.operation == ses::INSERT && last_diff_modified < edit.modified_offset) {

      diff_end_original = original[edit.original_offset - 1]->end_position() + 1;
      diff_end_modified = modified[edit.modified_offset - 1]->end_position() + 1;

    }

    // output area in common
    common_stream::output_common(out, diff_end_original, diff_end_modified);

    // handle pure delete or insert
    switch (edit.operation) {

      case ses::CHANGE: {

        // many to many handling
        /** loop O(RD^2) */
        many_differ diff(*this, edit);
        diff.output();

        // update for common
        last_diff_original = edit.original_offset + edit.original_length;
        last_diff_modified = edit.modified_offset + edit.modified_length;

        break;
      }

      case ses::COMMON: {

        if(original[edit.original_offset]->term(0)->get_type() != srcML::node_type::TEXT) {

          match_differ diff(out, original[edit.original_offset], modified[edit.modified_offset]);
          diff.output();

        } else {

          // common text nodes
          common_stream::output_common(out, original[edit.original_offset]->end_position() + 1,
                                            modified[edit.modified_offset]->end_position() + 1);

        }

        break;
      }

      case ses::INSERT: {

        output_pure(0, modified[edit.modified_offset + edit.modified_length - 1]->end_position() + 1);


        // update for common
        last_diff_original = edit.original_offset;
        last_diff_modified = edit.modified_offset + edit.modified_length;

        break;
      }

      case ses::DELETE: {

        output_pure(original[edit.original_offset + edit.original_length - 1]->end_position() + 1, 0);

        // update for common
        last_diff_original = edit.original_offset + edit.original_length;
        last_diff_modified = edit.modified_offset;

        break;
      }

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
  common_stream::output_common(out, diff_end_original, diff_end_modified);

}

void differ::output_pure(int end_original, int end_modified) {

  srcdiff::change_stream pure(*out, end_original, end_modified);
  pure.output_whitespace_prefix();
  pure.output();

}


void differ::output_change_whitespace(int end_original, int end_modified) {

  srcdiff::change_stream change(*out, end_original, end_modified);
  
  change.output_whitespace_prefix();
  
  change.output();
  

}

void differ::output_replace_inner_whitespace(int start_original, int end_original,
                                                   int start_modified [[maybe_unused]], int end_modified,
                                                   int common_offset) {


  srcdiff::change_stream change(*out, end_original - 1, end_modified - 1);
  change.output_whitespace_all();

  out->output_node(out->diff_common_start, COMMON);
  for(int i = 0; i < common_offset; ++i) {

    out->output_node(out->nodes_original().at(start_original + i), COMMON);
    ++out->last_output_original();
    ++out->last_output_modified();

  }

  change.output_whitespace_prefix();
  change.output();

  common_stream::output_common(out, end_original, end_modified);
  out->output_node(out->diff_common_end, COMMON);

}

}
