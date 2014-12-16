#include <srcdiff_diff.hpp>
#include <srcDiffUtility.hpp>
#include <srcDiffOutput.hpp>
#include <srcDiffCommon.hpp>
#include <srcDiffChange.hpp>
#include <srcDiffCommentDiff.hpp>
#include <srcDiffWhiteSpace.hpp>
#include <srcDiffNested.hpp>
#include <srcDiffMeasure.hpp>
#include <srcDiffMatch.hpp>
#include <srcDiffMove.hpp>
#include <srcDiffMany.hpp>
#include <srcDiffSingle.hpp>
#include <pthread.h>
#include <ShortestEditScript.hpp>

#include <string.h>
#include <Methods.hpp>

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;


srcdiff_diff::srcdiff_diff(reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate, node_sets * node_sets_old, node_sets * node_sets_new) 
  : rbuf_old(rbuf_old), rbuf_new(rbuf_new), wstate(wstate), node_sets_old(node_sets_old), node_sets_new(node_sets_new) {}

bool go_down_a_level(reader_state & rbuf_old, node_sets * node_sets_old
                     , unsigned int start_old
                     , reader_state & rbuf_new, node_sets * node_sets_new
                     , unsigned int start_new
                     , writer_state & wstate) {


  if(strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "expr_stmt") != 0
     && strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "decl_stmt") != 0
     && strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "expr") != 0)
    return true;

  int similarity, difference, text_old_length, text_new_length;
  compute_measures(rbuf_old.nodes, node_sets_old->at(start_old), rbuf_new.nodes, node_sets_new->at(start_new),
    similarity, difference, text_old_length, text_new_length);

  return !reject_match(similarity, difference, text_old_length, text_new_length,
          rbuf_old.nodes, node_sets_old->at(start_old), rbuf_new.nodes, node_sets_new->at(start_new));

}

bool group_sub_elements(reader_state & rbuf_old, node_sets * node_sets_old
                        , unsigned int start_old
                        , reader_state & rbuf_new, node_sets * node_sets_new
                        , unsigned int start_new
                        , writer_state & wstate) {


  if(strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "type") != 0)
    return false;

  unsigned int similarity = compute_similarity(rbuf_old.nodes, node_sets_old->at(start_old), rbuf_new.nodes, node_sets_new->at(start_new));

  unsigned int olength = node_sets_old->at(start_old)->size();
  unsigned int nlength = node_sets_new->at(start_new)->size();

  unsigned int size_old = 0;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(i))) && !is_white_space(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(i))))
      ++size_old;

  unsigned int size_new = 0;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(i))) && !is_white_space(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(i))))
      ++size_new;

  unsigned int min_length = size_old;
  if(size_new < min_length)
    min_length = size_new;

  return 4 * similarity < 3 * min_length;

}

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

  diff_nodes dnodes = { rbuf_old.nodes, rbuf_new.nodes };

  ShortestEditScript ses(node_set_syntax_compare, node_set_index, &dnodes);

  int distance = ses.compute((const void *)node_sets_old, node_sets_old->size(), (const void *)node_sets_new, node_sets_new->size());

  edit * edit_script = ses.get_script();

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  mark_moves(rbuf_old, node_sets_old, rbuf_new, node_sets_new, edit_script, wstate);

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

    } else if(edits->operation == SESINSERT && edits->offset_sequence_one != 0 && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(edits->offset_sequence_two - 1)->back() + 1;

    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);
    // detect and change
    edit * edit_next = edits->next;

    if(is_change(edits)) {

      //      fprintf(stderr, "HERE\n");

      // many to many handling
      output_many(rbuf_old, node_sets_old, rbuf_new, node_sets_new, edits, wstate);

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
                                          , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1,
                                          SESINSERT, wstate);


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
