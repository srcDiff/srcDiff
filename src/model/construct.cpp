// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file construct.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <construct.hpp>

#include <srcml_nodes.hpp>
#include <construct_factory.hpp>
#include <text_measurer.hpp>
#include <syntax_measurer.hpp>

#include <algorithm>
#include <iostream>


bool construct::is_non_white_space(std::size_t & node_pos, const srcml_nodes & node_list, const void * context [[maybe_unused]]) {

    const std::shared_ptr<srcML::node> & node = node_list[node_pos];

    // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
    return node->get_type() == srcML::node_type::START || (node->get_type() == srcML::node_type::TEXT && node->get_content() && !node->is_whitespace());

}

bool construct::is_match(std::size_t & node_pos, const srcml_nodes & nodes, const void * context) {

  const std::shared_ptr<srcML::node> & node = nodes[node_pos];
  const std::shared_ptr<srcML::node> & context_node = *(const std::shared_ptr<srcML::node> *)context;

  return (xmlReaderTypes)node->get_type() == XML_READER_TYPE_ELEMENT && *node == *context_node;

}

/// @todo make member.  Requires modifiying a lot of methods in other classes.
// name does not quite match because not a member yet.
construct::construct_list construct::get_descendents(std::size_t start_pos, std::size_t end_pos,
                                                     construct_filter filter, const void * context) const {
    construct::construct_list descendents;

    // runs on a subset of base array
    for(std::size_t pos = start_pos; pos < end_pos; ++pos) {

        // skip whitespace
        if(filter(pos, node_list, context)) {

            // text is separate node if not surrounded by a tag in range
            if(node_list.at(pos)->get_type() == srcML::node_type::TEXT || node_list.at(pos)->get_type() == srcML::node_type::START) {
                descendents.push_back(create_construct(this, pos));
            } else {
                return descendents;
            }

        }

    }
    return descendents;
}

construct::construct(const srcml_nodes & node_list, std::shared_ptr<srcdiff::output_stream> out)
    : out(out), node_list(node_list), terms(), hash_value(),
      nest_checker(), convert_checker() {
}

