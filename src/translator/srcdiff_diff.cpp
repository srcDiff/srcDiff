#include <srcdiff_diff.hpp>

#include <srcdiff_many.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_common.hpp>
#include <srcdiff_move.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_match.hpp>
#include <shortest_edit_script.hpp>

#include <cstring>
#include <methods.hpp>

srcdiff_diff::srcdiff_diff(srcdiff_output & out, const node_sets & node_sets_old, const node_sets & node_sets_new) 
  : out(out), node_sets_old(node_sets_old), node_sets_new(node_sets_new) {}

/*

  Outputs diff on each level.  First, Common areas as well as inserts and deletes
  are output directly.  One-to-one matches result in recursion on syntax match and
  direct output otherwise.  Many-to-many decides to treat parts or all of the
  change as multiple one-one or deletions followed by insert.

  Whitespace is dis-regarded during the entire process, and is incorporated on
  output before and after changes/common sections.

*/
void srcdiff_diff::output() {

  //fprintf(stderr, "HERE_DOUBLE\n");

  diff_nodes dnodes = { out.get_nodes_old(), out.get_nodes_new() };

  class shortest_edit_script ses(srcdiff_compare::node_set_syntax_compare, srcdiff_compare::node_set_index, &dnodes);

  int distance = ses.compute((const void *)&node_sets_old, node_sets_old.size(), (const void *)&node_sets_new, node_sets_new.size());

  edit * edit_script = ses.get_script();

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  srcdiff_move::mark_moves(out.get_nodes_old(), node_sets_old, out.get_nodes_new(), node_sets_new, edit_script);

  int last_diff_old = 0;
  int last_diff_new = 0;
  int diff_end_old = out.last_output_old();
  int diff_end_new = out.last_output_new();

  edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_old = out.last_output_old();
    diff_end_new = out.last_output_new();

    if(edits->operation == SESDELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old.at(edits->offset_sequence_one - 1).back() + 1;
      diff_end_new = node_sets_new.at(edits->offset_sequence_two - 1).back() + 1;

    } else if(edits->operation == SESINSERT && edits->offset_sequence_one != 0 && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old.at(edits->offset_sequence_one - 1).back() + 1;
      diff_end_new = node_sets_new.at(edits->offset_sequence_two - 1).back() + 1;

    }

    // output area in common
    output_common(diff_end_old, diff_end_new);
    // detect and change
    edit * edit_next = edits->next;

    if(is_change(edits)) {

      //      fprintf(stderr, "HERE\n");

      // many to many handling
      srcdiff_many diff(*this, edits);
      diff.output();

      // update for common
      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case SESINSERT:

        //fprintf(stderr, "HERE\n");
        output_pure(0, node_sets_new.at(edits->offset_sequence_two + edits->length - 1).back() + 1);


        // update for common
        last_diff_old = edits->offset_sequence_one;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case SESDELETE:

        //fprintf(stderr, "HERE\n");
        output_pure(node_sets_old.at(edits->offset_sequence_one + edits->length - 1).back() + 1, 0);

        // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two;

        break;
      }
      
    }

  }

  // determine ending position to output
  diff_end_old = out.last_output_old();
  diff_end_new = out.last_output_new();
  if(last_diff_old < (signed)node_sets_old.size()) {

    diff_end_old = node_sets_old.back().back() + 1;
    diff_end_new = node_sets_new.back().back() + 1;

  }

  // output area in common
  output_common(diff_end_old, diff_end_new);

}

void srcdiff_diff::output_common(int end_old, int end_new) {

  srcdiff_common common(out, end_old, end_new);
  common.output();

}

void srcdiff_diff::output_pure(int end_old, int end_new) {

  srcdiff_change pure(out, end_old, end_new);
  pure.output_whitespace();
  pure.output();

}

void srcdiff_diff::output_change(int end_old, int end_new) {

  srcdiff_change change(out, end_old, end_new);
  change.output();

}

void srcdiff_diff::output_change_whitespace(int end_old, int end_new) {

  srcdiff_change change(out, end_old, end_new);
  change.output_whitespace();
  change.output();

}
