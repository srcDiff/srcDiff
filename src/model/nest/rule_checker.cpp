// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file rule_checker.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include "rule_checker.hpp"

#include <construct.hpp>
#include <srcdiff_text_measure.hpp>

namespace nest {

rule_checker::rule_checker(const construct& client, const string_set& nestable_constructs)
    : client(client), nestable_constructs(nestable_constructs) {}

bool rule_checker::check_nest(const construct & modified) const {
  return client.can_refine_difference(modified);
}

bool rule_checker::can_nest(const construct& modified) const {
    if(*client.root_term() == *modified.root_term())
        return can_nest_same(modified);
    else
        return can_nest_internal(modified);
}

bool rule_checker::can_nest_internal(const construct& modified) const {
  return nestable_constructs.find(modified.root_term_name()) != nestable_constructs.end();;
}

bool rule_checker::can_nest_same(const construct& modified) const {

  if(!can_nest_internal(modified)) return false;

  std::shared_ptr<const construct> best_match = client.find_best_descendent(modified);
  if(!best_match) return false;

  srcdiff_text_measure match_measure(*best_match, modified);
  match_measure.compute();

  srcdiff_text_measure measure(client, modified);
  measure.compute();

  double min_size = measure.min_length();
  double match_min_size = std::min(measure.original_length(), match_measure.modified_length());

  return (match_measure.similarity() >= measure.similarity() && match_measure.difference() <= measure.difference()) 
  || (match_min_size > 50 && min_size > 50 && (match_min_size / match_measure.similarity()) < (0.9 * (min_size / measure.similarity()))
    && best_match->check_nest(modified));

}

}
