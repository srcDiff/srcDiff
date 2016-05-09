#include <srcdiff_many.hpp>

#include <srcdiff_single.hpp>
#include <srcdiff_nested.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_match.hpp>
#include <type_query.hpp>

#include <unordered_map>

constexpr int MOVE = int(SES_INSERT) + 1;

srcdiff_many::srcdiff_many(const srcdiff_diff & diff, edit * edit_script) : srcdiff_diff(diff), edit_script(edit_script) {}

void srcdiff_many::output_unmatched(int start_original, int end_original, int start_modified, int end_modified) {

  unsigned int finish_original = out.last_output_original();
  unsigned int finish_modified = out.last_output_modified();

  if((start_original <= end_original && start_original >= 0 && end_original < (signed)node_sets_original.size())
      || (start_modified <= end_modified && start_modified >= 0 && end_modified < (signed)node_sets_modified.size())) {

    if(start_original <= end_original && start_original >= 0 && end_original < (signed)node_sets_original.size()
      && start_modified <= end_modified && start_modified >= 0 && end_modified < (signed)node_sets_modified.size()) {

      int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;

      do {

        srcdiff_nested::check_nestable(node_sets_original, start_original, end_original + 1
                        , node_sets_modified, start_modified, end_modified + 1
                        , start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation);

        finish_original = node_sets_original.at(end_original).back() + 1;
        finish_modified = node_sets_modified.at(end_modified).back() + 1;

        unsigned int pre_nest_end_original = 0;
        if(start_nest_original > 0) {

          pre_nest_end_original = node_sets_original.at(start_nest_original - 1).back() + 1;

        }

        unsigned int pre_nest_end_modified = 0;
        if(start_nest_modified > 0) {

          pre_nest_end_modified = node_sets_modified.at(start_nest_modified - 1).back() + 1;

        }

        output_change(pre_nest_end_original, pre_nest_end_modified);

        if((end_nest_original - start_nest_original) > 0 && (end_nest_modified - start_nest_modified) > 0) {

          srcdiff_nested diff(*this, start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation);
          diff.output();

        }

        start_original = end_nest_original;
        start_modified = end_nest_modified;

      } while((end_nest_original - start_nest_original) > 0 && (end_nest_modified - start_nest_modified) > 0 && end_nest_original <= end_original && end_nest_modified <= end_modified);

      /** @todo may only need to do this if not at end */
      if(end_nest_original > end_original && end_nest_modified > end_modified) {

        output_change(finish_original, finish_modified);
        return;

      }

    } else {

      if(start_original <= end_original && start_original >= 0 && end_original < (signed)node_sets_original.size()) {

        finish_original = node_sets_original.at(end_original).back() + 1;
      }

      if(start_modified <= end_modified && start_modified >= 0 && end_modified < (signed)node_sets_modified.size()) {

        finish_modified = node_sets_modified.at(end_modified).back() + 1;
      }

    }

    if(start_original == end_original && start_original >= 0 && end_original < (signed)node_sets_original.size()
      && start_modified == end_modified && start_modified >= 0 && end_modified < (signed)node_sets_modified.size()) {

      if(is_identifier(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name)
         && is_identifier(out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))->name))
         output_replace_inner_whitespace(node_sets_original.at(start_original).at(0), finish_original,
                                         node_sets_modified.at(start_modified).at(0), finish_modified,
                                         1);

       if(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name == "return"
          && out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))->name == "return")
          output_replace_inner_whitespace(node_sets_original.at(start_original).at(0), finish_original,
                                          node_sets_modified.at(start_modified).at(0), finish_modified,
                                          2);

       if(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name == "throw"
          && out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))->name == "throw")
          output_replace_inner_whitespace(node_sets_original.at(start_original).at(0), finish_original,
                                          node_sets_modified.at(start_modified).at(0), finish_modified,
                                          2);

    }

    output_change_whitespace(finish_original, finish_modified);
              
  }

}

srcdiff_many::moves srcdiff_many::determine_operations() {

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  offset_pair * matches = NULL;

  int_pairs original_moved;
  std::vector<int> pos_original;
  node_sets original_sets(out.get_nodes_original());

  for(unsigned int i = 0; (signed)i < edits->length; ++i) {

    unsigned int index = edits->offset_sequence_one + i;

    if(out.get_nodes_original().at(node_sets_original.at(index).at(0))->move) {

      original_moved.push_back(int_pair(MOVE, 0));

    } else {

      original_moved.push_back(int_pair(SES_DELETE, 0));
      pos_original.push_back(i);
      original_sets.push_back(node_sets_original.at(index));

    }

  }

  int_pairs modified_moved;
  std::vector<int> pos_modified;
  node_sets modified_sets(out.get_nodes_modified());

  for(unsigned int i = 0; (signed)i < edit_next->length; ++i) {

    unsigned int index = edit_next->offset_sequence_two + i;

    if(out.get_nodes_modified().at(node_sets_modified.at(index).at(0))->move) {

      modified_moved.push_back(int_pair(MOVE, 0));

    } else {

      modified_moved.push_back(int_pair(SES_INSERT, 0));
      pos_modified.push_back(i);
      modified_sets.push_back(node_sets_modified.at(index));

    }

  }

  if(pos_original.size() != 0 && pos_modified.size()) {

    srcdiff_match match(original_sets, modified_sets);
    matches = match.match_differences();

  }

  offset_pair * matches_save = matches;

  for(; matches; matches = matches->next) {

    original_moved.at(pos_original.at(matches->original_offset)).first = SES_COMMON;
    original_moved.at(pos_original.at(matches->original_offset)).second = pos_modified.at(matches->modified_offset);

    modified_moved.at(pos_modified.at(matches->modified_offset)).first = SES_COMMON;
    modified_moved.at(pos_modified.at(matches->modified_offset)).second = pos_original.at(matches->original_offset);

  }

  for(; matches_save;) {

    offset_pair * original_match = matches_save;
    matches_save = matches_save->next;
    delete original_match;

  }

  srcdiff_many::moves moves;
  moves.push_back(original_moved);
  moves.push_back(modified_moved);

  return moves;

}

