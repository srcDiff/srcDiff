/**
 * @file construct.cpp
 *
 * @copyright Copyright (C) 2023-2023 srcML, LLC. (www.srcML.org)
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

#include <construct.hpp>

#include <srcml_nodes.hpp>
#include <construct_factory.hpp>
#include <srcdiff_text_measure.hpp>
#include <srcdiff_syntax_measure.hpp>

//temp, probably
#include <srcdiff_match.hpp>
#include <srcdiff_nested.hpp>

#include <algorithm>
#include <iostream>

bool construct::is_non_white_space(int & node_pos, const srcml_nodes & node_list, const void * context) {

    const std::shared_ptr<srcml_node> & node = node_list[node_pos];

    // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
    return (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT || ((xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && !node->is_white_space());

}

bool construct::is_match(int & node_pos, const srcml_nodes & nodes, const void * context) {

  const std::shared_ptr<srcml_node> & node = nodes[node_pos];
  const std::shared_ptr<srcml_node> & context_node = *(const std::shared_ptr<srcml_node> *)context;

  return (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT && *node == *context_node;

}

/// @todo make member.  Requires modifiying a lot of methods in other classes.
// name does not quite match because not a member yet.
construct::construct_list construct::get_descendent_constructs(const srcml_nodes & node_list, 
                                                    std::size_t start_pos, std::size_t end_pos,
                                                    construct_filter filter,
                                                    const void * context,
                                                    std::shared_ptr<srcdiff_output> out) {
    construct::construct_list descendent_constructs;

    // runs on a subset of base array
    for(int pos = start_pos; pos < end_pos; ++pos) {

        // skip whitespace
        if(filter(pos, node_list, context)) {

            // text is separate node if not surrounded by a tag in range
            if((xmlReaderTypes)node_list.at(pos)->type == XML_READER_TYPE_TEXT || (xmlReaderTypes)node_list.at(pos)->type == XML_READER_TYPE_ELEMENT) {
                descendent_constructs.push_back(create_construct(node_list, pos, out));
            } else {
                return descendent_constructs;
            }

        }

    }
    return descendent_constructs;
}

construct::construct(const construct & that) : out(that.out), node_list(that.node_list), terms(), hash_value(that.hash_value) {

    for(std::size_t pos = 0; pos < that.size(); ++pos) {
        terms.push_back(that.terms[pos]);
    }

}

construct::construct(const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) : out(out), node_list(node_list), hash_value() {

  if((xmlReaderTypes)node_list.at(start)->type != XML_READER_TYPE_TEXT && (xmlReaderTypes)node_list.at(start)->type != XML_READER_TYPE_ELEMENT) return;

  terms.push_back(start);

  if(node_list.at(start)->is_empty || (xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_TEXT) return;

  ++start;

  // track open tags because could have same type nested
  int is_open = 1;
  for(; is_open; ++start) {

    // skip whitespace
    if(node_list.at(start)->is_white_space()) {
      continue;
    }

    terms.push_back(start);

    // opening tags
    if((xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_ELEMENT
       && !(node_list.at(start)->is_empty)) {
      ++is_open;
    }

    // closing tags
    else if((xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_END_ELEMENT) {
      --is_open;
    }

  }

  --start;
}

void construct::swap(construct & that) {
    std::swap(terms, that.terms);
    std::swap(hash_value, that.hash_value);
    std::swap(child_constructs, that.child_constructs);
}

construct & construct::operator=(construct that) {
    swap(that);
    return *this;
}

bool construct::operator==(const construct & that) const {

    if(!(hash() == that.hash())) return false;

    for(std::size_t i = 0, j = 0; i < size() && j < that.size();) {

      // string consecutive non whitespace text nodes
      if(term(i)->is_text() && that.term(j)->is_text()) {

        std::string text1 = "";
        for(; i < size() && term(i)->is_text(); ++i) {
          text1 += term(i)->content ? *term(i)->content : "";
        }

        std::string text2 = "";
        for(; j < that.size() && that.term(j)->is_text(); ++j) {
          text2 += that.term(j)->content ? *that.term(j)->content : "";
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

void construct::expand_children() const {
    child_constructs = get_descendent_constructs(node_list, start_position() + 1, end_position(), is_non_white_space, nullptr, out);
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

const std::shared_ptr<srcml_node> & construct::term(std::size_t pos) const {
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

const std::shared_ptr<srcml_node> & construct::last_term() const {
    assert(!node_list.empty());
    return node_list[terms.back()];
} 

/// position info of element
std::size_t construct::start_position() const {
    assert(!terms.empty());
    return terms.front();
}

std::size_t construct::end_position() const {
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
    for(int pos = start_position(); pos < end_position(); ++pos) {
        std::shared_ptr<const srcml_node> node = node_list[pos];
        if(skip_whitespace && node->is_white_space()) continue;
        if(!node->content) continue;
        str += *node->content;
    }
    return str;
}

const std::shared_ptr<srcml_node> & construct::root_term() const {
    return term(0);
}

const std::string & construct::term_name(std::size_t pos) const {
    return term(pos)->name;
}
const std::string & construct::root_term_name() const {
    return term_name(0);
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

construct::construct_list construct::find_descendents(std::shared_ptr<srcml_node> element) const {
    return get_descendent_constructs(node_list, start_position() + 1, end_position(), construct::is_match, &element, out);
}

std::shared_ptr<const construct> construct::find_best_descendent(std::shared_ptr<const construct> match_construct) const {
    construct::construct_list descendents = find_descendents(match_construct->term(0));

    std::shared_ptr<const construct> best_descendent;
    for(std::shared_ptr<const construct> descendent : descendents) {

        size_t max_size = std::max(descendent->terms.size(), match_construct->terms.size());
        size_t min_size = std::min(descendent->terms.size(), match_construct->terms.size());

        if(max_size > (4 * min_size)) {
          continue;
        }

        if(!best_descendent || match_construct->measure(*descendent)->similarity() > match_construct->measure(*best_descendent)->similarity()) {
            best_descendent = descendent;
        }

    }

    return best_descendent;
}


const std::shared_ptr<srcdiff_measure> & construct::measure(const construct & modified) const {
    std::unordered_map<int, std::shared_ptr<srcdiff_measure>>::const_iterator citr = measures.find(modified.start_position());
    if(citr != measures.end()) return citr->second;

    std::shared_ptr<srcdiff_measure> similarity = std::make_shared<srcdiff_text_measure>(*this, modified);
    similarity->compute();

    citr = measures.insert(citr, std::pair(modified.start_position(), similarity));
    return citr->second;
}

bool construct::is_similar(const construct & modified) const {

  const std::string & original_tag = root_term_name();
  const std::string & modified_tag = modified.root_term_name();

  if(size() == 1 && modified.size() == 1) {
    return original_tag == modified_tag;
  }

  if(size() == 1 || modified.size() == 1) {

    /** @todo need to handle this some time */
    return false;

  }

  srcdiff_syntax_measure syntax_measure(*this, modified);
  syntax_measure.compute();

  int min_child_length = syntax_measure.min_length();
  int max_child_length = syntax_measure.max_length();

  if(min_child_length > 1) { 

    if(2 * syntax_measure.similarity() >= min_child_length && syntax_measure.difference() <= min_child_length)
      return true;

  }

  /// @todo remove copy
  construct::construct_list child_construct_list_original = children();
  construct::construct_list child_construct_list_modified = modified.children();

  // check block of first child of if_stmt (old if behavior)
  if(original_tag == "if_stmt" && !child_construct_list_original.empty()) {

    std::string tag = child_construct_list_original.at(0)->root_term_name();
    if(tag == "else" || tag == "if") {
      construct::construct_list temp = construct::get_descendent_constructs(nodes(), child_construct_list_original.at(0)->get_terms().at(1), child_construct_list_original.back()->end_position());
      child_construct_list_original = temp;
    }

  }

  // check block of first child of if_stmt (old if behavior)
  if(modified_tag == "if_stmt" && !child_construct_list_modified.empty()) {

    std::string tag =  child_construct_list_modified.at(0)->root_term_name();
    if(tag == "else" || tag == "if") {
      construct::construct_list temp = construct::get_descendent_constructs(modified.nodes(), child_construct_list_modified.at(0)->get_terms().at(1), child_construct_list_modified.back()->end_position());
      child_construct_list_modified = temp;
    }

  }

  if(!child_construct_list_original.empty() && !child_construct_list_modified.empty()
    && child_construct_list_original.back()->root_term_name() == "block" && child_construct_list_modified.back()->root_term_name() == "block") {

    /// Why a copy?
    std::shared_ptr<construct> original_set = child_construct_list_original.back();
    std::shared_ptr<construct> modified_set = child_construct_list_modified.back();

    // block children actually in block_content
    construct::construct_list original_temp = construct::get_descendent_constructs(nodes(), child_construct_list_original.back()->get_terms().at(1), child_construct_list_original.back()->end_position());
    for(const std::shared_ptr<construct> & set : original_temp) {
      if(set->root_term_name() == "block_content") {
        original_set = set;
      }
    }

    // block children actually in block_content
    construct::construct_list modified_temp = construct::get_descendent_constructs(modified.nodes(), child_construct_list_modified.back()->get_terms().at(1), child_construct_list_modified.back()->end_position());
    for(const std::shared_ptr<construct> & set : modified_temp) {
      if(set->root_term_name() == "block_content") {
        modified_set = set;
      }
    }

    srcdiff_syntax_measure syntax_measure(*original_set, *modified_set);
    syntax_measure.compute();

    int min_child_length = syntax_measure.min_length();
    int max_child_length = syntax_measure.max_length();
    if(min_child_length > 1) { 
      if(2 * syntax_measure.similarity() >= min_child_length && syntax_measure.difference() <= min_child_length)
        return true;

    }

  }

  const srcdiff_measure & measure = *this->measure(modified);

  int min_size = measure.min_length();
  int max_size = measure.max_length();

