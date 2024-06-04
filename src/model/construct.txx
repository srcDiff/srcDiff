/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2024-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

template <class nest_rule_checker, class convert_rule_checker>
construct::construct(const construct* parent, std::size_t& start,
          std::shared_ptr<nest_rule_checker>    nest_checker,
          std::shared_ptr<convert_rule_checker> convert_checker)

  : nest_checker(nest_checker ? nest_checker : std::make_shared<nest_rule_checker>(*this)),
    convert_checker(convert_checker ? convert_checker : std::make_shared<convert_rule_checker>(*this)),
    out(parent->output()), node_list(parent->nodes()), hash_value(), parent_construct(parent) {

  if(node_list.at(start)->get_type() != srcML::node_type::TEXT && node_list.at(start)->get_type() != srcML::node_type::START) return;

  terms.push_back(start);

  if(node_list.at(start)->is_empty() || node_list.at(start)->get_type() == srcML::node_type::TEXT) return;

  ++start;

  // track open tags because could have same type nested
  int is_open = 1;
  for(; is_open; ++start) {

    // skip whitespace
    if(node_list.at(start)->is_whitespace()) {
      continue;
    }

    terms.push_back(start);

    // opening tags
    if(node_list.at(start)->get_type() == srcML::node_type::START
       && !(node_list.at(start)->is_empty())) {
      ++is_open;
    }

    // closing tags
    else if(node_list.at(start)->get_type() == srcML::node_type::END) {
      --is_open;
    }
  }

  --start;
}

template <class nest_rule_checker, class convert_rule_checker>
void construct::set_rule_checkers() {
  nest_checker = std::make_shared<nest_rule_checker>(*this);
  convert_checker = std::make_shared<convert_rule_checker>(*this);
}
