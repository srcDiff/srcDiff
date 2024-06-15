// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_nested.cpp
 *
 * @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_nested.hpp>

#include <srcdiff_constants.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_text_measure.hpp>
#include <srcdiff_match.hpp>
#include <shortest_edit_script.h>
#include <type_query.hpp>

#include <name.hpp>

#include <algorithm>
#include <cstring>

srcdiff_nested::srcdiff_nested(std::shared_ptr<srcdiff_output> out, const construct::construct_list_view original, const construct::construct_list_view modified, int operation)
  : srcdiff_diff(out, original, modified), operation(operation) {}

bool has_compound_inner(std::shared_ptr<const construct> & node_set_outer) {

  if(node_set_outer->root_term()->is_simple()) return false;

  for(unsigned int i = 1; i < node_set_outer->size(); ++i) {
    if(node_set_outer->term(i)->is_start()
      && node_set_outer->term(i)->get_name() == "name" && !node_set_outer->term(i)->is_simple())
      return true;
  }

  return false;

}

bool is_better_nest(std::shared_ptr<const construct> node_set_outer,
                    std::shared_ptr<const construct> node_set_inner,
                    const srcdiff_measure & measure, bool recurse = true) {

  // do not nest compound name in simple or anything into something that is not compound
  if(node_set_outer->root_term_name() == "name" && node_set_inner->root_term_name() == "name"
    &&    (node_set_outer->root_term()->is_simple()
      || (!node_set_inner->root_term()->is_simple() && !has_compound_inner(node_set_outer)))) return false;

  // parents and children same do not nest.
  if(node_set_outer->can_nest(*node_set_inner)) {

    std::shared_ptr<const construct> best_match = node_set_outer->find_best_descendent(*node_set_inner);

    if(best_match) {

      srcdiff_text_measure match_measure(*best_match, *node_set_inner);
      match_measure.compute();

      double min_size = measure.min_length();
      double nest_min_size = match_measure.min_length();

      /** in addition to being a valid match
          must have > or = similarity < or = difference or a better ratio of size to similarity
      */
      if((match_measure.similarity() >= measure.similarity() && match_measure.difference() <= measure.difference())
       || ((nest_min_size / match_measure.similarity()) < (min_size / measure.similarity())
        // old code used node_set_outer (i.e., is it interchangeable) this seemed wrong
        // fixes test case, but it failed because interchange not implemented (passes now)
        // that interchange implemented
          && node_set_inner->check_nest(*best_match))
       ) {
        // check if other way is better
        return recurse? !is_better_nest(node_set_inner, node_set_outer, match_measure, false) : true;
      }

    }

  }

  return false;

}

bool srcdiff_nested::is_better_nested(construct::construct_list_view original, construct::construct_list_view modified) {

  const srcdiff_measure & measure = *original[0]->measure(*modified[0]);

  for(std::size_t pos = 0; pos < original.size(); ++pos) {

    nest_result nestable = check_nestable(original, modified);
    if(nestable.operation == SES_COMMON) continue;
    if(is_better_nest(original[pos], modified[0], measure)) {
      return true;
    }

  }

  for(std::size_t pos = 0; pos < modified.size(); ++pos) {

    nest_result nestable = check_nestable(original, modified);
    if(nestable.operation == SES_COMMON) continue;
    if(is_better_nest(modified[pos], original[0], measure)) {
      return true;
    }

  }

  return false;

}

