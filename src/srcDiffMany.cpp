#include "srcDiffMany.hpp"

#include "srcDiffChange.hpp"
#include "srcDiffDiff.hpp"
#include "srcDiffMatch.hpp"
#include "srcDiffMeasure.hpp"
#include "srcDiffNested.hpp"
#include "srcDiffSingle.hpp"
#include "srcDiffUtility.hpp"

void output_unmatched(reader_state & rbuf_old, NodeSets * node_sets_old
                      , int start_old, int end_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , int start_new, int end_new
                      , writer_state & wstate) {

  unsigned int finish_old = rbuf_old.last_output;
  unsigned int finish_new = rbuf_new.last_output;

  if(start_old <= end_old && start_old >= 0 && end_old < (signed)node_sets_old->size()) {

    finish_old = node_sets_old->at(end_old)->back() + 1;
  }

  if(start_new <= end_new && start_new >= 0 && end_new < (signed)node_sets_new->size()) {

    finish_new = node_sets_new->at(end_new)->back() + 1;
  }

  NodeSets slice_old;

  for(int i = start_old; i <= end_old; ++i) {

    slice_old.push_back(node_sets_old->at(i));

  }

  NodeSets slice_new;

  for(int i = start_new; i <= end_new; ++i) {

    slice_new.push_back(node_sets_new->at(i));

  }

  if(slice_old.size() == 1 && slice_new.size() > 0
     && complete_nestable(slice_new, rbuf_new.nodes, node_sets_old->at(start_old), rbuf_old.nodes)) {

    NodeSet node_set;

    for(unsigned int i = 0; i < slice_new.size(); ++i) {

      for(unsigned int j = 0; j < slice_new.at(i)->size(); ++j) {

        node_set.push_back(slice_new.at(i)->at(j));

      }

    }

    output_nested(rbuf_old, node_sets_old->at(start_old), rbuf_new, &node_set, SESDELETE, wstate);

  } else if(slice_new.size() == 1 && slice_old.size() > 0
            && complete_nestable(slice_old, rbuf_old.nodes, node_sets_new->at(start_new), rbuf_new.nodes)) {
    NodeSet node_set;

    for(unsigned int i = 0; i < slice_old.size(); ++i) {

      for(unsigned int j = 0; j < slice_old.at(i)->size(); ++j) {

        node_set.push_back(slice_old.at(i)->at(j));

      }

    }

    output_nested(rbuf_old, &node_set, rbuf_new, node_sets_new->at(start_new), SESINSERT, wstate);

  } else
    output_change_white_space(rbuf_old, finish_old, rbuf_new, finish_new, wstate);

}

Moves determine_operations(reader_state & rbuf_old, NodeSets * node_sets_old
                           , reader_state & rbuf_new, NodeSets * node_sets_new
                           , edit * edit_script, writer_state & wstate) {

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  offset_pair * matches = NULL;

  IntPairs old_moved;
  std::vector<int> pos_old;
  NodeSets old_sets;

  for(unsigned int i = 0; (signed)i < edits->length; ++i) {

    unsigned int index = edits->offset_sequence_one + i;

    if(rbuf_old.nodes.at(node_sets_old->at(index)->at(0))->move) {

      old_moved.push_back(IntPair(SESMOVE, 0));

    } else if(rbuf_old.nodes.at(node_sets_old->at(index)->at(0))->nest) {

      old_moved.push_back(IntPair(SESNEST, 0));

    } else {

      old_moved.push_back(IntPair(SESDELETE, 0));
      pos_old.push_back(i);
      old_sets.push_back(node_sets_old->at(index));

    }

  }

  IntPairs new_moved;
  std::vector<int> pos_new;
  NodeSets new_sets;

  for(unsigned int i = 0; (signed)i < edit_next->length; ++i) {

    unsigned int index = edit_next->offset_sequence_two + i;

    if(rbuf_new.nodes.at(node_sets_new->at(index)->at(0))->move) {

      new_moved.push_back(IntPair(SESMOVE, 0));

    } else if(rbuf_new.nodes.at(node_sets_new->at(index)->at(0))->nest) {

      new_moved.push_back(IntPair(SESNEST, 0));

    } else {

      new_moved.push_back(IntPair(SESINSERT, 0));
      pos_new.push_back(i);
      new_sets.push_back(node_sets_new->at(index));

    }

  }

  if(pos_old.size() != 0 && pos_new.size())
    match_differences_dynamic(rbuf_old.nodes, &old_sets, rbuf_new.nodes, &new_sets, &matches);

  offset_pair * matches_save = matches;

  for(; matches; matches = matches->next) {

    old_moved.at(pos_old.at(matches->old_offset)).first = SESCOMMON;
    old_moved.at(pos_old.at(matches->old_offset)).second = pos_new.at(matches->new_offset);

    new_moved.at(pos_new.at(matches->new_offset)).first = SESCOMMON;
    new_moved.at(pos_new.at(matches->new_offset)).second = pos_old.at(matches->old_offset);

  }

  for(; matches_save;) {

    offset_pair * old_match = matches_save;
    matches_save = matches_save->next;
    delete old_match;

  }

  Moves moves;
  moves.push_back(old_moved);
  moves.push_back(new_moved);

  return moves;

}