void srcdiff_many::output() {

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  srcdiff_many::moves moves = determine_operations();
  int_pairs original_moved = moves.at(0);
  int_pairs modified_moved = moves.at(1);

  // assumption only one common pair possible
  for(unsigned int i = 0, j = 0; i < original_moved.size() && j < modified_moved.size(); ++i, ++j) {

    unsigned int start_original = i;

    unsigned int start_modified = j;

    unsigned int end_original = start_original;

    unsigned int end_modified = start_modified;

    for(; end_original < original_moved.size() && (original_moved.at(end_original).first == SES_DELETE || original_moved.at(end_original).first == MOVE); ++end_original)
      ;
    for(; end_modified < modified_moved.size() && (modified_moved.at(end_modified).first == SES_INSERT || modified_moved.at(end_modified).first == MOVE); ++end_modified)
      ;

    std::unordered_map<int, int> move_list(end_original - start_original);
    for(int pos = start_original; pos < end_original; ++pos) {

      if(original_moved.at(pos).first == MOVE) {

        int move_id = node_sets_original.nodes().at(node_sets_original.at(edits->offset_sequence_one + pos).at(0))->move;
        move_list.emplace(move_id, pos);

      }

    }

    for(int pos = start_modified; pos < end_modified; ++pos) {

      if(modified_moved.at(pos).first == MOVE) {

        const node_set & move_set = node_sets_modified.at(edit_next->offset_sequence_two + pos);
        int move_id = node_sets_modified.nodes().at(move_set.at(0))->move;
        std::unordered_map<int, int>::iterator itr = move_list.find(move_id);
        if(itr != move_list.end()) {

          const node_set & original_set = node_sets_original.at(edit_next->offset_sequence_one + itr->second);
          node_sets_original.nodes().at(original_set.at(0))->move = 0;
          node_sets_original.nodes().at(original_set.back())->move = 0;         
          original_moved.at(itr->second).first = SES_COMMON;
          original_moved.at(itr->second).second = pos;

          modified_moved.at(pos).first = SES_COMMON;
          modified_moved.at(pos).second = itr->second;
          node_sets_modified.nodes().at(move_set.at(0))->move = 0;
          node_sets_modified.nodes().at(move_set.back())->move = 0;

        }
        
      }

    }

  }

  unsigned int i = 0;
  unsigned int j = 0;
  for(; i < original_moved.size() && j < modified_moved.size(); ++i, ++j) {

    unsigned int start_original = i;

    unsigned int start_modified = j;

    unsigned int end_original = start_original;

    unsigned int end_modified = start_modified;

    for(; end_original < original_moved.size() && (original_moved.at(end_original).first == SES_DELETE || original_moved.at(end_original).first == MOVE); ++end_original)
      ;
    for(; end_modified < modified_moved.size() && (modified_moved.at(end_modified).first == SES_INSERT || modified_moved.at(end_modified).first == MOVE); ++end_modified)
      ;

    // output diffs until match
    output_unmatched(edits->offset_sequence_one + start_original, edits->offset_sequence_one + end_original - 1, 
                     edit_next->offset_sequence_two + start_modified, edit_next->offset_sequence_two + end_modified - 1);

    i = end_original;
    j = end_modified;

    if(i >= original_moved.size() || j >= modified_moved.size())
      break;

    if(original_moved.at(i).first == SES_COMMON && modified_moved.at(j).first == SES_COMMON) {
 
      if((xmlReaderTypes)out.get_nodes_original().at(node_sets_original.at(edits->offset_sequence_one + i).at(0))->type != XML_READER_TYPE_TEXT) {

        srcdiff_single diff(*this, edits->offset_sequence_one + i, edit_next->offset_sequence_two + j);
        diff.output();

      } else {

        // syntax mismatch
        output_change_whitespace(node_sets_original.at(edits->offset_sequence_one + i).back() + 1,
          node_sets_modified.at(edit_next->offset_sequence_two + j).back() + 1);

      }

    } else {

      fprintf(stderr, "Mismatched index: %d-%d\n", original_moved.at(i).first, modified_moved.at(j).first);
      exit(1);

    }

  }

  output_unmatched(edits->offset_sequence_one + i, edits->offset_sequence_one + original_moved.size() - 1,
                   edit_next->offset_sequence_two + j, edit_next->offset_sequence_two + modified_moved.size() - 1);

}
