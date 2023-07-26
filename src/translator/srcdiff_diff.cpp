#include <srcdiff_diff.hpp>

#include <srcdiff_many.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_single.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcdiff_common.hpp>
#include <srcdiff_move.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_match.hpp>
#include <shortest_edit_script.hpp>
#include <srcdiff_edit_correction.hpp>

#include <cstring>
#include <methods.hpp>

srcdiff_diff::srcdiff_diff(srcdiff_output & out, const element_list & element_list_original, const element_list & element_list_modified) 
  : out(out), element_list_original(element_list_original), element_list_modified(element_list_modified) {}

/*

  Outputs diff on each level.  First, Common areas as well as inserts and deletes
  are output directly.  One-to-one matches result in recursion on syntax match and
  direct output otherwise.  Many-to-many decides to treat parts or all of the
  change as multiple one-one or deletions followed by insert.

  Whitespace is dis-regarded during the entire process, and is incorporated on
  output before and after changes/common sections.

*/
void srcdiff_diff::output() {

  diff_nodes dnodes = { element_list_original.nodes(), element_list_modified.nodes() };

  shortest_edit_script_t ses(srcdiff_compare::element_syntax_compare, srcdiff_compare::element_array_index, &dnodes);

  /** O(CND) */
  int distance = ses.compute<element_list>(element_list_original, element_list_modified, false);
  if(ses.is_approximate()) out.approximate(true);

  srcdiff_edit_correction corrector(element_list_original, element_list_modified, ses);
  corrector.correct();

  edit_t * edit_script = ses.script();

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  /** O(CD^2) */
  srcdiff_move::mark_moves(out.nodes_original(),
                           element_list_original,
                           out.nodes_modified(),
                           element_list_modified,
                           edit_script);

  int last_diff_original = 0;
  int last_diff_modified = 0;
  int diff_end_original = out.last_output_original();
  int diff_end_modified = out.last_output_modified();

  edit_t * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_original = out.last_output_original();
    diff_end_modified = out.last_output_modified();

    if(edits->operation == SES_DELETE && last_diff_original < edits->offset_sequence_one) {

      diff_end_original = element_list_original.at(edits->offset_sequence_one - 1).end_position() + 1;
      diff_end_modified = element_list_modified.at(edits->offset_sequence_two - 1).end_position() + 1;

    } else if(edits->operation == SES_INSERT && last_diff_modified < edits->offset_sequence_two) {

      diff_end_original = element_list_original.at(edits->offset_sequence_one - 1).end_position() + 1;
      diff_end_modified = element_list_modified.at(edits->offset_sequence_two - 1).end_position() + 1;

    }

    // output area in common
    output_common(diff_end_original, diff_end_modified);

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

        case SES_COMMON:

          if((xmlReaderTypes)element_list_original.at(edits->offset_sequence_one).term(0)->type != XML_READER_TYPE_TEXT) {

            srcdiff_single diff(*this, edits->offset_sequence_one, edits->offset_sequence_two);
            diff.output();

          } else {

            // common text nodes
            output_common(element_list_original.at(edits->offset_sequence_one).end_position() + 1,
                          element_list_modified.at(edits->offset_sequence_two).end_position() + 1);

          }

        case SES_INSERT:

          output_pure(0, element_list_modified.at(edits->offset_sequence_two + edits->length - 1).end_position() + 1);


          // update for common
          last_diff_original = edits->offset_sequence_one;
          last_diff_modified = edits->offset_sequence_two + edits->length;

          break;

        case SES_DELETE:

          output_pure(element_list_original.at(edits->offset_sequence_one + edits->length - 1).end_position() + 1, 0);

          // update for common
          last_diff_original = edits->offset_sequence_one + edits->length;
          last_diff_modified = edits->offset_sequence_two;

          break;
      }
      
    }

  }

  // determine ending position to output
  diff_end_original = out.last_output_original();
  diff_end_modified = out.last_output_modified();
  if(last_diff_original < (signed)element_list_original.size()) {

    diff_end_original = element_list_original.back().end_position() + 1;
    diff_end_modified = element_list_modified.back().end_position() + 1;

  }

  // output area in common
  output_common(diff_end_original, diff_end_modified);

}

void srcdiff_diff::output_common(int end_original, int end_modified) {

  srcdiff_common common(out, end_original, end_modified);
  common.output();

}

void srcdiff_diff::output_pure(int end_original, int end_modified) {

  srcdiff_change pure(out, end_original, end_modified);
  pure.output_whitespace_prefix();
  pure.output();

}

void srcdiff_diff::output_change(int end_original, int end_modified) {

  srcdiff_change change(out, end_original, end_modified);
  change.output();

}

void srcdiff_diff::output_whitespace() {

  srcdiff_whitespace whitespace(out);
  whitespace.output_all();

}

void srcdiff_diff::output_change_whitespace(int end_original, int end_modified) {

  srcdiff_change change(out, end_original, end_modified);
  change.output_whitespace_prefix();
  change.output();

}

void srcdiff_diff::output_replace_inner_whitespace(int start_original, int end_original,
                                                   int start_modified, int end_modified,
                                                   int common_offset) {


  srcdiff_change change(out, end_original - 1, end_modified - 1);
  change.output_whitespace_all();

  out.output_node(out.diff_common_start, SES_COMMON);
  for(int i = 0; i < common_offset; ++i) {

    out.output_node(out.nodes_original().at(start_original + i), SES_COMMON);
    ++out.last_output_original();
    ++out.last_output_modified();

  }

  change.output_whitespace_prefix();
  change.output();

  output_common(end_original, end_modified);
  out.output_node(out.diff_common_end, SES_COMMON);

}
