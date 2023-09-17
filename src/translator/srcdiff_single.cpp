/**
 * @file srcdiff_single.cpp
 *
 * @copyright Copyright (C) 2014-2023 srcML, LLC. (www.srcML.org)
 *
 * srcDiff is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * srcDiff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <srcdiff_single.hpp>

#include <srcdiff_constants.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_comment.hpp>
#include <srcdiff_common.hpp>
#include <srcdiff_whitespace.hpp>
#include <shortest_edit_script.h>

#include <cstring>
#include <map>
#include <list>

const std::string convert("convert");
const srcml_attribute diff_convert_type("type", DIFF_NAMESPACE, convert);

srcdiff_single::srcdiff_single(std::shared_ptr<srcdiff_output> out, const std::shared_ptr<construct> & original_construct, const std::shared_ptr<construct> & modified_construct) 
  : out(out), original_construct(original_construct), modified_construct(modified_construct) {}

static srcML::node::srcml_attribute_map merge_attributes(const srcML::node::srcml_attribute_map & attributes_original, const srcML::node::srcml_attribute_map & attributes_modified) {

  srcML::node::srcml_attribute_map attributes;

  srcML::node::srcml_attribute_map_citr citr_original = attributes_original.begin();
  srcML::node::srcml_attribute_map_citr citr_modified = attributes_modified.begin();

  while(citr_original != attributes_original.end() && citr_modified != attributes_modified.end()) {

    if(citr_original->first == citr_modified->first) {

      if(citr_original->second.get_value() == citr_modified->second.get_value()) {
        attributes.emplace(citr_original->first, citr_original->second);
      }
      else {
        attributes.emplace(citr_original->first, srcml_attribute(citr_original->second.get_name(), citr_original->second.get_ns(), *citr_original->second.get_value() + std::string("|") + *citr_modified->second.get_value()));
      }

      ++citr_original;  
      ++citr_modified;

    } else {

      ++citr_modified;

      bool is_end = citr_modified == attributes_modified.end();
      std::string name;
      if(!is_end) name = citr_modified->second.get_name();

      --citr_modified;

      if(!is_end && citr_original->second.get_name() == name) {

        attributes.emplace(citr_modified->second.get_name(), srcml_attribute(citr_modified->second.get_name(), citr_modified->second.get_ns(), std::string("|") + *citr_modified->second.get_value()));

        ++citr_modified;

      } else {

        attributes.emplace(citr_original->second.get_name(), srcml_attribute(citr_original->second.get_name(), citr_original->second.get_ns(), *citr_original->second.get_value() + std::string("|")));

        ++citr_original;

      }

    }

  }

  while(citr_original != attributes_original.end()) {

      attributes.emplace(citr_original->first, srcml_attribute(citr_original->second.get_name(), citr_original->second.get_ns(), *citr_original->second.get_value() + std::string("|")));

      ++citr_original;

  }

  while(citr_modified != attributes_modified.end()) {

      attributes.emplace(citr_modified->first, srcml_attribute(citr_modified->second.get_name(), citr_modified->second.get_ns(), std::string("|") + *citr_modified->second.get_value()));

      ++citr_modified;

  }

  return attributes;

}

void srcdiff_single::output_recursive_same() {

  srcdiff_whitespace whitespace(*out);
  whitespace.output_all();

  if(original_construct->root_term()->is_temporary() == modified_construct->root_term()->is_temporary()) {
    out->output_node(out->diff_common_start, SES_COMMON);
  }

  if(*original_construct->root_term() == *modified_construct->root_term()) {

    out->output_node(original_construct->root_term(), modified_construct->root_term(), SES_COMMON);

  } else {

    std::shared_ptr<srcML::node> merged_node = std::make_shared<srcML::node>(*original_construct->root_term());
    merged_node->set_empty(original_construct->root_term()->is_empty() && modified_construct->root_term()->is_empty());
    merged_node->set_attributes(merge_attributes(original_construct->root_term()->get_attributes(),
                                                 modified_construct->root_term()->get_attributes()));
    out->output_node(merged_node, SES_COMMON);

  }

  ++out->last_output_original();
  ++out->last_output_modified();

  // diff comments differently then source-code
  if(original_construct->root_term()->get_name() == "comment") {

    // collect subset of nodes
    construct::construct_list children_original = original_construct->children();

    construct::construct_list children_modified = modified_construct->children();

    srcdiff_comment diff(out, children_original, children_modified);
    diff.output();

  } else {

      // collect subset of nodes
      construct::construct_list children_original;
      if(!original_construct->root_term()->is_empty())
        children_original = original_construct->children();

      construct::construct_list children_modified;
      if(!modified_construct->root_term()->is_empty())
        children_modified =  modified_construct->children();

      srcdiff_diff diff(out, children_original, children_modified);
      diff.output();

  }

  srcdiff_common::output_common(out, original_construct->end_position() + 1, modified_construct->end_position() + 1);

  if(original_construct->root_term()->is_temporary() == modified_construct->root_term()->is_temporary()) {
    out->output_node(out->diff_common_end, SES_COMMON);
  }

}

void srcdiff_single::output_recursive_interchangeable() {

  srcdiff_whitespace whitespace(*out);
  whitespace.output_all();

  const std::shared_ptr<srcML::node> & original_start_node = original_construct->root_term();
  const std::shared_ptr<srcML::node> & modified_start_node = modified_construct->root_term();

  int original_collect_start_pos = 1;
  if(original_start_node->get_name() == "if_stmt") {
    // must have if, if interchange passed
    while(original_construct->term(original_collect_start_pos)->get_name() != "if") {
      ++original_collect_start_pos;
    }
    ++original_collect_start_pos;
  }

  int modified_collect_start_pos = 1;
  if(modified_start_node->get_name() == "if_stmt") {
    // must have if, if interchange passed
    while(modified_construct->term(modified_collect_start_pos)->get_name() != "if") {
      ++modified_collect_start_pos;
    }
    ++modified_collect_start_pos;
  }

  // get keyword if present
  const std::shared_ptr<srcML::node> & keyword_node_original = original_construct->term(original_collect_start_pos);
  const std::shared_ptr<srcML::node> & keyword_node_modified = modified_construct->term(modified_collect_start_pos);
  bool is_keyword  = keyword_node_original->is_text() && !keyword_node_original->is_whitespace();
  bool is_keywords = is_keyword
                     && keyword_node_modified->is_text() && !keyword_node_modified->is_whitespace();
  bool is_same_keyword = is_keywords && *keyword_node_original == *keyword_node_modified;


  if((is_keyword && !is_keywords) || (is_keywords && !is_same_keyword)) {
    ++original_collect_start_pos;
  }

  // output deleted nodes
  out->diff_original_start->emplace_attribute("type", diff_convert_type);
  out->output_node(out->diff_original_start, SES_DELETE, true);
  out->diff_original_start->clear_attributes();

  for(int output_pos = 0; output_pos < original_collect_start_pos; ++output_pos) {
    out->output_node(original_construct->term(output_pos), SES_DELETE);
    ++out->last_output_original();
  }

  // output inserted nodes
  out->diff_modified_start->emplace_attribute("type", diff_convert_type);
  out->output_node(out->diff_modified_start, SES_INSERT, true);
  out->diff_modified_start->clear_attributes();

  if(is_keywords && !is_same_keyword){
    ++modified_collect_start_pos;
  }

  for(int output_pos = 0; output_pos < modified_collect_start_pos; ++output_pos) {
    out->output_node(modified_construct->term(output_pos), SES_INSERT);
    ++out->last_output_modified();
  }

  // collect subset of nodes
  construct::construct_list next_set_original
    = construct::get_descendent_constructs(out->nodes_original(), original_construct->get_terms().at(original_collect_start_pos)
                      , original_construct->end_position());

  construct::construct_list next_set_modified
    = construct::get_descendent_constructs(out->nodes_modified(), modified_construct->get_terms().at(modified_collect_start_pos)
                      , modified_construct->end_position());

  srcdiff_diff diff(out, next_set_original, next_set_modified);
  diff.output();

  srcdiff_whitespace::output_whitespace(out);

  srcdiff_change::output_change(out, out->last_output_original(), modified_construct->end_position() + 1);

  out->output_node(out->diff_modified_end, SES_INSERT, true);
  if(out->output_state() == SES_INSERT) {
    out->output_node(out->diff_modified_end, SES_INSERT);
  }

  srcdiff_change::output_change(out, original_construct->end_position() + 1, out->last_output_modified());

  out->output_node(out->diff_original_end, SES_DELETE, true);
  if(out->output_state() == SES_DELETE) {
      out->output_node(out->diff_original_end, SES_DELETE);
  }

}

void srcdiff_single::output() {

  const std::shared_ptr<srcML::node> & start_node_original = original_construct->root_term();
  const std::shared_ptr<srcML::node> & start_node_modified = modified_construct->root_term();

  if(start_node_original->get_name() == start_node_modified->get_name()
    && start_node_original->get_namespace()->get_uri() == start_node_modified->get_namespace()->get_uri()) {
    output_recursive_same();
  } else {
    output_recursive_interchangeable();
  }

}
