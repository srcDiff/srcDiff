#include <srcDiffCommentDiff.hpp>

#include <srcdiff_diff.hpp>

#include <shortest_edit_script.h>
#include <ShortestEditScript.hpp>
#include <srcDiffUtility.hpp>
#include <srcDiffCommon.hpp>
#include <srcDiffChange.hpp>
#include <srcdiff_diff.hpp>

void output_change_white_space(reader_state & rbuf_old, unsigned int end_old
                               , reader_state & rbuf_new, unsigned int end_new
                               , writer_state & wstate) {

  srcdiff_diff diff(rbuf_old, rbuf_new, wstate, 0, 0);
  diff.output_change_whitespace(end_old, end_new);


}

void output_pure_operation_white_space(reader_state & rbuf_old, unsigned int end_old
                                       , reader_state & rbuf_new, unsigned int end_new
                                       , int operation, writer_state & wstate) {

  srcdiff_diff diff(rbuf_old, rbuf_new, wstate, 0, 0);
  diff.output_pure(end_old, end_new);

}

/*

  Once a comment change is encountered, they are first matched based
  on paragraphs (separated by two new lines), then as needed, they are processes by line
  , and then by word using shortest edit script. Whitespace is included after/before changes

*/
void output_comment_paragraph(reader_state & rbuf_old, node_sets * node_sets_old, reader_state & rbuf_new, node_sets * node_sets_new, writer_state & wstate) {

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

      // 1-1
      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        node_sets next_set_old
          = node_sets(rbuf_old.nodes, node_sets_old->at(edits->offset_sequence_one)->at(0)
                                    , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1) + 1);

        node_sets next_set_new
          = node_sets(rbuf_new.nodes, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                                    , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1) + 1);

        // compare as lines
        output_comment_word(rbuf_old, &next_set_old, rbuf_new, &next_set_new, wstate);

      } else {

        // many to many
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                  , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1, wstate);
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
        output_pure_operation_white_space(rbuf_old, 0
                                          , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, SESINSERT, wstate);
        // update for common
        last_diff_old = edits->offset_sequence_one;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case SESDELETE:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                          , rbuf_new, 0, SESDELETE, wstate);

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

/*

  Breaks down paragraphs and runs on a line bases.
  Whitespace is included after/before changes

*/
void output_comment_line(reader_state & rbuf_old, node_sets * node_sets_old, reader_state & rbuf_new, node_sets * node_sets_new, writer_state & wstate) {

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

      // 1-1
      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        node_sets next_set_old
          = node_sets(rbuf_old.nodes, node_sets_old->at(edits->offset_sequence_one)->at(0)
                            , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1) + 1);

        node_sets next_set_new
          = node_sets(rbuf_new.nodes, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                            , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1) + 1);

        // compare on word basis
        output_comment_word(rbuf_old, &next_set_old, rbuf_new, &next_set_new, wstate);

      } else

        // many to many
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                  , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1, wstate);

      // update for common
      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case SESINSERT:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, 0
                                          , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, SESINSERT, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case SESDELETE:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                          , rbuf_new, 0, SESDELETE, wstate);

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

/*

  Outputs differences in a comment on a word bases.

  Whitespace is not included with changes, but marked up
  where in common.

*/
void output_comment_word(reader_state & rbuf_old, node_sets * node_sets_old, reader_state & rbuf_new, node_sets * node_sets_new, writer_state & wstate) {

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

        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one)->back() + 1
                      , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)->back() + 1, wstate);

      } else {

        // many to many
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                      , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1, wstate);

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
        output_pure_operation_white_space(rbuf_old, 0
                                          , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, SESINSERT, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case SESDELETE:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                          , rbuf_new, 0, SESDELETE, wstate);

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



