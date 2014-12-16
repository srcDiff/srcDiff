#include <srcdiff_many.hpp>

#include <srcdiff_single.hpp>
#include <srcdiff_nested.hpp>

#include <srcDiffChange.hpp>
#include <srcDiffMatch.hpp>
#include <srcDiffMeasure.hpp>
#include <srcDiffUtility.hpp>

srcdiff_many::srcdiff_many(const srcdiff_diff & diff, edit * edit_script) : srcdiff_diff(diff), edit_script(edit_script) {}

void srcdiff_many::output_unmatched(int start_old, int end_old, int start_new, int end_new) {

  unsigned int finish_old = rbuf_old.last_output;
  unsigned int finish_new = rbuf_new.last_output;

  if((start_old <= end_old && start_old >= 0 && end_old < (signed)node_sets_old->size())
      || (start_new <= end_new && start_new >= 0 && end_new < (signed)node_sets_new->size())) {

    if(start_old <= end_old && start_old >= 0 && end_old < (signed)node_sets_old->size()
      && start_new <= end_new && start_new >= 0 && end_new < (signed)node_sets_new->size()) {

      int start_nest_old, end_nest_old, start_nest_new, end_nest_new, operation;

      do {

        srcdiff_nested::check_nestable(node_sets_old, rbuf_old.nodes, start_old, end_old + 1
                        , node_sets_new, rbuf_new.nodes, start_new, end_new + 1
                        , start_nest_old, end_nest_old, start_nest_new, end_nest_new, operation);

        finish_old = node_sets_old->at(end_old)->back() + 1;
        finish_new = node_sets_new->at(end_new)->back() + 1;

        unsigned int pre_nest_end_old = 0;
        if(start_nest_old > 0) {

          pre_nest_end_old = node_sets_old->at(start_nest_old - 1)->back() + 1;

        }

        unsigned int pre_nest_end_new = 0;
        if(start_nest_new > 0) {

          pre_nest_end_new = node_sets_new->at(start_nest_new - 1)->back() + 1;

        }

        output_change(rbuf_old, pre_nest_end_old, rbuf_new, pre_nest_end_new, wstate);

        if((end_nest_old - start_nest_old) > 0 && (end_nest_new - start_nest_new) > 0) {

          srcdiff_nested diff(*this, start_nest_old, end_nest_old, start_nest_new, end_nest_new, operation);
          diff.output();

        }

        start_old = end_nest_old;
        start_new = end_nest_new;

      } while((end_nest_old - start_nest_old) > 0 && (end_nest_new - start_nest_new) > 0 && end_nest_old <= end_old && end_nest_new <= end_new);

      /** @todo may only need to do this if not at end */
      if(end_nest_old > end_old && end_nest_new > end_new) {

        output_change(rbuf_old, finish_old, rbuf_new, finish_new, wstate);
        return;

      }

    } else {

      if(start_old <= end_old && start_old >= 0 && end_old < (signed)node_sets_old->size()) {

        finish_old = node_sets_old->at(end_old)->back() + 1;
      }

      if(start_new <= end_new && start_new >= 0 && end_new < (signed)node_sets_new->size()) {

        finish_new = node_sets_new->at(end_new)->back() + 1;
      }

    }

    output_change_white_space(rbuf_old, finish_old, rbuf_new, finish_new, wstate);
              
  }

}

srcdiff_many::Moves srcdiff_many::determine_operations() {

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  offset_pair * matches = NULL;

  IntPairs old_moved;
  std::vector<int> pos_old;
  node_sets old_sets(rbuf_old.nodes);

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
  node_sets new_sets(rbuf_new.nodes);

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

  srcdiff_many::Moves moves;
  moves.push_back(old_moved);
  moves.push_back(new_moved);

  return moves;

}

void srcdiff_many::output() {

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  srcdiff_many::Moves moves = determine_operations();
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
    output_unmatched(edits->offset_sequence_one + start_old, edits->offset_sequence_one + end_old - 1, 
                      edit_next->offset_sequence_two + start_new, edit_next->offset_sequence_two + end_new - 1);

    i = end_old;
    j = end_new;

    if(i >= old_moved.size() || j >= new_moved.size())
      break;

    if(old_moved.at(i).first == SESCOMMON && new_moved.at(j).first == SESCOMMON) {
 
      if((xmlReaderTypes)rbuf_old.nodes.at(node_sets_old->at(edits->offset_sequence_one + i)->at(0))->type != XML_READER_TYPE_TEXT
         && (ismethod(wstate.method, METHOD_RAW) || srcdiff_diff::go_down_a_level(rbuf_old, node_sets_old, edits->offset_sequence_one + i
                                                                    , rbuf_new, node_sets_new, edit_next->offset_sequence_two + j, wstate))) {

        srcdiff_single diff(*this, edits->offset_sequence_one + i, edit_next->offset_sequence_two + j);
        diff.output();

      } else {

        // syntax mismatch
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + i)->back() + 1
                                  , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + j)->back() + 1, wstate);
      }

    }
    /** @todo this appears to now be dead code */
     else if(old_moved.at(i).first == SESNEST && new_moved.at(j).first == SESNEST) {
  
      if(srcdiff_nested::is_nestable(node_sets_old->at(edits->offset_sequence_one + i)
                     , rbuf_old.nodes, node_sets_new->at(edit_next->offset_sequence_two + j), rbuf_new.nodes)) {

          int nest_length = 1;
          while(i + nest_length < old_moved.size() && old_moved.at(i + nest_length).first == SESNEST)
            ++nest_length;

          srcdiff_nested diff(*this, edits->offset_sequence_one + i, edits->offset_sequence_one + i + nest_length,
                              edit_next->offset_sequence_two + j, edit_next->offset_sequence_two + j + 1, SESINSERT);
          diff.output();

      } else if(srcdiff_nested::is_nestable(node_sets_new->at(edit_next->offset_sequence_two + j)
                            , rbuf_new.nodes, node_sets_old->at(edits->offset_sequence_one + i), rbuf_old.nodes)) {

          int nest_length = 1;
          while(j + nest_length < new_moved.size() && new_moved.at(j + nest_length).first == SESNEST)
            ++nest_length;

          srcdiff_nested diff(*this, edits->offset_sequence_one + i, edits->offset_sequence_one + i + 1,
                              edit_next->offset_sequence_two + j, edit_next->offset_sequence_two + j + nest_length, SESDELETE);
          diff.output();

      } else {

        fprintf(stderr, "Nesting Error\n");
        exit(1);

      }

    } else {

      fprintf(stderr, "Mismatched index: %d-%d\n", old_moved.at(i).first, new_moved.at(j).first);
      exit(1);

    }

  }

  output_unmatched(edits->offset_sequence_one + i, edits->offset_sequence_one + old_moved.size() - 1
                   , edit_next->offset_sequence_two + j, edit_next->offset_sequence_two + new_moved.size() - 1);

}
