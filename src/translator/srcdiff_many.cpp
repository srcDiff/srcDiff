// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_many.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_many.hpp>

#include <srcdiff_single.hpp>
#include <srcdiff_nested.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_measure.hpp>
#include <change_matcher.hpp>
#include <type_query.hpp>

constexpr int MOVE = int(SES_INSERT) + 1;

construct::construct_list_view safe_subspan(construct::construct_list_view view, std::size_t start, std::size_t end) {
  if(start < 0 || start > end || end > view.size()) return construct::construct_list_view();
  return view.subspan(start, end - start + 1);
}

srcdiff_many::srcdiff_many(const srcdiff_diff & diff, edit_t * edit_script) : srcdiff_diff(diff), edit_script(edit_script) {}

void srcdiff_many::output_unmatched(construct::construct_list_view original_unmatched, construct::construct_list_view modified_unmatched) {

  unsigned int finish_original = out->last_output_original();
  unsigned int finish_modified = out->last_output_modified();

  construct::construct_list_view original_view = original_unmatched;
  construct::construct_list_view modified_view = modified_unmatched;
  if(!original_view.empty() || !modified_view.empty()) {

    if(!original_view.empty() && !modified_view.empty()) {

      nest_result nesting;
      do {

        nesting = srcdiff_nested::check_nestable(original_view, modified_view);
    
        finish_original = original_unmatched.back()->end_position() + 1;
        finish_modified = modified_unmatched.back()->end_position() + 1;

        unsigned int pre_nest_end_original = 0;
        if(nesting.start_original > 0) {
          pre_nest_end_original = original_view[nesting.start_original - 1]->end_position() + 1;
        }

        unsigned int pre_nest_end_modified = 0;
        if(nesting.start_modified > 0) {
          pre_nest_end_modified = modified_view[nesting.start_modified - 1]->end_position() + 1;
        }

        srcdiff_change::output_change(out, pre_nest_end_original, pre_nest_end_modified);

        if((nesting.end_original - nesting.start_original) > 0 && (nesting.end_modified - nesting.start_modified) > 0) {

          srcdiff_nested diff(out, original_view.subspan(nesting.start_original, nesting.end_original - nesting.start_original), 
                                   modified_view.subspan(nesting.start_modified, nesting.end_modified - nesting.start_modified),
                                   nesting.operation);
          diff.output();

        }

        original_view = original_view.subspan(nesting.end_original);
        modified_view = modified_view.subspan(nesting.end_modified);

      } while((nesting.end_original - nesting.start_original) > 0 && (nesting.end_modified - nesting.start_modified) > 0 && !original_view.empty() && !modified_view.empty());

      /** @todo may only need to do this if not at end */
      if(original_view.empty() && modified_view.empty()) {

        srcdiff_change::output_change(out, finish_original, finish_modified);
        return;

      }

    } else {

      if(!original_view.empty()) {

        finish_original = original_unmatched.back()->end_position() + 1;
      }

      if(!modified_view.empty()) {

        finish_modified = modified_unmatched.back()->end_position() + 1;
      }

    }

    if(original_view.size() == 1 && modified_view.size() == 1) {

      if(is_identifier(original_view.front()->term(0)->get_name())
         && is_identifier(modified_view.front()->term(0)->get_name())) {
         output_replace_inner_whitespace(original_view.front()->start_position(), finish_original,
                                         modified_view.front()->start_position(), finish_modified,
                                         1);
       
          }

       if(original_view.front()->term(0)->get_name() == "return"
          && modified_view.front()->term(0)->get_name() == "return") {
          output_replace_inner_whitespace(original_view.front()->start_position(), finish_original,
                                          modified_view.front()->start_position(), finish_modified,
                                          2);
        
          }

       if(original_view.front()->term(0)->get_name() == "throw"
          && modified_view.front()->term(0)->get_name() == "throw") {
          output_replace_inner_whitespace(original_view.front()->start_position(), finish_original,
                                          modified_view.front()->start_position(), finish_modified,
                                          2);
        
          }

    }

    output_change_whitespace(finish_original, finish_modified);

  }

}