static nest_result check_nested_single_to_many(construct::construct_list_view original, construct::construct_list_view modified) {

  int nest_count_original = 0;
  std::optional<int> pos_original;
  std::optional<int> similarity_original;
  std::optional<int> difference_original;
  int is_name_nest_original = 0;
  for(std::size_t i = 0; i < original.size(); ++i) {

    if(original[i]->root_term()->get_move()) continue;

    for(std::size_t j = 0; j < modified.size(); ++j) {

      if(modified[j]->root_term()->get_move()) continue;

      if(original[i]->can_nest(*modified[j])) {

        std::shared_ptr<const construct> best_match = original[i]->find_best_descendent(*modified[j]);
        if(!best_match) continue;

        srcdiff_text_measure measure(*best_match, *modified[j]);
        measure.compute();

        if(!best_match->check_nest(*modified[j])) {
          continue;
        }

        if(modified[j]->root_term_name() == "name"
          && modified[j]->root_term()->get_parent() && modified[j]->root_term()->get_parent()->get_name() == "expr"
          && original[i]->root_term()->get_parent() && original[i]->root_term()->get_parent()->get_name() == "expr"
          && ((original.size() - 0) > 1 || (modified.size() - 0) > 1)) {
          ++is_name_nest_original;
        }

        if(!bool(pos_original) || measure.similarity() > similarity_original) {

          ++nest_count_original;
          pos_original = i;
          similarity_original = measure.similarity();
          difference_original = measure.difference();

        }

      }

    }

  }

  if(nest_count_original > 1 && is_name_nest_original > 1) {
    nest_count_original = 0;
    pos_original = std::optional<int>();
    similarity_original = std::optional<int>();
    difference_original = std::optional<int>();
  }

  int nest_count_modified = 0;
  std::optional<int> pos_modified;
  std::optional<int> similarity_modified;
  std::optional<int> difference_modified;
  int is_name_nest_modified = 0;
  for(std::size_t i = 0; i < modified.size(); ++i) {

    if(modified[i]->root_term()->get_move()) continue;

    for(std::size_t j = 0; j < original.size(); ++j) {

      if(original[j]->root_term()->get_move()) continue;

      if(modified[i]->can_nest(*original[j])) {

        std::shared_ptr<const construct> best_match = modified[i]->find_best_descendent(*original[j]);
        if(!best_match) continue;

        srcdiff_text_measure measure(*original[j], *best_match);
        measure.compute();

        if(!original[j]->check_nest(*best_match)) {
          continue;
        }

        if(original[j]->root_term_name() == "name"
          && original[j]->root_term()->get_parent() && original[j]->root_term()->get_parent()->get_name() == "expr"
          && modified[i]->root_term()->get_parent() && modified[i]->root_term()->get_parent()->get_name() == "expr"
          && ((original.size() - 0) > 1 || (modified.size() - 0) > 1)) {
            ++is_name_nest_modified;
        }

        if(!bool(pos_modified) || measure.similarity() > similarity_modified) {

          ++nest_count_modified;
          pos_modified = i;
          similarity_modified = measure.similarity();
          difference_modified = measure.difference();

        }

      }

    }

  }

  if(nest_count_modified > 1 && is_name_nest_modified > 1) {
    nest_count_modified = 0;
    pos_modified = std::optional<int>();
    similarity_modified = std::optional<int>();
    difference_modified = std::optional<int>();
  }

  // does not signal end this way
  if(nest_count_original == 0 && nest_count_modified == 0) return nest_result();
  if(((original.size()) > 1 || (modified.size()) > 1)
    &&  (((original.size()) == 1 && nest_count_original != 0)
      || ((modified.size()) == 1 && nest_count_modified != 0)))
    return nest_result();

  int start_nest_original = 0;  
  int end_nest_original = 0;
  int start_nest_modified = 0;  
  int end_nest_modified = 0;
  int operation = SES_COMMON;
  if(bool(pos_original) && (!bool(pos_modified) || *similarity_original > *similarity_modified
    || (*similarity_original == *similarity_modified && *difference_original <= *difference_modified))) {

      start_nest_original = *pos_original;
      end_nest_original   = *pos_original + 1;
      end_nest_modified   = 1;
      operation = SES_DELETE;

  } else if(bool(pos_modified)) {

      end_nest_original   = 1;
      start_nest_modified = *pos_modified;
      end_nest_modified   = *pos_modified + 1;
      operation = SES_INSERT;

  }

  return nest_result(start_nest_original, end_nest_original,
                     start_nest_modified, end_nest_modified,
                     operation);

}

