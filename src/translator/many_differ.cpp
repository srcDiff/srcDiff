// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file many_differ.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <many_differ.hpp>

#include <match_differ.hpp>
#include <convert_differ.hpp>
#include <nested_differ.hpp>
#include <change_stream.hpp>
#include <measurer.hpp>
#include <change_matcher.hpp>
#include <type_query.hpp>

namespace srcdiff {

static construct::construct_list_view safe_subspan(construct::construct_list_view view, std::size_t start, std::size_t end) {
  if(start < 0 || start > end || end > view.size()) return construct::construct_list_view();
  return view.subspan(start, end - start + 1);
}

many_differ::many_differ(const differ& diff, edit_t* edit_script) 
  : differ(diff), edit_script(edit_script), original_sets(), modified_sets() {}

void many_differ::output_unmatched(construct::construct_list_view original_unmatched, construct::construct_list_view modified_unmatched) {

  unsigned int finish_original = out->last_output_original();
  unsigned int finish_modified = out->last_output_modified();

  construct::construct_list_view original_view = original_unmatched;
  construct::construct_list_view modified_view = modified_unmatched;
  if(!original_view.empty() || !modified_view.empty()) {

    if(!original_view.empty() && !modified_view.empty()) {

      nest_result nesting;
      do {

        nesting = nested_differ::check_nestable(original_view, modified_view);
    
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

        change_stream::output_change(out, pre_nest_end_original, pre_nest_end_modified);

        if((nesting.end_original - nesting.start_original) > 0 && (nesting.end_modified - nesting.start_modified) > 0) {

          nested_differ diff(out, original_view.subspan(nesting.start_original, nesting.end_original - nesting.start_original), 
                                  modified_view.subspan(nesting.start_modified, nesting.end_modified - nesting.start_modified),
                                  nesting.operation);
          diff.output();

        }

        original_view = original_view.subspan(nesting.end_original);
        modified_view = modified_view.subspan(nesting.end_modified);

      } while((nesting.end_original - nesting.start_original) > 0 && (nesting.end_modified - nesting.start_modified) > 0 && !original_view.empty() && !modified_view.empty());

      /** @todo may only need to do this if not at end */
      if(original_view.empty() && modified_view.empty()) {

        change_stream::output_change(out, finish_original, finish_modified);
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
change_list many_differ::determine_operations() {

  edit_t * edits = edit_script;
  edit_t * edit_next = edit_script->next;

  for(std::size_t i = 0; i < edits->length; ++i) {
    unsigned int index = edits->offset_sequence_one + i;
    original_sets.push_back(original[index]);
  }

  for(std::size_t i = 0; i < edit_next->length; ++i) {
    unsigned int index = edit_next->offset_sequence_two + i;
    modified_sets.push_back(modified[index]);
  }

  change_list changes;
  if(original_sets.size() && modified_sets.size()) {
    change_matcher matcher(original_sets, modified_sets);
    changes = matcher.match_differences();
  }

  return changes;

}

void many_differ::output() {

  change_list changes = determine_operations();
  for(struct change& change : changes) {

    if(change.operation == srcdiff::COMMON) {
      if(change.original.front()->term(0)->get_type() != srcML::node_type::TEXT) {

        if(change.original.front()->root_term_name() == change.modified.front()->root_term_name()) {
          match_differ diff(out, change.original.front(), change.modified.front());      
          diff.output();
        } else {
          convert_differ diff(out, change.original.front(), change.modified.front());      
          diff.output();
        }

      } else {
        // syntax mismatch
        output_change_whitespace(change.original.front()->end_position() + 1,
                                 change.modified.front()->end_position() + 1);
      }
    } else {
      output_unmatched(change.original, change.modified);
    }

  }

}

}
