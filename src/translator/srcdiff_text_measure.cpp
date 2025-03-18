// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_text_measure.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_text_measure.hpp>


#include <srcdiff_shortest_edit_script.hpp>
#include <srcdiff_constants.hpp>
#include <srcdiff_diff.hpp>

#include <algorithm>
#include <cassert>

srcdiff_text_measure::srcdiff_text_measure(const construct & set_original, const construct & set_modified, bool important_only) 
  : srcdiff_measure(set_original, set_modified),
    set_original_text(std::make_shared<construct>(set_original.nodes())),
    set_modified_text(std::make_shared<construct>(set_modified.nodes())),
    important_only(important_only),
    text_collected(false) {}

void srcdiff_text_measure::collect_text() {

  text_collected = true;

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

  for(unsigned int i = 0; i < olength; ++i) {
    if(set_original.term(i)->is_text() && !set_original.term(i)->is_whitespace()) {
      set_original_text->get_terms().push_back(set_original.get_terms().at(i));
    }
  }

  for(unsigned int i = 0; i < nlength; ++i) {
    if(set_modified.term(i)->is_text() && !set_modified.term(i)->is_whitespace()) {
      set_modified_text->get_terms().push_back(set_modified.get_terms().at(i));
    }
  }

  original_len = set_original_text->size();
  modified_len = set_modified_text->size();

}

void srcdiff_text_measure::collect_text_element(const construct & set, construct & set_text) {

  std::size_t length = set.size();

  for(unsigned int i = 0; i < length; ++i) {

    if(set.term(i)->get_name() == "operator"
      || set.term(i)->get_name() == "modifier") {

      if(set.term(i)->get_parent() && set.term(i)->get_parent()->get_name() != "name") continue;
      // comparison of signed vs unsigned
      if((std::size_t)(set.get_terms().at(i) + 1) < set.nodes().size() && set.nodes().at(set.get_terms().at(i) + 1)->is_text()
        && (*set.nodes().at(set.get_terms().at(i) + 1)->get_content() == "::")) continue;

      while(set.term(i)->get_type() != srcML::node_type::END) {
        ++i;
      }

    }

    const std::shared_ptr<srcML::node> & node = set.term(i);

    bool is_text = node->is_text() && !node->is_whitespace() && node->get_content();
    bool is_operator = node->get_parent() && node->get_parent()->get_name() == "operator";

    if(is_operator && is_text
        && (*node->get_content() == "."
          || *node->get_content() == "->"
          || *node->get_content() == ".*"
          || *node->get_content() == "->*"))
        continue;

    if(is_text 
      && (is_operator
        ||   (*node->get_content() != "("
          && *node->get_content() != ")"
          && *node->get_content() != "{"
          && *node->get_content() != "}"
          && *node->get_content() != "["
          && *node->get_content() != "]"
          && *node->get_content() != ":"
          && *node->get_content() != ";"
          && *node->get_content() != ",")))
      set_text.get_terms().push_back(set.get_terms().at(i));

  }


}
 
void srcdiff_text_measure::collect_important_text() {

  text_collected = true;

  collect_text_element(set_original, *set_original_text);
  collect_text_element(set_modified, *set_modified_text);

  original_len = set_original_text->size();
  modified_len = set_modified_text->size();

}

/// @todo this is not really used, and it seems like it was always wrong even before
/// construct refactoring
void srcdiff_text_measure::unigrams(construct & collected_set_original,
                                    construct & collected_set_modified) {

  std::sort(collected_set_original.get_terms().begin(), collected_set_original.get_terms().end());
  std::sort(collected_set_modified.get_terms().begin(), collected_set_modified.get_terms().end());

  int i = 0, j = 0;
  while(i < original_len && j < modified_len) {

   if(collected_set_original.get_terms().at(i) == collected_set_modified.get_terms().at(j)) {

     ++a_similarity;
     ++i;
     ++j;

   } else {

     if(collected_set_original.get_terms().at(i) < collected_set_modified.get_terms().at(j)) {
       ++i;
       ++a_original_difference;      
     }
     else {
       ++j;
       ++a_modified_difference;      
     }

    }

  }

}

void srcdiff_text_measure::compute() {

  if(computed) return;

  computed = true;

  if(set_original.term(0)->get_type() != srcML::node_type::START
     || set_modified.term(0)->get_type() != srcML::node_type::START
     || (*set_original.term(0) != *set_modified.term(0)
        && !set_original.is_tag_convertable(set_modified)
        && (set_original.term(0)->get_name() != "block" || set_modified.term(0)->get_name() != "block"))) {

    a_similarity = MAX_INT;
    a_original_difference = MAX_INT;
    a_modified_difference = MAX_INT;

    return;

  }

  if(!text_collected) {

    if(important_only) {
      collect_important_text();
    }
    else {
      collect_text();
    }

  }

    srcdiff_shortest_edit_script ses;
    ses.compute_edit_script(set_original_text, set_modified_text);
    process_edit_script(ses.script());

}

int srcdiff_text_measure::number_match_beginning() {

      collect_important_text();
      computed = true;

      std::size_t count = 0;
      while((count < set_original_text->size() && count < set_modified_text->size())
        && *set_original_text->term(count) == *set_modified_text->term(count)) {
        ++count;
      }

      return count;

}