#if DEBUG_SIMILARITY
  std::cerr << "Similarity: " << measure.similarity() << '\n';
  std::cerr << "Difference: " << measure.difference() << '\n';
  std::cerr << "Original Difference: " << measure.original_difference() << '\n';
  std::cerr << "Modified Difference: " << measure.modified_difference() << '\n';
  std::cerr << "Min Size: "   << min_size   << '\n';
  std::cerr << "Max Size: "   << max_size   << '\n';
#endif

  /** @todo consider making this configurable.  That is, allow user to specify file or have default file to read from */
  if(measure.difference() != 0 && measure.similarity() == 0) return false;

  if(min_size == measure.similarity() && measure.difference() < 2 * min_size) return true;
  if(min_size < 30 && measure.difference() > 1.25 * min_size)       return false;
  if(min_size >= 30 && measure.original_difference() > 0.25 * measure.original_length()
    && measure.modified_difference() > 0.25 * measure.modified_length()) return false;
  if(measure.difference() > max_size)                               return false;

  if(min_size <= 2)                return 2  * measure.similarity() >=     min_size;
  else if(min_size <= 3)           return 3  * measure.similarity() >= 2 * min_size;
  else if(min_size <= 30)          return 10 * measure.similarity() >= 7 * min_size;
  else                             return 2  * measure.similarity() >=     min_size;

}

