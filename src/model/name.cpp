/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <name.hpp>

#include <srcdiff_text_measure.hpp>

std::string name_t::simple_name() const {
    if(root_term()->is_simple()) return to_string();

    const construct_list childs = children();
    if(childs.empty() || childs[0]->root_term_name() != "name") return "";

    return childs[0]->to_string();
}

bool name_t::check_nest_name(const construct & modified, bool find_name_parent) const {

  std::shared_ptr<srcML::node> parent_original = parent_term();
  std::shared_ptr<srcML::node> parent_modified = modified.parent_term();

  if(find_name_parent) {
      while(parent_original && parent_original->get_name() == "name") {
        parent_original = parent_original->get_parent();
      }

      while(parent_modified && parent_modified->get_name() == "name") {
        parent_modified = parent_modified->get_parent();
      }
  }

  bool is_call_name_original = parent_original && parent_original->get_name() == "call";
  bool is_expr_name_original = parent_original && parent_original->get_name() == "expr";
  // java does not have an expr in generics
  bool is_argument_name_original = parent_original && parent_original->get_name() == "argument";
  bool is_type_name_original = parent_original && parent_original->get_name() == "type";

  bool is_call_name_modified = parent_modified && parent_modified->get_name() == "call";
  bool is_expr_name_modified = parent_modified && parent_modified->get_name() == "expr";
  // java does not have an expr in generics
  bool is_argument_name_modified = parent_modified && parent_modified->get_name() == "argument";
  bool is_type_name_modified = parent_modified && parent_modified->get_name() == "type";

  if(is_type_name_original && (is_expr_name_modified || is_argument_name_modified))
    return true;
  if(is_type_name_modified && (is_expr_name_original || is_argument_name_original))
    return true;

  if(is_call_name_original && is_expr_name_modified) {

      srcdiff_text_measure measure(*this, modified);
      int count = measure.number_match_beginning();
      return 2 * count >= measure.max_length();

  }

  if(is_call_name_modified && is_expr_name_original) {

      srcdiff_text_measure measure(*this, modified);
      int count = measure.number_match_beginning();
      return 2 * count >= measure.max_length();

  }

  return parent_original->get_name() == parent_modified->get_name();
}

bool name_t::can_nest(const construct & modified) const {
    if(root_term()->is_simple() != modified.root_term()->is_simple()
       && !check_nest_name(modified, false)) return false;

    return is_similar(modified);
}

bool name_t::is_matchable_impl(const construct & modified) const {

    if(root_term()->is_simple() && modified.root_term()->is_simple()) return true;

    return false;
}