void output_many(reader_state & rbuf_old, NodeSets * node_sets_old
                 , reader_state & rbuf_new, NodeSets * node_sets_new
                 , edit * edit_script, writer_state & wstate) {

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  Moves moves = determine_operations(rbuf_old, node_sets_old, rbuf_new, node_sets_new, edit_script, wstate);
  IntPairs old_moved = moves.at(0);
  IntPairs new_moved = moves.at(1);

  unsigned int i = 0;
  unsigned int j = 0;

  for(; i < old_moved.size() && j < new_moved.size(); ++i, ++j) {

    unsigned int start_old = i;

    unsigned int start_new = j;

    unsigned int end_old = start_old;

    unsigned int end_new = start_new;

    for(; end_old < old_moved.size() && (old_moved.at(end_old).first == SESDELETE || old_moved.at(end_old).first == SESMOVE); ++end_old)
      ;

    for(; end_new < new_moved.size() && (new_moved.at(end_new).first == SESINSERT || new_moved.at(end_new).first == SESMOVE); ++end_new)
      ;

    // output diffs until match
    output_unmatched(rbuf_old, node_sets_old, edits->offset_sequence_one + start_old,
                     edits->offset_sequence_one + end_old - 1
                     , rbuf_new, node_sets_new, edit_next->offset_sequence_two + start_new
                     , edit_next->offset_sequence_two + end_new - 1
                     , wstate);

    i = end_old;
    j = end_new;

    if(i >= old_moved.size() || j >= new_moved.size())
      break;

    if(old_moved.at(i).first == SESCOMMON && new_moved.at(j).first == SESCOMMON) {
 
      if((xmlReaderTypes)rbuf_old.nodes.at(node_sets_old->at(edits->offset_sequence_one + i)->at(0))->type != XML_READER_TYPE_TEXT
         && (ismethod(wstate.method, METHOD_RAW) || go_down_a_level(rbuf_old, node_sets_old, edits->offset_sequence_one + i
                                                                    , rbuf_new, node_sets_new, edit_next->offset_sequence_two + j, wstate))) {

        output_recursive(rbuf_old, node_sets_old, edits->offset_sequence_one + i
                         , rbuf_new, node_sets_new, edit_next->offset_sequence_two + j, wstate);

      } else {

        // syntax mismatch
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + i)->back() + 1
                                  , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + j)->back() + 1, wstate);
      }

    } else if(old_moved.at(i).first == SESNEST && new_moved.at(j).first == SESNEST) {
 
      if(is_nestable(node_sets_old->at(edits->offset_sequence_one + i)
                     , rbuf_old.nodes, node_sets_new->at(edit_next->offset_sequence_two + j), rbuf_new.nodes)) {

        output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one + i), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + j)
                      , SESINSERT, wstate);

      } else if(is_nestable(node_sets_new->at(edit_next->offset_sequence_two + j)
                            , rbuf_new.nodes, node_sets_old->at(edits->offset_sequence_one + i), rbuf_old.nodes)) {

        output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one + i), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + j)
                      , SESDELETE, wstate);

      } else {

        fprintf(stderr, "Nesting Error\n");
        exit(1);

      }

    } else {

      fprintf(stderr, "Mismatched index: %d-%d\n", old_moved.at(i).first, new_moved.at(j).first);
      exit(1);

    }

  }

  output_unmatched(rbuf_old, node_sets_old, edits->offset_sequence_one + i,
                   edits->offset_sequence_one + old_moved.size() - 1
                   , rbuf_new, node_sets_new, edit_next->offset_sequence_two + j
                   , edit_next->offset_sequence_two + new_moved.size() - 1
                   , wstate);

}