bool srcdiff_nested::check_nestable_predicate(construct::construct_list_view construct_list_outer,
                                              construct::construct_list_view construct_list_inner) {

  if(construct_list_inner[0]->root_term()->get_move()) return true;

  if(!construct_list_outer[0]->can_nest(*construct_list_inner[0]))
    return true;

  std::shared_ptr<const construct> best_match = construct_list_outer[0]->find_best_descendent(*construct_list_inner[0]);
  if(!best_match) return true;

  srcdiff_text_measure measure(*best_match, *construct_list_inner[0]);
  measure.compute();

  if(!best_match->check_nest(*construct_list_inner[0]))
    return true;

  if(is_better_nest(construct_list_inner[0], construct_list_outer[0], measure))
    return true;

  if(construct_list_outer.size() > 1 && is_better_nest(construct_list_outer[1], construct_list_inner[0], measure))
    return true;

  if(construct_list_inner.size() > 1 && is_better_nest(construct_list_inner[1], construct_list_outer[0], measure))
    return true;

  if(construct_list_inner[0]->root_term_name() == "name"
    && construct_list_inner[0]->root_term()->get_parent() && construct_list_inner[0]->root_term()->get_parent()->get_name() == "expr"
    && construct_list_outer[0]->root_term()->get_parent() && construct_list_outer[0]->root_term()->get_parent()->get_name() == "expr"
    && (construct_list_outer.size() > 1 || construct_list_inner.size() > 1))
    return true;

  if(construct_list_inner[0]->root_term_name() == "name") {

      if(!construct_list_inner[0]->root_term()->get_parent() || !best_match->root_term()->get_parent())
        return true;

       if(!static_cast<const name_t&>(*best_match).check_nest(*construct_list_inner[0]))
        return true;

  }

  return false;


}

/**
 *
 * @todo need to make this more robust using dynamic programming.
 * Probably need one for both ways. Collect all statements of type one, and all the possible matches (statements of those types in order)
 * and run dynamic programming algorithm.  Run both ways and use number matched and similarity to choose best.
 *
 */

std::tuple<std::vector<int>, int, int> srcdiff_nested::check_nestable_inner(construct::construct_list_view parent_list, construct::construct_list_view child_list) {

  for(std::size_t i = 0; i < parent_list.size(); ++i) {

    if(parent_list[i]->root_term()->get_move()) continue;

    for(std::size_t j = 0; j < child_list.size(); ++j) {

      if(check_nestable_predicate(parent_list.subspan(i, parent_list.size() - i),
                                  child_list.subspan(j, child_list.size() - j))) {
        continue;
      }

      std::tuple<std::vector<int>, int, int> nestings = std::make_tuple(std::vector<int>(), i, i + 1);
      std::get<0>(nestings).push_back(j);
      for(std::size_t k = j + 1; k < child_list.size(); ++k) {

        if(check_nestable_predicate(parent_list.subspan(i, parent_list.size() - i),
                                    child_list.subspan(k, child_list.size() - k))) {
          continue;
        }

        std::get<0>(nestings).push_back(k);

      }

      return nestings;
    }
  }

  return std::make_tuple(std::vector<int>(), 0, 0);

}