construct::construct(const construct* parent, std::size_t& start)
  : out(parent->output()), node_list(parent->nodes()), hash_value(), parent_construct(parent),
    nest_checker(), convert_checker() {

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

bool construct::operator==(const construct & that) const {

    if(!(hash() == that.hash())) return false;

    for(std::size_t i = 0, j = 0; i < size() && j < that.size();) {

      // string consecutive non whitespace text nodes
      if(term(i)->is_text() && that.term(j)->is_text()) {

        std::string text1 = "";
        for(; i < size() && term(i)->is_text(); ++i) {
          text1 += term(i)->get_content() ? *term(i)->get_content() : "";
        }

        std::string text2 = "";
        for(; j < that.size() && that.term(j)->is_text(); ++j) {
          text2 += that.term(j)->get_content() ? *that.term(j)->get_content() : "";
        }

        if(text1 != text2) return false;

      } else {

        if(*term(i) != *that.term(j)) return false;

        ++i;
        ++j;

      }
    }

    return true;
}

bool construct::operator!=(const construct & that) const {
    return !operator==(that);
}

std::ostream & operator<<(std::ostream & out, const construct & that) {

    for(std::size_t pos = 0, size = that.size(); pos < size; ++pos) {
        out << *that.term(pos);
    }

    return out;

}

const construct* construct::parent() const {
    return parent_construct;
}

const std::shared_ptr<srcdiff::output_stream> construct::output() const {
    return out;
}

std::shared_ptr<srcdiff::output_stream> construct::output() {
    return out;
}

void construct::expand_children() const {
    child_constructs = get_descendents(start_position() + 1, end_position(), is_non_white_space);
}

const construct::construct_list & construct::children() const {
    if(!child_constructs) expand_children();
    return *child_constructs;
}

construct::construct_list & construct::children() {
    if(!child_constructs) expand_children();
    return *child_constructs;
}


/// term access api ///
std::size_t construct::size() const {
    return terms.size();
}

bool construct::empty() const {
    return terms.empty();
}

const std::shared_ptr<srcML::node> & construct::term(std::size_t pos) const {
    assert(pos < terms.size());
    return node_list[terms[pos]];
}

const std::vector<int> & construct::get_terms() const {
    return terms;
}

// @todo possibly remove this
std::vector<int> & construct::get_terms() {
    return terms;
}

const std::shared_ptr<srcML::node> & construct::last_term() const {
    assert(!node_list.empty());
    return node_list[terms.back()];
} 

/// position info of element
int construct::start_position() const {
    assert(!terms.empty());
    return terms.front();
}

int construct::end_position() const {
    assert(!terms.empty());
    return terms.back();
}



const srcml_nodes & construct::nodes() const {
    return node_list;
}

std::size_t construct::hash() const {
    if(!hash_value) hash_value = std::hash<construct>()(*this);
    return *hash_value;
}

std::string construct::to_string(bool skip_whitespace) const {

    std::string str;
    for(size_t pos = start_position(); pos < end_position(); ++pos) {
        std::shared_ptr<const srcML::node> node = node_list[pos];
        if(skip_whitespace && node->is_whitespace()) continue;
        if(!node->get_content()) continue;
        str += *node->get_content();
    }
    return str;
}

const std::shared_ptr<srcML::node> & construct::root_term() const {
    return term(0);
}

const std::string & construct::term_name(std::size_t pos) const {
    return term(pos)->get_name();
}
const std::string & construct::root_term_name() const {
    return term_name(0);
}

const std::shared_ptr<srcML::node> construct::parent_term() const {
    return term(0)->get_parent();
}
const std::string & construct::parent_term_name() const {
    return term(0)->get_parent()->get_name();
}

std::shared_ptr<const construct> construct::find_child(const std::string & name) const {
    std::shared_ptr<const construct> found_child;
    for(std::shared_ptr<const construct> child : children()) {
        if(child->root_term_name() == name) {
            found_child = child;
            break;
        }
    }
    return found_child;
}

construct::construct_list construct::find_descendents(std::shared_ptr<srcML::node> element) const {
    return get_descendents(start_position() + 1, end_position(), construct::is_match, &element);
}

std::shared_ptr<const construct> construct::find_best_descendent(const construct& match_construct) const {
    construct::construct_list descendents = find_descendents(match_construct.term(0));

    std::shared_ptr<const construct> best_descendent;
    for(std::shared_ptr<const construct> descendent : descendents) {

        size_t max_size = std::max(descendent->terms.size(), match_construct.terms.size());
        size_t min_size = std::min(descendent->terms.size(), match_construct.terms.size());

        if(max_size > (4 * min_size)) {
          continue;
        }

        if(!best_descendent || match_construct.measure(*descendent)->similarity() > match_construct.measure(*best_descendent)->similarity()) {
            best_descendent = descendent;
        }

    }

    return best_descendent;
}


const std::shared_ptr<srcdiff::measurer> & construct::measure(const construct & modified) const {
    std::unordered_map<int, std::shared_ptr<srcdiff::measurer>>::const_iterator citr = measures.find(modified.start_position());
    if(citr != measures.end()) return citr->second;

    std::shared_ptr<srcdiff::measurer> similarity = std::make_shared<srcdiff::text_measurer>(*this, modified);
    similarity->compute();

    citr = measures.insert(citr, std::pair(modified.start_position(), similarity));
    return citr->second;
}

bool construct::is_similar(const construct & modified) const {
    return is_text_similar(modified) || is_syntax_similar(modified);
}

bool construct::is_text_similar(const construct & modified) const {

 const srcdiff::measurer & measure = *this->measure(modified);

  int min_size = measure.min_length();
  int max_size = measure.max_length();

  /** @todo consider making this configurable.  That is, allow user to specify file or have default file to read from */
  if(measure.difference() != 0 && measure.similarity() == 0) return false;

  if(min_size == measure.similarity() && measure.difference() < 2 * min_size) return true;
  if(min_size < 30 && measure.difference() > 1.25 * min_size)                 return false;
  if(min_size >= 30 
    && measure.original_difference() > 0.25 * measure.original_length()
    && measure.modified_difference() > 0.25 * measure.modified_length())      return false;
  if(measure.difference() > max_size)                                         return false;

  if(min_size <= 2)  return 2  * measure.similarity() >=     min_size;
  if(min_size <= 3)  return 3  * measure.similarity() >= 2 * min_size;
  if(min_size <= 30) return 10 * measure.similarity() >= 7 * min_size;
  return 2  * measure.similarity() >=     min_size;
}

bool construct::is_syntax_similar(const construct & modified) const {

  const std::string & original_tag = root_term_name();
  const std::string & modified_tag = modified.root_term_name();

  if(size() == 1 && modified.size() == 1) {
    return original_tag == modified_tag;
  }

  if(size() == 1 || modified.size() == 1) {

    /** @todo need to handle this some time */
    return false;

  }

  srcdiff::syntax_measurer syntax_measure(*this, modified);
  syntax_measure.compute();

  int min_child_length = syntax_measure.min_length();
  //int max_child_length = syntax_measure.max_length();

  if(min_child_length > 1) { 
    if(2 * syntax_measure.similarity() >= min_child_length && syntax_measure.difference() <= min_child_length)
      return true;
  }

  if(original_tag != modified_tag) return false;
  return is_syntax_similar_impl(modified);
}

bool construct::is_syntax_similar_impl(const construct & modified [[maybe_unused]]) const {
    return false;
}

enum srcdiff::operation construct::can_refine_difference(const construct & modified) const {

  const std::string & original_tag = root_term_name();
  const std::string & modified_tag = modified.root_term_name();

  const std::string & original_uri = term(0)->get_namespace()->get_uri();
  const std::string & modified_uri = modified.term(0)->get_namespace()->get_uri();

  if(original_tag == modified_tag && original_uri == modified_uri) {
    return is_matchable(modified) ? srcdiff::MATCH : srcdiff::NONE;
  } else if(is_tag_convertable(modified)) {
    return is_convertable(modified) ? srcdiff::CONVERT : srcdiff::NONE;
  } else {
    return srcdiff::NONE;
  }

}

bool construct::is_matchable(const construct & modified) const {

  const std::string & original_tag = root_term_name();
  const std::string & modified_tag = modified.root_term_name();

  const std::string & original_uri = term(0)->get_namespace()->get_uri();
  const std::string & modified_uri = modified.term(0)->get_namespace()->get_uri();

  if(original_uri != modified_uri) return false;
  if(original_tag != modified_tag) return false;
  if(is_matchable_impl(modified))  return true;

  return is_similar(modified);

}

bool construct::is_matchable_impl(const construct & modified [[maybe_unused]]) const {
    return false;
}

bool construct::is_match_similar(const construct & modified) const {

  if(*term(0) != *modified.term(0)) return false;

  srcdiff::text_measurer complete_measure(*this, modified, false);
  complete_measure.compute();
  int min_size = complete_measure.min_length();

  if(min_size == 0) return false;

  return min_size == complete_measure.similarity();

}

bool construct::is_tag_convertable(const construct & modified) const {
  return convert_checker->is_tag_convertable(modified);
}

bool construct::is_convertable(const construct & modified) const {
  return convert_checker->is_convertable(modified);
}

bool construct::can_nest(const construct & modified) const {
    return nest_checker->can_nest(modified);
}

bool construct::check_nest(const construct & modified) const {
  return nest_checker->check_nest(modified);
}