bool construct::is_match_similar(const construct & modified) const {

  int original_pos = start_position();
  int modified_pos = modified.start_position();

  if(*term(0) != *modified.term(0)) return false;

  srcdiff_text_measure complete_measure(*this, modified, false);
  complete_measure.compute();
  int min_size = complete_measure.min_length();

  if(min_size == 0) return false;

  return min_size == complete_measure.similarity();

}

bool construct::can_refine_difference(const construct & modified) const {

  const std::string & original_tag = root_term_name();
  const std::string & modified_tag = modified.root_term_name();

  const std::string & original_uri = term(0)->ns.href;
  const std::string & modified_uri = modified.term(0)->ns.href;

  if(original_tag == modified_tag && original_uri == modified_uri) {
    return is_matchable(modified);
  } else if(is_tag_convertable(modified)) {
    return is_convertable(modified);
  } else {
    return false;
  }

}

bool construct::is_matchable(const construct & modified) const {

  const std::string & original_tag = root_term_name();
  const std::string & modified_tag = modified.root_term_name();

  const std::string & original_uri = term(0)->ns.href;
  const std::string & modified_uri = modified.term(0)->ns.href;

  if(original_uri != modified_uri) return false;
  if(original_tag != modified_tag) return false;
  if(is_matchable_impl(modified))  return true;

  return is_similar(modified);

}


bool construct::is_tag_convertable(const construct & modified) const {
  return false;
}

bool construct::is_matchable_impl(const construct & modified) const {
    return false;
}

bool construct::is_convertable(const construct & modified) const {
  if(is_convertable_impl(modified)) return true;
  return is_similar(modified);
}

bool construct::is_convertable_impl(const construct & modified) const {
    return false;
}
