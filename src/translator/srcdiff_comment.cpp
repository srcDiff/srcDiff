#include <srcdiff_comment.hpp>

#include <shortest_edit_script.h>
#include <ShortestEditScript.hpp>
#include <srcDiffUtility.hpp>
#include <srcDiffCommon.hpp>

srcdiff_comment::srcdiff_comment(reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate, node_sets * node_sets_old, node_sets * node_sets_new) 
  : srcdiff_diff(rbuf_old, rbuf_new, wstate, node_sets_old, node_sets_new) {}

/*

  Outputs differences in a comment on a word bases.

  Whitespace is not included with changes, but marked up
  where in common.

*/
void srcdiff_comment::output() {

  //fprintf(stderr, "HERE_DOUBLE\n");

  diff_nodes dnodes = { rbuf_old.nodes, rbuf_new.nodes };
  ShortestEditScript ses(node_set_syntax_compare, node_set_index, &dnodes);

  int distance = ses.compute((const void *)node_sets_old, node_sets_old->size(), (const void *)node_sets_new, node_sets_new->size());

  edit * edit_script = ses.get_script();

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_old = 0;
  int last_diff_new = 0;
  int diff_end_old = rbuf_old.last_output;
  int diff_end_new = rbuf_new.last_output;

  edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_old = rbuf_old.last_output;
    diff_end_new = rbuf_new.last_output;
    if(edits->operation == SESDELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(edits->offset_sequence_two - 1)->back() + 1;

    } else if(edits->operation == SESINSERT && edits->offset_sequence_one != 0 && last_diff_old <= edits->offset_sequence_one)  {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(edits->offset_sequence_two - 1)->back() + 1;

    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

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
  diff_end_old = rbuf_old.last_output;
  diff_end_new = rbuf_new.last_output;
  if(last_diff_old < (signed)node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }

  // output area in common
  output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

}