/** loop O(RD^2) */
srcdiff_many::moves srcdiff_many::determine_operations() {

  edit_t * edits = edit_script;
  edit_t * edit_next = edit_script->next;

  srcdiff::offset_pair * matches = NULL;

  int_pairs original_moved;
  std::vector<int> pos_original;
  construct::construct_list original_sets;

  for(std::size_t i = 0; i < edits->length; ++i) {

    unsigned int index = edits->offset_sequence_one + i;

    if(original[index]->term(0)->get_move()) {

      original_moved.push_back(int_pair(MOVE, 0));

    } else {

      original_moved.push_back(int_pair(SES_DELETE, 0));
      pos_original.push_back(i);
      original_sets.push_back(original[index]);

    }

  }

  int_pairs modified_moved;
  std::vector<int> pos_modified;
  construct::construct_list modified_sets;

  for(std::size_t i = 0; i < edit_next->length; ++i) {

    unsigned int index = edit_next->offset_sequence_two + i;

    if(modified[index]->term(0)->get_move()) {

      modified_moved.push_back(int_pair(MOVE, 0));

    } else {

      modified_moved.push_back(int_pair(SES_INSERT, 0));
      pos_modified.push_back(i);
      modified_sets.push_back(modified[index]);

    }

  }

  if(pos_original.size() != 0 && pos_modified.size()) {

    srcdiff::change_matcher matcher(original_sets, modified_sets);
    matches = matcher.match_differences();

  }

  srcdiff::offset_pair * matches_save = matches;

  for(; matches; matches = matches->next) {

    original_moved.at(pos_original.at(matches->original_offset)).first = SES_COMMON;
    original_moved.at(pos_original.at(matches->original_offset)).second = pos_modified.at(matches->modified_offset);

    modified_moved.at(pos_modified.at(matches->modified_offset)).first = SES_COMMON;
    modified_moved.at(pos_modified.at(matches->modified_offset)).second = pos_original.at(matches->original_offset);

  }

  for(; matches_save;) {

    srcdiff::offset_pair * original_match = matches_save;
    matches_save = matches_save->next;
    delete original_match;

  }

  srcdiff_many::moves moves;
  moves.push_back(original_moved);
  moves.push_back(modified_moved);

  return moves;

}

void srcdiff_many::output() {

  edit_t * edits = edit_script;
  edit_t * edit_next = edit_script->next;

  srcdiff_many::moves moves = determine_operations();
  int_pairs original_moved = moves.at(0);
  int_pairs modified_moved = moves.at(1);

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
    output_unmatched(safe_subspan(original, edits->offset_sequence_one + start_original, edits->offset_sequence_one + end_original - 1), 
                     safe_subspan(modified, edit_next->offset_sequence_two + start_modified, edit_next->offset_sequence_two + end_modified - 1));

    i = end_original;
    j = end_modified;

    if(i >= original_moved.size() || j >= modified_moved.size()) {
      break;
    }

    if(original_moved.at(i).first == SES_COMMON && modified_moved.at(j).first == SES_COMMON) {
 
      if(original[edits->offset_sequence_one + i]->term(0)->get_type() != srcML::node_type::TEXT) {

        srcdiff_single diff(out, original[edits->offset_sequence_one + i], modified[edit_next->offset_sequence_two + j]);
        
        diff.output();

      } else {

        // syntax mismatch
        output_change_whitespace(original[edits->offset_sequence_one + i]->end_position() + 1,
                                 modified[edit_next->offset_sequence_two + j]->end_position() + 1);

      }

    } else {

      fprintf(stderr, "Mismatched index: %d-%d\n", original_moved.at(i).first, modified_moved.at(j).first);
      exit(1);

    }

  }

  output_unmatched(safe_subspan(original, edits->offset_sequence_one + i, edits->offset_sequence_one + original_moved.size() - 1),
                   safe_subspan(modified, edit_next->offset_sequence_two + j, edit_next->offset_sequence_two + modified_moved.size() - 1));

}