nest_result srcdiff_nested::check_nestable(construct::construct_list_view original, construct::construct_list_view modified) {

  if(original.size() == 1 || modified.size() == 1) {

    if(nest_result nesting = check_nested_single_to_many(original, modified)) {
      return nesting;
    }

  }

  int start_nest_original = 0;  
  int end_nest_original = 0;
  int start_nest_modified = 0;  
  int end_nest_modified = 0;
  int operation = SES_COMMON;

  std::tuple<std::vector<int>, int, int> original_check = check_nestable_inner(original, modified);

  const std::vector<int> & valid_nests_original = std::get<0>(original_check);
  if(valid_nests_original.size()) {
    start_nest_original = std::get<1>(original_check);  
    end_nest_original  = std::get<2>(original_check); 
  }

  std::tuple<std::vector<int>, int, int> modified_check = check_nestable_inner(modified, original);

  const std::vector<int> & valid_nests_modified = std::get<0>(modified_check);
  if(valid_nests_modified.size()) {
    start_nest_modified = std::get<1>(modified_check);  
    end_nest_modified  = std::get<2>(modified_check); 
  }
  /** @todo may need a more exact check to pick most optimal or another check 

    For now if only valid, less than or equal and do not cross, or cross and larger.

  */
  if(!valid_nests_original.empty() && (valid_nests_modified.empty()
   || (start_nest_original < valid_nests_modified.front() && valid_nests_original.back() < start_nest_modified)
   || (!(start_nest_original > valid_nests_modified.back() && valid_nests_original.front() > start_nest_modified)
      && (valid_nests_original.back() - valid_nests_original.front()) >= (valid_nests_modified.back() - valid_nests_modified.front())))) {

      start_nest_modified = valid_nests_original.front();
      end_nest_modified = valid_nests_original.back() + 1;
      operation = SES_DELETE;

  } else if(!valid_nests_modified.empty()) {

      start_nest_original = valid_nests_modified.front();
      end_nest_original = valid_nests_modified.back() + 1;
      operation = SES_INSERT;

  }

  return nest_result(start_nest_original, end_nest_original,
                     start_nest_modified, end_nest_modified,
                     operation);

}

void srcdiff_nested::output() {

  construct::construct_list_view outer;
  construct::construct_list_view inner;

  if(operation == SES_DELETE) {
    outer = original;
    inner = modified;
  } else {
    outer = modified;
    inner = original;
  }

  srcdiff_whitespace whitespace(*out);

  whitespace.output_prefix();


  std::size_t start_pos = outer.front()->get_terms().at(1);
  std::size_t end_pos = outer.back()->end_position();

  const std::string & structure_outer = outer.front()->root_term_name();
  if(structure_outer == "block_content") {
    // do not skip whitespace
    start_pos = outer.front()->start_position() + 1;

  } else if(structure_outer == "if" && !bool(outer.front()->root_term()->get_attribute("type"))) {

    advance_to_child(outer.back()->nodes(), start_pos, srcML::node_type::START, "block");

  } else if(structure_outer == "while") {

    advance_to_child(outer.back()->nodes(), start_pos, srcML::node_type::END, "condition");
    ++start_pos;

  } else if(structure_outer == "for") {

    advance_to_child(outer.back()->nodes(), start_pos, srcML::node_type::END, "control");
    ++start_pos;

  } else if(is_class_type(structure_outer)) {

    advance_to_child(outer.back()->nodes(), start_pos, srcML::node_type::START, "block");
    ++start_pos;

    end_pos = start_pos - 1;
    advance_to_child(outer.back()->nodes(), end_pos, srcML::node_type::END, "block");

  }

  construct::construct_list set = outer.back()->get_descendents(start_pos, end_pos);

  construct::construct_list nest_set;

  for(std::size_t i = 0; i < inner.size(); ++i) {
      nest_set.push_back(inner[i]);
  }

  if(operation == SES_DELETE) {
    srcdiff_change::output_change(out, start_pos, out->last_output_modified());
  }
  else {
    srcdiff_change::output_change(out, out->last_output_original(), start_pos);
  }

  if(structure_outer == "block_content") {
    whitespace.output_prefix();
  } else {
    whitespace.output_nested(operation);
  }

  if(operation == SES_DELETE) {

    srcdiff_diff diff(out, set, nest_set);
    diff.output();

  } else {

    srcdiff_diff diff(out, nest_set, set);
    diff.output();

  }

  if(structure_outer == "block_content") {
    whitespace.output_prefix();
  } else {
    whitespace.output_nested(operation);
  }

  if(operation == SES_DELETE) {
    srcdiff_change::output_change(out, outer.back()->end_position() + 1, out->last_output_modified());
  }
  else {
    srcdiff_change::output_change(out, out->last_output_original(), outer.back()->end_position() + 1);
  }

}
