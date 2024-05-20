/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <srcdiff_diff.hpp>

#include <srcdiff_many.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_single.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcdiff_common.hpp>
#include <srcdiff_move.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_shortest_edit_script.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_edit_correction.hpp>

#include <cstring>
#include <methods.hpp>

srcdiff_diff::srcdiff_diff(std::shared_ptr<srcdiff_output> out, const construct::construct_list & construct_list_original, const construct::construct_list & construct_list_modified) 
  : out(out), construct_list_original(construct_list_original), construct_list_modified(construct_list_modified) {}

/*

  Outputs diff on each level.  First, Common areas as well as inserts and deletes
  are output directly.  One-to-one matches result in recursion on syntax match and
  direct output otherwise.  Many-to-many decides to treat parts or all of the
  change as multiple one-one or deletions followed by insert.

  Whitespace is dis-regarded during the entire process, and is incorporated on
  output before and after changes/common sections.

*/
void srcdiff_diff::output() {

  srcdiff_shortest_edit_script ses;

  /** O(CND) */
  int distance = ses.compute_edit_script(construct_list_original, construct_list_modified);
  if(ses.is_approximate()) out->approximate(true);

  srcdiff_edit_correction corrector(construct_list_original, construct_list_modified, ses);
  corrector.correct();

  edit_t * edit_script = ses.script();

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  /** O(CD^2) */
  srcdiff_move::mark_moves(construct_list_original, construct_list_modified, edit_script);

  std::size_t last_diff_original = 0;
  std::size_t last_diff_modified = 0;
  std::size_t diff_end_original = out->last_output_original();
  std::size_t diff_end_modified = out->last_output_modified();

  edit_t * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_original = out->last_output_original();
    diff_end_modified = out->last_output_modified();

    if(edits->operation == SES_DELETE && last_diff_original < edits->offset_sequence_one) {

      diff_end_original = construct_list_original.at(edits->offset_sequence_one - 1)->end_position() + 1;
      diff_end_modified = construct_list_modified.at(edits->offset_sequence_two - 1)->end_position() + 1;

    } else if(edits->operation == SES_INSERT && last_diff_modified < edits->offset_sequence_two) {

      diff_end_original = construct_list_original.at(edits->offset_sequence_one - 1)->end_position() + 1;
      diff_end_modified = construct_list_modified.at(edits->offset_sequence_two - 1)->end_position() + 1;

    }

    // output area in common
    srcdiff_common::output_common(out, diff_end_original, diff_end_modified);

    // detect and change
    edit_t * edit_next = edits->next;
    if(is_change(edits)) {

      // many to many handling
      /** loop O(RD^2) */
      srcdiff_many diff(*this, edits);
      diff.output();

      // update for common
      last_diff_original = edits->offset_sequence_one + edits->length;
      last_diff_modified = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

        case SES_COMMON: {

          if(construct_list_original.at(edits->offset_sequence_one)->term(0)->get_type() != srcML::node_type::TEXT) {

            srcdiff_single diff(out,
                                construct_list_original.at(edits->offset_sequence_one),
                                construct_list_modified.at(edits->offset_sequence_two));
            diff.output();

          } else {

            // common text nodes
            srcdiff_common::output_common(out, construct_list_original.at(edits->offset_sequence_one)->end_position() + 1,
                          construct_list_modified.at(edits->offset_sequence_two)->end_position() + 1);

          }

          break;
        }

        case SES_INSERT: {

          output_pure(0, construct_list_modified.at(edits->offset_sequence_two + edits->length - 1)->end_position() + 1);


          // update for common
          last_diff_original = edits->offset_sequence_one;
          last_diff_modified = edits->offset_sequence_two + edits->length;

          break;
        }

        case SES_DELETE: {

          output_pure(construct_list_original.at(edits->offset_sequence_one + edits->length - 1)->end_position() + 1, 0);

          // update for common
          last_diff_original = edits->offset_sequence_one + edits->length;
          last_diff_modified = edits->offset_sequence_two;

          break;
        }

      }
      
    }

  }

  // determine ending position to output
  diff_end_original = out->last_output_original();
  diff_end_modified = out->last_output_modified();
  if(last_diff_original < construct_list_original.size()) {

    diff_end_original = construct_list_original.back()->end_position() + 1;
    diff_end_modified = construct_list_modified.back()->end_position() + 1;

  }

  // output area in common
  srcdiff_common::output_common(out, diff_end_original, diff_end_modified);

}

void srcdiff_diff::output_pure(int end_original, int end_modified) {

  srcdiff_change pure(*out, end_original, end_modified);
  pure.output_whitespace_prefix();
  pure.output();

}


void srcdiff_diff::output_change_whitespace(int end_original, int end_modified) {

  srcdiff_change change(*out, end_original, end_modified);
  
  change.output_whitespace_prefix();
  
  change.output();
  

}

void srcdiff_diff::output_replace_inner_whitespace(int start_original, int end_original,
                                                   int start_modified [[maybe_unused]], int end_modified,
                                                   int common_offset) {


  srcdiff_change change(*out, end_original - 1, end_modified - 1);
  change.output_whitespace_all();

  out->output_node(out->diff_common_start, SES_COMMON);
  for(int i = 0; i < common_offset; ++i) {

    out->output_node(out->nodes_original().at(start_original + i), SES_COMMON);
    ++out->last_output_original();
    ++out->last_output_modified();

  }

  change.output_whitespace_prefix();
  change.output();

  srcdiff_common::output_common(out, end_original, end_modified);
  out->output_node(out->diff_common_end, SES_COMMON);

}
