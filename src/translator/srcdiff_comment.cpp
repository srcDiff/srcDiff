#include <srcdiff_comment.hpp>

#include <srcdiff_common.hpp>

#include <srcdiff_compare.hpp>

#include <shortest_edit_script.h>
#include <ShortestEditScript.hpp>

srcdiff_comment::srcdiff_comment(srcdiff_output & out, node_sets * node_sets_old, node_sets * node_sets_new) 
  : srcdiff_diff(out, node_sets_old, node_sets_new) {}

/*

  Outputs differences in a comment on a word bases.

  Whitespace is not included with changes, but marked up
  where in common.

*/
void srcdiff_comment::output() {

  //fprintf(stderr, "HERE_DOUBLE\n");

  diff_nodes dnodes = { out.get_nodes_old(), out.get_nodes_new() };
  ShortestEditScript ses(srcdiff_compare::node_set_syntax_compare, srcdiff_compare::node_set_index, &dnodes);

  int distance = ses.compute((const void *)node_sets_old, node_sets_old->size(), (const void *)node_sets_new, node_sets_new->size());

  edit * edit_script = ses.get_script();

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

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

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(edits->offset_sequence_two - 1)->back() + 1;

    } else if(edits->operation == SESINSERT && edits->offset_sequence_one != 0 && last_diff_old <= edits->offset_sequence_one)  {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(edits->offset_sequence_two - 1)->back() + 1;

    }

    // output area in common
    output_common(diff_end_old, diff_end_new);

    // detect and change
    edit * edit_next = edits->next;
    if(is_change(edits)) {

      //      fprintf(stderr, "HERE\n");

      // TODO: simplify unless plan to handle many to many different // 1-1
      if(edits->length == edit_next->length && edits->length == 1
         && (node_sets_old->at(edits->offset_sequence_one)->size() > 1
             || node_sets_old->at(edits->offset_sequence_one)->size() > 1)) {

        output_change_whitespace(node_sets_old->at(edits->offset_sequence_one)->back() + 1, node_sets_new->at(edit_next->offset_sequence_two)->back() + 1);

      } else {

        // many to many
        output_change_whitespace(node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1,
         node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1);

      }

      // update for common
      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case SESINSERT:

        //fprintf(stderr, "HERE\n");
        output_pure(0, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1);

        // update for common
        last_diff_old = edits->offset_sequence_one;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case SESDELETE:

        //fprintf(stderr, "HERE\n");
        output_pure(node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1, 0);

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
  if(last_diff_old < (signed)node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }

  // output area in common
  output_common(diff_end_old, diff_end_new);

}



