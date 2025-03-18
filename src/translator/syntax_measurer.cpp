// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file syntax_measurer.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <syntax_measurer.hpp>

#include <shortest_edit_script.hpp>
#include <srcdiff_constants.hpp>

#include <algorithm>

namespace srcdiff {

syntax_measurer::syntax_measurer(const construct & set_original, const construct & set_modified) 
  : measurer(set_original, set_modified) {}

// context is unused here
static bool is_significant(std::size_t & node_pos, const srcml_nodes & nodes, const void * context [[maybe_unused]]) {

  const std::shared_ptr<srcML::node> & node = nodes[node_pos];

  if(node->get_name() == "argument_list") {

    std::size_t pos = node_pos + 1;
    while(pos < nodes.size() && (nodes[pos]->get_type() == srcML::node_type::START || nodes[pos]->get_name() != "argument_list")) {

      if(!nodes[pos]->is_text()) return true;

      ++pos;

    }

    node_pos = pos;

    return false;

  }

  return !node->is_text() && node->get_type() == srcML::node_type::START
    && node->get_name() != "operator" && node->get_name() != "literal" && node->get_name() != "modifier";

}

void syntax_measurer::compute() {

  if(computed) return;

  computed = true;
  
  if(set_original.term(0)->get_type() != srcML::node_type::START
     || set_modified.term(0)->get_type() != srcML::node_type::START
     || (*set_original.term(0) != *set_modified.term(0)
        && !set_original.is_tag_convertable(set_modified)
        && (set_original.term(0)->get_name() != "block" || set_modified.term(0)->get_name() != "block"))) {

    a_similarity = 0;
    a_original_difference = MAX_INT;
    a_modified_difference = MAX_INT;

    return;

  }

  // collect subset of nodes
  construct::construct_list next_construct_list_original = set_original.size() > 1 ? set_original.get_descendents(set_original.get_terms().at(1), set_original.end_position(), is_significant) : construct::construct_list();
  construct::construct_list next_construct_list_modified = set_modified.size() > 1 ? set_modified.get_descendents(set_modified.get_terms().at(1), set_modified.end_position(), is_significant) : construct::construct_list();
  original_len = next_construct_list_original.size();
  modified_len = next_construct_list_modified.size();

  shortest_edit_script ses;
  ses.compute_edit_script(next_construct_list_original, next_construct_list_modified);
  process_edit_script(ses.script());

}

}
