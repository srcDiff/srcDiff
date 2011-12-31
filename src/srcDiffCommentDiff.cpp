#include "srcDiffCommentDiff.hpp"
#include "shortest_edit_script.h"
#include "srcDiffUtility.hpp"
#include "srcDiffCommon.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffDiff.hpp"

/*
  Collect paragraphs
*/
std::vector<std::vector<int> *> create_comment_paragraph_set(std::vector<xNodePtr> & nodes, int start, int end) {

  // collect all the paragraphs separated by double newlines
  std::vector<std::vector<int> *> node_sets;
  for(int i = start; i < end; ++i) {

    // move past any starting newlines
    for(; is_new_line(nodes.at(i)); ++i)
      ;

    // collect the nodes in the paragraph
    std::vector <int> * node_set = new std::vector <int>;

    int newlines = 0;
    for(; i < end; ++i) {

      if(is_new_line(nodes.at(i)))
        ++newlines;

      if(newlines > 1)
        break;

      if(!is_white_space(nodes.at(i)))
        node_set->push_back(i);
    }

    node_sets.push_back(node_set);

  }

  return node_sets;

}

// collect lines
std::vector<std::vector<int> *> create_comment_line_set(std::vector<xNodePtr> & nodes, int start, int end) {

  std::vector<std::vector<int> *> node_sets;

  for(int i = start; i < end; ++i) {

    std::vector<int> * node_set = new std::vector <int>;

    for(; i < end; ++i) {

      // stop the node set at the newline
      if(is_new_line(nodes.at(i)))
        break;

      // only collect non-whitespace nodes
      if(!is_white_space(nodes.at(i)))
        node_set->push_back(i);
    }

    node_sets.push_back(node_set);

  }

  return node_sets;

}

/*

  Once a comment change is encountered, they are first matched based
  on paragraphs (separated by two new lines), then as needed, they are processes by line
  , and then by word using shortest edit script. Whitespace is included after/before changes

*/
void output_comment_paragraph(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, writer_state & wstate) {

  edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script, 0);

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
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

    // detect and change
    edit * edit_next = edits->next;
    if(is_change(edits)) {

      // 1-1
      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        std::vector<std::vector<int> *> next_node_set_old
          = create_node_set(rbuf_old.nodes, node_sets_old->at(edits->offset_sequence_one)->at(0)
                                    , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1) + 1);

        std::vector<std::vector<int> *> next_node_set_new
          = create_node_set(rbuf_new.nodes, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                                    , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1) + 1);

        // compare as lines
        output_comment_word(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

        free_node_sets(next_node_set_old);
        free_node_sets(next_node_set_new);

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

      case INSERT:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, 0
                                          , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, INSERT, wstate);
        // update for common
        last_diff_old = edits->offset_sequence_one + 1;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case DELETE:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                          , rbuf_new, 0, DELETE, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two + 1;

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

  free_shortest_edit_script(edit_script);

}

/*

  Breaks down paragraphs and runs on a line bases.
  Whitespace is included after/before changes

*/
void output_comment_line(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, writer_state & wstate) {

  edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script, 0);

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
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

    // detect and change
    edit * edit_next = edits->next;
    if(is_change(edits)) {

      // 1-1
      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        std::vector<std::vector<int> *> next_node_set_old
          = create_node_set(rbuf_old.nodes, node_sets_old->at(edits->offset_sequence_one)->at(0)
                            , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1) + 1);

        std::vector<std::vector<int> *> next_node_set_new
          = create_node_set(rbuf_new.nodes, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                            , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1) + 1);

        // compare on word basis
        output_comment_word(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

        free_node_sets(next_node_set_old);
        free_node_sets(next_node_set_new);

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

      case INSERT:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, 0
                                          , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, INSERT, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + 1;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case DELETE:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                          , rbuf_new, 0, DELETE, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two + 1;

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
void output_comment_word(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, writer_state & wstate) {

  //fprintf(stderr, "HERE_DOUBLE\n");

  edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script, 0);

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
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
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

      case INSERT:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, 0
                                          , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, INSERT, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + 1;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case DELETE:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                          , rbuf_new, 0, DELETE, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two + 1;

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

  free_shortest_edit_script(edit_script);

}



