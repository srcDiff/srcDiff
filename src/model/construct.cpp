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

#include <convertable_constructs.hpp>

//temp, probably
#include <srcdiff_match.hpp>
#include <srcdiff_match_internal.hpp>
#include <srcdiff_nested.hpp>

#include <iostream>

bool construct::is_non_white_space(int & node_pos, const srcml_nodes & node_list, const void * context) {

    const std::shared_ptr<srcml_node> & node = node_list[node_pos];

    // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
    return (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT || ((xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && !node->is_white_space());

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

std::shared_ptr<const construct> construct::name() const {
    return std::shared_ptr<const construct>();
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

  /** if different prefix should not reach here, however, may want to add that here */
  int original_pos = start_position();
  int modified_pos = modified.start_position();

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

  int original_pos = start_position();
  int modified_pos = modified.start_position();

  const std::string & original_tag = root_term_name();
  const std::string & modified_tag = modified.root_term_name();

  const std::string & original_uri = term(0)->ns.href;
  const std::string & modified_uri = modified.term(0)->ns.href;

  if(original_tag != modified_tag) return false;
  if(is_matchable_impl(modified)) return true;

  if(original_tag == "type" || original_tag == "then" || original_tag == "condition" || original_tag == "control" || original_tag == "init"
    || original_tag == "default" || original_tag == "comment"
    || original_tag == "private" || original_tag == "protected" || original_tag == "public" || original_tag == "signals"
    || original_tag == "parameter_list" || original_tag == "krparameter_list" || original_tag == "argument_list"
    || original_tag == "attribute_list" || original_tag == "association_list" || original_tag == "protocol_list"
    || original_tag == "super_list" || original_tag == "member_init_list" || original_tag == "member_list"
    || original_tag == "argument"
    || original_tag == "range"
    || original_tag == "literal" || original_tag == "operator" || original_tag == "modifier"
    || original_tag == "number" || original_tag == "file"

    // consider having this used to test similarity instead of block
    || original_tag == "block_content"
    )
    return true;

  const srcdiff_measure & measure = *this->measure(modified);

  if((original_tag == "expr" || original_tag == "expr_stmt") && measure.similarity() > 0 && measure.difference() <= measure.max_length()) return true;

  // may need to refine to if child only single name
  if(original_tag == "expr" && term(0)->parent && (*term(0)->parent)->name == "argument") return true;
  if(original_tag == "expr" && modified.term(0)->parent && (*modified.term(0)->parent)->name == "argument") return true;
  if(original_tag == "expr" && is_single_name_expr(nodes(), original_pos) && is_single_name_expr(modified.nodes(), modified_pos)) return true;

  if(original_tag == "block") {

    bool is_pseudo_original = bool(find_attribute(term(0), "type"));
    bool is_pseudo_modified = bool(find_attribute(modified.term(0), "type"));

    if(is_pseudo_original == is_pseudo_modified) {

      return true;

    } else if(measure.similarity()) {

      bool is_matchable = false;

      if(is_pseudo_original) {

        size_t block_contents_pos = 1;
        while(term_name(block_contents_pos) != "block_content") {
          ++block_contents_pos;
        }
        ++block_contents_pos;

        construct::construct_list construct_list_original = construct::get_descendent_constructs(nodes(), get_terms().at(block_contents_pos), end_position());
        construct::construct_list construct_list_modified = construct::get_descendent_constructs(modified.nodes(), modified.get_terms().at(0), modified.end_position() + 1);

        int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
        srcdiff_nested::check_nestable(construct_list_original, 0, construct_list_original.size(), construct_list_modified, 0, 1,
                      start_nest_original, end_nest_original, start_nest_modified , end_nest_modified, operation);

        is_matchable = (operation == SES_INSERT);

      } else {

        size_t block_contents_pos = 1;
        while(modified.term_name(block_contents_pos) != "block_content") {
          ++block_contents_pos;
        }
        ++block_contents_pos;

        construct::construct_list construct_list_original = construct::get_descendent_constructs(nodes(), get_terms().at(0), end_position() + 1);
        construct::construct_list construct_list_modified = construct::get_descendent_constructs(modified.nodes(), modified.get_terms().at(block_contents_pos), modified.end_position());

        int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
        srcdiff_nested::check_nestable(construct_list_original, 0, 1, construct_list_modified, 0, construct_list_modified.size(),
                      start_nest_original, end_nest_original, start_nest_modified , end_nest_modified, operation);

        is_matchable = (operation == SES_DELETE);

      }

      return is_matchable;

    }

  }

  if(original_tag == "call") {

    std::vector<std::string> original_names = get_call_name(nodes(), original_pos);
    std::vector<std::string> modified_names = get_call_name(modified.nodes(), modified_pos);

    if(name_list_similarity(original_names, modified_names)) return true;

  } else if(original_tag == "decl" || original_tag == "decl_stmt" || original_tag == "parameter" || original_tag == "param") {

    std::string original_name = get_decl_name(nodes(), original_pos);
    std::string modified_name = get_decl_name(modified.nodes(), modified_pos);

    if(original_name == modified_name && original_name != "") return true;

  } else if(original_tag == "if_stmt") {

    std::shared_ptr<construct> first_original = get_first_child(*this);
    std::shared_ptr<construct> first_modified = get_first_child(modified);

    if(is_child_if(*first_original) && is_child_if(*first_modified)) {

      /** todo play with getting and checking a match with all conditions */
      std::string original_condition = get_condition(nodes(), original_pos);
      std::string modified_condition = get_condition(modified.nodes(), modified_pos);

      bool original_has_block = conditional_has_block(*first_original);
      bool modified_has_block = conditional_has_block(*first_modified);

      bool original_has_else = if_stmt_has_else(*this);
      bool modified_has_else = if_stmt_has_else(modified);

      if(if_block_equal(*first_original, *first_modified)
        || (original_condition == modified_condition
          && ( original_has_block == modified_has_block 
            || original_has_else == modified_has_else 
            || (original_has_block && !modified_has_else) 
            || (modified_has_block && !original_has_else)))) {
        return true;
      }
    }

  } else if(original_tag == "if" && original_uri == SRCML_SRC_NAMESPACE_HREF) {

    if(get_condition(nodes(), original_pos) == get_condition(modified.nodes(), modified_pos)) {
      return true;
    }

    if(if_block_equal(*this, modified)) {
     return true;
    }

  } else if(original_tag == "while" || original_tag == "switch" || original_tag == "do") {

    std::string original_condition = get_condition(nodes(), original_pos);
    std::string modified_condition = get_condition(modified.nodes(), modified_pos);

    if(original_condition == modified_condition) return true;

  } else if(original_tag == "for" || original_tag == "foreach") {

    if(for_control_matches(*this, modified)) {
      return true;
    }

  } else if(original_tag == "case") { 

    std::string original_expr = get_case_expr(nodes(), original_pos);
    std::string modified_expr = get_case_expr(modified.nodes(), modified_pos);

    if(original_expr == modified_expr) return true;

  }

  return is_similar(modified);

}


bool construct::is_tag_convertable(const construct & modified) const {

  const std::string & original_tag = root_term_name();
  const std::string & modified_tag = modified.root_term_name();

  const std::string & original_uri = root_term()->ns.href;
  const std::string & modified_uri = modified.root_term()->ns.href;

  bool original_has_type_attribute = bool(find_attribute(root_term(), "type"));
  bool modified_has_type_attribute = bool(find_attribute(root_term(), "type"));

  if(original_uri != modified_uri) return false;

  if(original_tag == "if" && original_uri != SRCML_SRC_NAMESPACE_HREF) return false;
  if(modified_tag == "if" && modified_uri != SRCML_SRC_NAMESPACE_HREF) return false;

  if(original_tag == "if" && !original_has_type_attribute) return false;
  if(original_tag == "if" && !modified_has_type_attribute) return false;

  for(size_t list_pos = 0; convertable_table[list_pos].name; ++list_pos) {

    if(convertable_table[list_pos].name == original_tag) {

      for(size_t pos = 0; convertable_table[list_pos].list[pos]; ++pos) {

        if(convertable_table[list_pos].list[pos] == modified_tag)
          return true;

      }

    }

  }

  return false;

}
bool construct::is_matchable_impl(const construct & modified) const {
    return false;
}

bool construct::is_convertable(const construct & modified) const {

  int original_pos = start_position();
  int modified_pos = modified.start_position();

  const std::string & original_tag = root_term_name();
  const std::string & modified_tag = modified.root_term_name();

  const std::string & original_uri = term(0)->ns.href;
  const std::string & modified_uri = modified.term(0)->ns.href;

  std::string original_name;
  if(original_tag == "class" || original_tag == "struct" || original_tag == "union" || original_tag == "enum") {

      original_name = get_class_type_name(nodes(), original_pos);

  }

  std::string modified_name;
  if(modified_tag == "class" || modified_tag == "struct" || modified_tag == "union" || modified_tag == "enum") {

      modified_name = get_class_type_name(nodes(), original_pos);
    
  }

  if(original_name != "" && original_name == modified_name) return true;

  std::string original_condition;

  if(original_tag == "if_stmt") {
    /** todo play with getting and checking a match with all conditions */
    std::shared_ptr<construct> first_original = get_first_child(*this);
    if(is_child_if(*first_original)) {
      original_condition = get_condition(nodes(), original_pos);
    }
  }

  if(original_tag == "while" || original_tag == "for" || original_tag == "foreach") {

    original_condition = get_condition(nodes(), original_pos);

  }

  std::string modified_condition;
 
  if(modified_tag == "if_stmt") {
    std::shared_ptr<construct> first_modified = get_first_child(modified);
    if(is_child_if(*first_modified)) {
      modified_condition = get_condition(modified.nodes(), modified_pos);
    }
  }

  if(modified_tag == "while" || modified_tag == "for" || modified_tag == "foreach") {

    modified_condition = get_condition(modified.nodes(), modified_pos);

  }

  if(original_condition != "" && original_condition == modified_condition) return true;


  if(  (original_tag == "expr_stmt" || original_tag == "decl_stmt" || original_tag == "return")
    && (modified_tag == "expr_stmt" || modified_tag == "decl_stmt" || modified_tag == "return")) {

    std::shared_ptr<construct> expr_original(std::make_shared<construct>(nodes()));
    std::shared_ptr<construct> expr_modified(std::make_shared<construct>(modified.nodes()));
    if(original_tag == "decl_stmt" || modified_tag == "decl_stmt") {

      if(original_tag == "decl_stmt") {

        expr_modified = get_first_expr_child(modified.nodes(), modified_pos);

        if(!expr_modified->empty()) {

          construct::construct_list sets = construct::get_descendent_constructs(nodes(), get_terms().at(1), end_position(), srcdiff_nested::is_match,
                                                                                &expr_modified->term(0));
          int match = srcdiff_nested::best_match(sets, expr_modified);

          if(match < sets.size()) {
            expr_original = sets.at(match);
          }

        }

      } else {

        expr_original = get_first_expr_child(nodes(), original_pos);

        if(!expr_original->empty()) {

          construct::construct_list sets = construct::get_descendent_constructs(modified.nodes(), modified.get_terms().at(1), modified.end_position(), srcdiff_nested::is_match,
                                    &expr_original->term(0));
          int match = srcdiff_nested::best_match(sets, expr_original);

          if(match < sets.size()) {
            expr_modified = sets.at(match);
          }

        }

      }

    } else {

      expr_original = get_first_expr_child(nodes(), original_pos);
      expr_modified = get_first_expr_child(modified.nodes(), modified_pos);

    }

    if(expr_original->size() && expr_modified->size()) {

      const srcdiff_measure & expr_measure = *expr_original->measure(*expr_modified);

      bool is_expr_reject = !expr_original->is_similar(*expr_modified);

      int min_size = expr_measure.min_length();
      int max_size = expr_measure.max_length();

      if(!is_expr_reject && 2 * expr_measure.similarity() > max_size && 2 * expr_measure.difference() < max_size) return true;

    }

  }

  return is_similar(modified);

}

