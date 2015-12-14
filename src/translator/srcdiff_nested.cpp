#include <srcdiff_nested.hpp>

#include <srcdiff_constants.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_match.hpp>
#include <shortest_edit_script.h>

#include <cstring>

srcdiff_nested::srcdiff_nested(const srcdiff_many & diff, int start_original, int end_original, int start_modified, int end_modified, int operation)
  : srcdiff_many(diff), start_original(start_original), end_original(end_original), start_modified(start_modified), end_modified(end_modified), operation(operation) {}

int nest_id = 0;

struct nest_info {

  const char * type;

  const char * const * possible_nest_items;

};

// may need to change collection algorithm to gather full and nested of same type           
const char * const block_nest_types[]       = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach", "else", "elseif", "try", "catch", "finally", "synchronized",                                        0 };
const char * const then_nest_types[]        = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach", "else", "elseif", "try", "catch", "finally", "synchronized", "expr", "call", "operator", "literal", 0 };
const char * const else_nest_types[]        = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach", "try", "catch", "finally", "synchronized", "expr", "call", "operator", "literal",                   0 };
const char * const for_control_nest_types[] = { "condition", "comment",                                                                                                                                                                      0 };
const char * const class_nest_types[]       = { "decl_stmt", "function_decl", "function", "constructor_decl", "constructor", "destructor_decl", "destructor", "class_decl", "class",
                                                "struct_decl", "struct", "union_decl", "union", "typedef",                                                                                                                                   0 };
const char * const call_nest_types[]        = { "expr", "call", "operator", "literal", "name",                                                                                                                                               0 };
const char * const ternary_nest_types[]     = { "ternary", "call", "operator", "literal", "expr",                                                                                                                                            0 };
const char * const condition_nest_types[]   = { "expr", "call", "operator", "literal",                                                                                                                                                       0 };
const char * const name_nest_types[]        = { "name",                                                                                                                                                                                      0 };
const char * const extern_nest_types[]      = { "decl_stmt", "function_decl", "function", "class", "class_decl", "struct", "struct_decl", "union", "union_decl", "typedef", "using",                                                         0 };
const char * const decl_nest_types[]        = { "expr",                                                                                                                                                                                      0 };
const char * const static_nest_types[]      = { "decl_stmt",                                                                                                                                                                                 0 };

// tags that can have something nested in them (incomplete)    
const nest_info nesting[] = {   

  { "block",         block_nest_types       },
  { "if",            block_nest_types       },
  { "then",          then_nest_types        },
  { "elseif",        block_nest_types       },
  { "else",          else_nest_types        },
  { "while",         block_nest_types       },
  { "for",           block_nest_types       },
  { "foreach",       block_nest_types       },
  { "control",       for_control_nest_types },
  { "function",      block_nest_types       },
  { "class",         class_nest_types       },
  { "struct",        class_nest_types       },
  { "union",         class_nest_types       },
  { "call",          call_nest_types        },
  { "argument_list", call_nest_types        },
  { "argument",      call_nest_types        },
  { "expr",          call_nest_types        },
  { "ternary",       ternary_nest_types     },
  { "condition",     condition_nest_types   },
  { "name",          name_nest_types        },
  { "try",           block_nest_types       },
  { "catch",         block_nest_types       },
  { "extern",        extern_nest_types      },
  { "decl",          decl_nest_types        },
  { "init",          decl_nest_types        },

  // Java
  { "static",        static_nest_types      },
  { "synchronized",  block_nest_types       },
  { "finally",       block_nest_types       },
  { 0, 0 }

};

int is_block_type(const node_set & structure, const srcml_nodes & nodes) {

  if((xmlReaderTypes)nodes.at(structure.at(0))->type != XML_READER_TYPE_ELEMENT)
    return -1;

  if(nodes.at(structure.at(0))->ns->href != SRCML_SRC_NAMESPACE_HREF)
    return -1;

  for(int i = 0; nesting[i].type; ++i)
    if(nodes.at(structure.at(0))->name == nesting[i].type)
      return i;

  return -1;
}

bool has_internal_structure(const node_set & structure, const srcml_nodes & nodes, const boost::optional<std::string> & type) {

  if(!type) return false;

  for(unsigned int i = 1; i < structure.size(); ++i)
    if((xmlReaderTypes)nodes.at(structure.at(i))->type == XML_READER_TYPE_ELEMENT
              && nodes.at(structure.at(i))->name == type)
      return true;

  return false;
}

bool is_nest_type(const node_set & structure, const srcml_nodes & nodes
                           , const node_set & structure_other, const srcml_nodes & nodes_other, int type_index) {

  if((xmlReaderTypes)nodes.at(structure.at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

    if(nodes.at(structure.at(0))->ns->href != SRCML_SRC_NAMESPACE_HREF)
    return -1;

  for(int i = 0; nesting[type_index].possible_nest_items[i]; ++i)
    if(nodes.at(structure.at(0))->name == nesting[type_index].possible_nest_items[i]
       && has_internal_structure(structure_other, nodes_other, nodes.at(structure.at(0))->name))
      return true;

  return false;
}

bool srcdiff_nested::is_match(int & node_pos, const srcml_nodes & nodes, const void * context) {

  const std::shared_ptr<srcml_node> & node = nodes[node_pos];

  const std::shared_ptr<srcml_node> & context_node = *(const std::shared_ptr<srcml_node> *)context;

  return (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT && srcdiff_compare::node_compare(node, context_node) == 0;

}

int srcdiff_nested::best_match(const srcml_nodes & nodes, const node_sets & set, const srcml_nodes & nodes_match, const node_set & match, int operation) {

  int match_pos = set.size();
  int match_similarity = 0;

  if(set.size() > 0) {

    if(!((set.at(0).size() > match.size() && (set.at(0).size()) > (4 * match.size()))
      || (match.size() > set.at(0).size() && (match.size()) > (4 * set.at(0).size())))) {

      match_pos = 0;
      srcdiff_measure measure(nodes, nodes_match, set.at(0), match);
      match_similarity = measure.compute_similarity();

    }

  } else
    return 1;

  for(unsigned int i = 1; i < set.size(); ++i) {

    if(set.at(i).size() > match.size() && (set.at(i).size()) > (4 * match.size()))
      continue;

    if(match.size() > set.at(i).size() && (match.size()) > (4 * set.at(i).size()))
      continue;

    srcdiff_measure measure(nodes, nodes_match, set.at(i), match);
    int similarity = measure.compute_similarity();
    if(similarity > match_similarity) {

      match_pos = i;
      match_similarity = similarity;

    }

  }

  return match_pos;

}

bool is_nestable_internal(const node_set & structure_one, const srcml_nodes & nodes_one
                 , const node_set & structure_two, const srcml_nodes & nodes_two) {

  int block = is_block_type(structure_two, nodes_two);

  if(block == -1)
    return false;

  /** Only can nest a block into another block if it's parent is a block */
  bool is_block = nodes_one.at(structure_one.at(0))->name == "block" && nodes_two.at(structure_two.at(0))->name == "block";
  bool parent_is_block = nodes_one.at(structure_one.at(0))->parent && (*nodes_one.at(structure_one.at(0))->parent)->name == "block";
  if(is_block && !parent_is_block) return false;

  if(is_nest_type(structure_one, nodes_one, structure_two, nodes_two, block)) {

    return true;

  }

  return false;
}

bool srcdiff_nested::is_same_nestable(const node_set & structure_one, const srcml_nodes & nodes_one
                      , const node_set & structure_two, const srcml_nodes & nodes_two) {

  if(!is_nestable_internal(structure_one, nodes_one, structure_two, nodes_two))
    return false;

  //unsigned int similarity = compute_similarity(nodes_one, structure_one, nodes_two, structure_two);

  node_sets set = node_sets(nodes_two, structure_two.at(1), structure_two.back(), srcdiff_nested::is_match
                                                             , &nodes_one.at(structure_one.at(0)));

  unsigned int match = best_match(nodes_two, set, nodes_one, structure_one, SES_DELETE);

  if(match >= set.size())
    return false;

  srcdiff_measure match_measure(nodes_one, nodes_two, structure_one, set.at(match));
  int match_similarity, match_difference, size_one, size_match;
  match_measure.compute_measures(match_similarity, match_difference, size_one, size_match);

  srcdiff_measure measure(nodes_one, nodes_two, structure_one, structure_two);
  int similarity, difference, size_two;
  measure.compute_measures(similarity, difference, size_one, size_two);

  double min_size = size_one < size_two ? size_one : size_two;
  double match_min_size = size_one < size_match ? size_one : size_match;

  return (match_similarity >= similarity && match_difference <= difference) 
  || (match_min_size > 50 && min_size > 50 && (match_min_size / match_similarity) < (0.9 * (min_size / similarity))
//   && match_difference < 1.5 * difference
    && !srcdiff_nested::reject_match_nested(match_similarity, match_difference, size_match, size_one, nodes_two, set.at(match), nodes_one, structure_one));

}

bool srcdiff_nested::is_nestable(const node_set & structure_one, const srcml_nodes & nodes_one
                 , const node_set & structure_two, const srcml_nodes & nodes_two) {

  if(srcdiff_compare::node_compare(nodes_one.at(structure_one.at(0)), nodes_two.at(structure_two.at(0))) == 0)
    return is_same_nestable(structure_one, nodes_one, structure_two, nodes_two);
  else
    return is_nestable_internal(structure_one, nodes_one, structure_two, nodes_two);

}

bool is_better_nest_no_recursion(const srcml_nodes & nodes_outer, const node_set & node_set_outer,
                    const srcml_nodes & nodes_inner, const node_set & node_set_inner,
                    int similarity, int difference, int text_outer_length, int text_inner_length) {

    if(srcdiff_nested::is_nestable(node_set_inner, nodes_inner, node_set_outer, nodes_outer)) {

      node_sets set = node_sets(nodes_outer, node_set_outer.at(1), node_set_outer.back(), srcdiff_nested::is_match
                                                             , &nodes_inner.at(node_set_inner.at(0)));

      int match = srcdiff_nested::best_match(nodes_outer, set, nodes_inner, node_set_inner, SES_DELETE);

      if(match < set.size()) {

        srcdiff_measure measure(nodes_outer, nodes_inner, set.at(match), node_set_inner);
        int nest_similarity, nest_difference, nest_text_outer_length, nest_text_inner_length;
        measure.compute_measures(nest_similarity, nest_difference, nest_text_outer_length, nest_text_inner_length);

        double min_size = text_outer_length < text_inner_length ? text_outer_length : text_inner_length;
        double nest_min_size = nest_text_outer_length < nest_text_inner_length ? nest_text_outer_length : nest_text_inner_length;

        if((nest_similarity >= similarity && nest_difference <= difference)
         || ((nest_min_size / nest_similarity) < (min_size / similarity)
            && !srcdiff_nested::reject_match_nested(nest_similarity, nest_difference, nest_text_inner_length, nest_text_outer_length, nodes_inner, node_set_inner, nodes_outer, node_set_outer)))
          return true;
    
      }

    }

    return false;

}

bool has_compound_inner(const srcml_nodes & nodes_outer, const node_set & node_set_outer) {

  if(nodes_outer.at(node_set_outer.at(0))->is_simple) return false;

  for(unsigned int i = 1; i < node_set_outer.size(); ++i)
    if((xmlReaderTypes)nodes_outer.at(node_set_outer.at(i))->type == XML_READER_TYPE_ELEMENT
      && nodes_outer.at(node_set_outer.at(i))->name == "name" && !nodes_outer.at(node_set_outer.at(i))->is_simple)
      return true;

  return false;

}

bool is_better_nest(const srcml_nodes & nodes_outer, const node_set & node_set_outer,
                    const srcml_nodes & nodes_inner, const node_set & node_set_inner,
                    int similarity, int difference, int text_outer_length, int text_inner_length) {

    if(nodes_outer.at(node_set_outer.at(0))->name == "name" && nodes_inner.at(node_set_inner.at(0))->name == "name"
      &&    (nodes_outer.at(node_set_outer.at(0))->is_simple
        || (!nodes_inner.at(node_set_inner.at(0))->is_simple && !has_compound_inner(nodes_outer, node_set_outer)))) return false;

// parents and children same do not nest.
    if(srcdiff_nested::is_nestable(node_set_inner, nodes_inner, node_set_outer, nodes_outer)) {

      node_sets set = node_sets(nodes_outer, node_set_outer.at(1), node_set_outer.back(), srcdiff_nested::is_match
                                                             , &nodes_inner.at(node_set_inner.at(0)));

      int match = srcdiff_nested::best_match(nodes_outer, set, nodes_inner, node_set_inner, SES_DELETE);

      if(match < set.size()) {

        srcdiff_measure measure(nodes_outer, nodes_inner, set.at(match), node_set_inner);
        int nest_similarity, nest_difference, nest_text_outer_length, nest_text_inner_length;
        measure.compute_measures(nest_similarity, nest_difference, nest_text_outer_length, nest_text_inner_length);

        double min_size = text_outer_length < text_inner_length ? text_outer_length : text_inner_length;
        double nest_min_size = nest_text_outer_length < nest_text_inner_length ? nest_text_outer_length : nest_text_inner_length;

        if((nest_similarity >= similarity && nest_difference <= difference)
         || ((nest_min_size / nest_similarity) < (min_size / similarity)
            && !srcdiff_nested::reject_match_nested(nest_similarity, nest_difference, nest_text_inner_length, nest_text_outer_length, nodes_inner, node_set_inner, nodes_outer, node_set_outer))
//         || ((nest_min_size / nest_difference) > (min_size / difference))
         )
          return !is_better_nest_no_recursion(nodes_inner, node_set_inner, nodes_outer, node_set_outer, nest_similarity, nest_difference, nest_text_inner_length, nest_text_outer_length);

      }

    }

    return false;

}

bool srcdiff_nested::is_better_nested(const srcml_nodes & nodes_original, const node_sets & node_sets_original, int start_pos_original,
                    const srcml_nodes & nodes_modified, const node_sets & node_sets_modified, int start_pos_modified,
                    int similarity, int difference, int text_original_length, int text_modified_length) {

  for(int pos = start_pos_original; pos < node_sets_original.size(); ++pos) {

    int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
    check_nestable(node_sets_original, nodes_original, pos, pos + 1
                 , node_sets_modified, nodes_modified, start_pos_modified, start_pos_modified + 1
                 , start_nest_original, end_nest_original, start_nest_modified, end_nest_modified
                 , operation);
    if(operation == SES_COMMON) continue;
    if(is_better_nest(nodes_original, node_sets_original.at(pos), nodes_modified, node_sets_modified.at(start_pos_modified), similarity, difference, text_original_length, text_modified_length))
      return true;

  }

  for(int pos = start_pos_modified; pos < node_sets_modified.size(); ++pos) {

    int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
    check_nestable(node_sets_original, nodes_original, start_pos_original, start_pos_original + 1
                 , node_sets_modified, nodes_modified, pos, pos + 1
                 , start_nest_original, end_nest_original, start_nest_modified, end_nest_modified
                 , operation);
    if(operation == SES_COMMON) continue;
    if(is_better_nest(nodes_modified, node_sets_modified.at(pos), nodes_original, node_sets_original.at(start_pos_original), similarity, difference, text_original_length, text_modified_length))
      return true;

  }

  return false;

}

static bool is_decl_stmt_from_expr(const srcml_nodes & nodes, int pos) {

  if((*nodes.at(pos)->parent)->name == "init") {

    return bool((*nodes.at(pos)->parent)->parent) && bool((*(*nodes.at(pos)->parent)->parent)->parent)
            && (*(*(*nodes.at(pos)->parent)->parent)->parent)->name == "decl_stmt";

  }

  if((*nodes.at(pos)->parent)->name == "argument") {

    return bool((*nodes.at(pos)->parent)->parent) && bool((*(*nodes.at(pos)->parent)->parent)->parent)
            && bool((*(*(*nodes.at(pos)->parent)->parent)->parent)->parent)
            && (*(*(*(*nodes.at(pos)->parent)->parent)->parent)->parent)->name == "decl_stmt";

  }

  return false;

}

bool srcdiff_nested::reject_match_nested(int similarity, int difference, int text_original_length, int text_modified_length,
  const srcml_nodes & nodes_original, const node_set & set_original, const srcml_nodes & nodes_modified, const node_set & set_modified) {

  int original_pos = set_original.at(0);
  int modified_pos = set_modified.at(0);

  const std::string & original_tag = nodes_original.at(original_pos)->name;
  const std::string & modified_tag = nodes_modified.at(modified_pos)->name;

  const std::string & original_uri = nodes_original.at(original_pos)->ns->href;
  const std::string & modified_uri = nodes_modified.at(modified_pos)->ns->href;

  if(original_tag != modified_tag && !srcdiff_match::is_interchangeable_match(original_tag, original_uri, modified_tag, modified_uri)) return true;

  // if interchanging decl_stmt always nest expr into init or argument
  if(original_tag == "expr" && (is_decl_stmt_from_expr(nodes_original, original_pos) || is_decl_stmt_from_expr(nodes_modified, modified_pos))) return false;

  if(original_tag == "name" && nodes_original.at(original_pos)->is_simple != nodes_modified.at(modified_pos)->is_simple) return true;

  if(original_tag == "then" || original_tag == "block" || original_tag == "comment"
    || original_tag == "literal" || original_tag == "operator" || original_tag == "modifier"
    || original_tag == "expr" || original_tag == "expr_stmt" || original_tag == "name") {

    bool is_reject = srcdiff_match::reject_similarity(similarity, difference, text_original_length, text_modified_length,
      nodes_original, set_original, nodes_modified, set_modified);
    return is_reject;

  } else {

    return srcdiff_match::reject_match(similarity, difference, text_original_length, text_modified_length, nodes_original, set_original, nodes_modified, set_modified);

  }

}

static bool check_nested_single_to_many(const node_sets & node_sets_original, const srcml_nodes & nodes_original, int start_original, int end_original
                 , const node_sets & node_sets_modified, const srcml_nodes & nodes_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation) {

  int nest_count_original = 0;
  boost::optional<int> pos_original;
  boost::optional<int> similarity_original;
  boost::optional<int> difference_original;
  for(int i = start_original; i < end_original; ++i) {

    if(nodes_original.at(node_sets_original.at(i).at(0))->move) continue;

    for(int j = start_modified; j < end_modified; ++j) {

      if(nodes_modified.at(node_sets_modified.at(j).at(0))->move) continue;

      if(srcdiff_nested::is_nestable(node_sets_modified.at(j), nodes_modified, node_sets_original.at(i), nodes_original)) {

        node_sets set = node_sets(nodes_original, node_sets_original.at(i).at(1), node_sets_original.at(i).back(), srcdiff_nested::is_match
                                                             , &nodes_modified.at(node_sets_modified.at(j).at(0)));

        int match = srcdiff_nested::best_match(nodes_original, set, nodes_modified, node_sets_modified.at(j), SES_DELETE);

        if(match >= set.size()) continue;

        srcdiff_measure measure(nodes_original, nodes_modified, set.at(match), node_sets_modified.at(j));
        int similarity, difference, text_original_length, text_modified_length;
        measure.compute_measures(similarity, difference, text_original_length, text_modified_length);

        if(srcdiff_nested::reject_match_nested(similarity, difference, text_original_length, text_modified_length,
                                               nodes_original, set.at(match), nodes_modified, node_sets_modified.at(j)))
          continue;

        if(nodes_modified.at(node_sets_modified.at(j).at(0))->name == "name"
          && nodes_modified.at(node_sets_modified.at(j).at(0))->parent && (*nodes_modified.at(node_sets_modified.at(j).at(0))->parent)->name == "expr"
          && nodes_original.at(node_sets_original.at(i).at(0))->parent && (*nodes_original.at(node_sets_original.at(i).at(0))->parent)->name == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1))
          continue;

        if(nodes_modified.at(node_sets_modified.at(j).at(0))->name == "name") {

            if(!nodes_modified.at(node_sets_modified.at(j).at(0))->parent || !nodes_original.at(set.at(match).at(0))->parent)
              continue;

            boost::optional<std::shared_ptr<srcml_node>> parent_original = nodes_original.at(set.at(match).at(0))->parent;
            while((*parent_original)->name == "name")
              parent_original = (*parent_original)->parent;

            boost::optional<std::shared_ptr<srcml_node>> parent_modified = nodes_modified.at(node_sets_modified.at(j).at(0))->parent;
            while((*parent_modified)->name == "name")
              parent_modified = (*parent_modified)->parent;

            if((*parent_original)->name != (*parent_modified)->name)
              continue;

        }

        if(!bool(pos_original) || similarity > similarity_original) {

          ++nest_count_original;
          pos_original = i;
          similarity_original = similarity;
          difference_original = difference;

        }

      }

    }

  }

  int nest_count_modified = 0;
  boost::optional<int> pos_modified;
  boost::optional<int> similarity_modified;
  boost::optional<int> difference_modified;
  for(int i = start_modified; i < end_modified; ++i) {

    if(nodes_modified.at(node_sets_modified.at(i).at(0))->move) continue;

    for(int j = start_original; j < end_original; ++j) {

      if(nodes_original.at(node_sets_original.at(j).at(0))->move) continue;

      if(srcdiff_nested::is_nestable(node_sets_original.at(j), nodes_original, node_sets_modified.at(i), nodes_modified)) {

        node_sets set = node_sets(nodes_modified, node_sets_modified.at(i).at(1), node_sets_modified.at(i).back(), srcdiff_nested::is_match
                                                             , &nodes_original.at(node_sets_original.at(j).at(0)));

        int match = srcdiff_nested::best_match(nodes_modified, set, nodes_original, node_sets_original.at(j), SES_INSERT);

        if(match >= set.size()) continue;

        srcdiff_measure measure(nodes_original, nodes_modified, node_sets_original.at(j), set.at(match));
        int similarity, difference, text_original_length, text_modified_length;
        measure.compute_measures(similarity, difference, text_original_length, text_modified_length);

        if(srcdiff_nested::reject_match_nested(similarity, difference, text_original_length, text_modified_length,
                                               nodes_original, node_sets_original.at(j), nodes_modified, set.at(match)))
          continue;

        if(nodes_original.at(node_sets_original.at(j).at(0))->name == "name"
          && nodes_original.at(node_sets_original.at(j).at(0))->parent && (*nodes_original.at(node_sets_original.at(j).at(0))->parent)->name == "expr"
          && nodes_modified.at(node_sets_modified.at(i).at(0))->parent && (*nodes_modified.at(node_sets_modified.at(i).at(0))->parent)->name == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1))
          continue;

        if(nodes_original.at(node_sets_original.at(j).at(0))->name == "name") {

            if(!nodes_original.at(node_sets_original.at(j).at(0))->parent || !nodes_modified.at(set.at(match).at(0))->parent)
              continue;

            boost::optional<std::shared_ptr<srcml_node>> parent_original = nodes_original.at(node_sets_original.at(j).at(0))->parent;
            while(parent_original && (*parent_original)->name == "name")
              parent_original = (*parent_original)->parent;

            boost::optional<std::shared_ptr<srcml_node>> parent_modified = nodes_modified.at(set.at(match).at(0))->parent;
            while(parent_modified && (*parent_modified)->name == "name")
              parent_modified = (*parent_modified)->parent;

            if((*parent_original)->name != (*parent_modified)->name)
              continue;

        }

        if(!bool(pos_modified) || similarity > similarity_modified) {

          ++nest_count_modified;
          pos_modified = i;
          similarity_modified = similarity;
          difference_modified = difference;

        }

      }

    }

  }

  if(nest_count_original == 0 && nest_count_modified == 0) return true;
  if(((end_original - start_original) > 1 || (end_modified - start_modified) > 1)
    &&  (((end_original - start_original) == 1 && nest_count_original != 0)
      || ((end_modified - start_modified) == 1 && nest_count_modified != 0)))
    return false;

  if(bool(pos_original) && (!bool(pos_modified) || *similarity_original > *similarity_modified
    || (*similarity_original == *similarity_modified && *difference_original <= *difference_modified))) {

      start_nest_original = *pos_original;
      end_nest_original   = *pos_original + 1;
      start_nest_modified = start_modified;
      end_nest_modified   = start_modified + 1;
      operation = SES_DELETE;

  } else if(bool(pos_modified)) {

      start_nest_original = start_original;
      end_nest_original   = start_original + 1;
      start_nest_modified = *pos_modified;
      end_nest_modified   = *pos_modified + 1;
      operation = SES_INSERT;

  }

  return true;

}

bool srcdiff_nested::check_nestable_predicate(int similarity, int difference, int text_outer_length, int text_inner_length,
                                              const node_set & match,
                                              const srcml_nodes & nodes_outer, const node_sets & node_sets_outer, int pos_outer, int end_outer,
                                              const srcml_nodes & nodes_inner, const node_sets & node_sets_inner, int pos_inner, int end_inner) {

  if(reject_match_nested(similarity, difference, text_outer_length, text_inner_length,
          nodes_outer, match, nodes_inner, node_sets_inner.at(pos_inner)))
    return true;

  if(is_better_nest(nodes_inner, node_sets_inner.at(pos_inner), nodes_outer, node_sets_outer.at(pos_outer), similarity, difference, text_inner_length, text_outer_length))
    return true;

  if(pos_outer + 1 < end_outer && is_better_nest(nodes_outer, node_sets_outer.at(pos_outer + 1), nodes_inner, node_sets_inner.at(pos_inner), similarity, difference, text_outer_length, text_inner_length))
    return true;

  if(pos_inner + 1 < end_inner && is_better_nest(nodes_inner, node_sets_inner.at(pos_inner + 1), nodes_outer, node_sets_outer.at(pos_outer), similarity, difference, text_inner_length, text_outer_length))
    return true;


  return false;


}

/**
 *
 * @todo need to make this more robust using dynamic programming.
 * Probably need one for both ways. Collect all statements of type one, and all the possible matches (statements of those types in order)
 * and run dynamic programming algorithm.  Run both ways and use number matched and similarity to choose best.
 *
 */

void srcdiff_nested::check_nestable(const node_sets & node_sets_original, const srcml_nodes & nodes_original, int start_original, int end_original
                 , const node_sets & node_sets_modified, const srcml_nodes & nodes_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation) {

  start_nest_original = start_original;  
  end_nest_original = start_original;  

  start_nest_modified = start_modified;  
  end_nest_modified = start_modified;

  operation = SES_COMMON;

  if((end_original - start_original) == 1 || (end_modified - start_modified) == 1) {

    if(check_nested_single_to_many(node_sets_original, nodes_original, start_original, end_original,
                                node_sets_modified, nodes_modified, start_modified, end_modified,
                                start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation))
      return;

  }

  std::vector<int> valid_nests_original;
  std::vector<int> valid_nests_modified;

  for(int i = start_original; i < end_original; ++i) {

    if(nodes_original.at(node_sets_original.at(i).at(0))->move) continue;

    for(int j = start_modified; j < end_modified; ++j) {

      if(nodes_modified.at(node_sets_modified.at(j).at(0))->move) continue;

      if(is_nestable(node_sets_modified.at(j), nodes_modified, node_sets_original.at(i), nodes_original)) {

        node_sets set = node_sets(nodes_original, node_sets_original.at(i).at(1), node_sets_original.at(i).back(), is_match
                                                             , &nodes_modified.at(node_sets_modified.at(j).at(0)));

        int match = best_match(nodes_original, set, nodes_modified, node_sets_modified.at(j), SES_DELETE);

        if(match >= set.size()) continue;

        srcdiff_measure measure(nodes_original, nodes_modified, set.at(match), node_sets_modified.at(j));
        int similarity, difference, text_original_length, text_modified_length;
        measure.compute_measures(similarity, difference, text_original_length, text_modified_length);


        if(check_nestable_predicate(similarity, difference, text_original_length, text_modified_length,
           set.at(match), nodes_original, node_sets_original, i, end_original, nodes_modified, node_sets_modified, j, end_modified))
          continue;

        if(nodes_modified.at(node_sets_modified.at(j).at(0))->name == "name"
          && nodes_modified.at(node_sets_modified.at(j).at(0))->parent && (*nodes_modified.at(node_sets_modified.at(j).at(0))->parent)->name == "expr"
          && nodes_original.at(node_sets_original.at(i).at(0))->parent && (*nodes_original.at(node_sets_original.at(i).at(0))->parent)->name == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1))
          continue;

        if(nodes_modified.at(node_sets_modified.at(j).at(0))->name == "name") {

            if(!nodes_modified.at(node_sets_modified.at(j).at(0))->parent || !nodes_original.at(set.at(match).at(0))->parent)
              continue;

            boost::optional<std::shared_ptr<srcml_node>> parent_original = nodes_original.at(set.at(match).at(0))->parent;
            while((*parent_original)->name == "name")
              parent_original = (*parent_original)->parent;

            boost::optional<std::shared_ptr<srcml_node>> parent_modified = nodes_modified.at(node_sets_modified.at(j).at(0))->parent;
            while((*parent_modified)->name == "name")
              parent_modified = (*parent_modified)->parent;

            if((*parent_original)->name != (*parent_modified)->name)
              continue;

        }

        valid_nests_original.push_back(j);

        start_nest_original = i;
        end_nest_original = i + 1;

        for(int k = j + 1; k < end_modified; ++k) {

          if(nodes_modified.at(node_sets_modified.at(k).at(0))->move) continue;

          if(!is_nestable(node_sets_modified.at(k), nodes_modified, node_sets_original.at(i), nodes_original)) continue;

          node_sets set = node_sets(nodes_original, node_sets_original.at(i).at(1), node_sets_original.at(i).back(), is_match
                                                               , &nodes_modified.at(node_sets_modified.at(k).at(0)));

          int match = best_match(nodes_original, set, nodes_modified, node_sets_modified.at(k), SES_DELETE);

          if(match >= set.size()) continue;

          srcdiff_measure measure(nodes_original, nodes_modified, set.at(match), node_sets_modified.at(k));
          int similarity, difference, text_original_length, text_modified_length;
          measure.compute_measures(similarity, difference, text_original_length, text_modified_length);

          if(check_nestable_predicate(similarity, difference, text_original_length, text_modified_length,
             set.at(match), nodes_original, node_sets_original, i, end_original, nodes_modified, node_sets_modified, k, end_modified))
            continue;

          if(nodes_modified.at(node_sets_modified.at(k).at(0))->name == "name"
            && nodes_modified.at(node_sets_modified.at(k).at(0))->parent && (*nodes_modified.at(node_sets_modified.at(k).at(0))->parent)->name == "expr"
            && nodes_original.at(node_sets_original.at(i).at(0))->parent && (*nodes_original.at(node_sets_original.at(i).at(0))->parent)->name == "expr"
            && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1))
            continue;

          if(nodes_modified.at(node_sets_modified.at(k).at(0))->name == "name") {

              if(!nodes_modified.at(node_sets_modified.at(k).at(0))->parent || !nodes_original.at(set.at(match).at(0))->parent)
                continue;

              boost::optional<std::shared_ptr<srcml_node>> parent_original = nodes_original.at(set.at(match).at(0))->parent;
              while(parent_original && (*parent_original)->name == "name")
                parent_original = (*parent_original)->parent;

              boost::optional<std::shared_ptr<srcml_node>> parent_modified = nodes_modified.at(node_sets_modified.at(k).at(0))->parent;
              while(parent_modified && (*parent_modified)->name == "name")
                parent_modified = (*parent_modified)->parent;

              if((*parent_original)->name != (*parent_modified)->name)
                continue;

          }

          valid_nests_original.push_back(k);

        }

        goto end_nest_check_original;

      }

    }

  }

  end_nest_check_original:

  for(int i = start_modified; i < end_modified; ++i) {

    if(nodes_modified.at(node_sets_modified.at(i).at(0))->move) continue;

    for(int j = start_original; j < end_original; ++j) {

      if(nodes_original.at(node_sets_original.at(j).at(0))->move) continue;

      if(is_nestable(node_sets_original.at(j), nodes_original, node_sets_modified.at(i), nodes_modified)) {

        node_sets set = node_sets(nodes_modified, node_sets_modified.at(i).at(1), node_sets_modified.at(i).back(), is_match
                                                             , &nodes_original.at(node_sets_original.at(j).at(0)));

        int match = best_match(nodes_modified, set, nodes_original, node_sets_original.at(j), SES_INSERT);

        if(match >= set.size()) continue;

        srcdiff_measure measure(nodes_original, nodes_modified, node_sets_original.at(j), set.at(match));
        int similarity, difference, text_original_length, text_modified_length;
        measure.compute_measures(similarity, difference, text_original_length, text_modified_length);

        if(check_nestable_predicate(similarity, difference, text_modified_length, text_original_length,
           set.at(match), nodes_modified, node_sets_modified, i, end_modified, nodes_original, node_sets_original, j, end_original))
          continue;

        if(nodes_original.at(node_sets_original.at(j).at(0))->name == "name"
          && nodes_original.at(node_sets_original.at(j).at(0))->parent && (*nodes_original.at(node_sets_original.at(j).at(0))->parent)->name == "expr"
          && nodes_modified.at(node_sets_modified.at(i).at(0))->parent && (*nodes_modified.at(node_sets_modified.at(i).at(0))->parent)->name == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1))
          continue;

        if(nodes_original.at(node_sets_original.at(j).at(0))->name == "name") {

            if(!nodes_original.at(node_sets_original.at(j).at(0))->parent || !nodes_modified.at(set.at(match).at(0))->parent)
              continue;

            boost::optional<std::shared_ptr<srcml_node>> parent_original = nodes_original.at(node_sets_original.at(j).at(0))->parent;
            while(parent_original && (*parent_original)->name == "name")
              parent_original = (*parent_original)->parent;

            boost::optional<std::shared_ptr<srcml_node>> parent_modified = nodes_modified.at(set.at(match).at(0))->parent;
            while(parent_modified && (*parent_modified)->name == "name")
              parent_modified = (*parent_modified)->parent;

            if((*parent_original)->name != (*parent_modified)->name)
              continue;

        }

        valid_nests_modified.push_back(j);

        start_nest_modified = i;
        end_nest_modified = i + 1;

        for(int k = j + 1; k < end_original; ++k) {

          if(nodes_original.at(node_sets_original.at(k).at(0))->move) continue;
        
          if(!is_nestable(node_sets_original.at(k), nodes_original, node_sets_modified.at(i), nodes_modified)) continue;

            node_sets set = node_sets(nodes_modified, node_sets_modified.at(i).at(1), node_sets_modified.at(i).back(), is_match
                                                             , &nodes_original.at(node_sets_original.at(k).at(0)));

            int match = best_match(nodes_modified, set, nodes_original, node_sets_original.at(k), SES_INSERT);

            if(match >= set.size()) continue;

            srcdiff_measure measure(nodes_original, nodes_modified, node_sets_original.at(k), set.at(match));
            int similarity, difference, text_original_length, text_modified_length;
            measure.compute_measures(similarity, difference, text_original_length, text_modified_length);

            if(check_nestable_predicate(similarity, difference, text_modified_length, text_original_length,
               set.at(match), nodes_modified, node_sets_modified, i, end_modified, nodes_original, node_sets_original, k, end_original))
              continue;

            if(nodes_original.at(node_sets_original.at(k).at(0))->name == "name" 
              && nodes_original.at(node_sets_original.at(k).at(0))->parent && (*nodes_original.at(node_sets_original.at(k).at(0))->parent)->name == "expr"
              && nodes_modified.at(node_sets_modified.at(i).at(0))->parent && (*nodes_modified.at(node_sets_modified.at(i).at(0))->parent)->name == "expr"
              && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1))
              continue;

            if(nodes_original.at(node_sets_original.at(k).at(0))->name == "name") {

                if(!nodes_original.at(node_sets_original.at(k).at(0))->parent || !nodes_modified.at(set.at(match).at(0))->parent)
                  continue;

                boost::optional<std::shared_ptr<srcml_node>> parent_original = nodes_original.at(node_sets_original.at(k).at(0))->parent;
                while(parent_original && (*parent_original)->name == "name")
                  parent_original = (*parent_original)->parent;

                boost::optional<std::shared_ptr<srcml_node>> parent_modified = nodes_modified.at(set.at(match).at(0))->parent;
                while(parent_modified && (*parent_modified)->name == "name")
                  parent_modified = (*parent_modified)->parent;

                if((*parent_original)->name != (*parent_modified)->name)
                  continue;

            }

          valid_nests_modified.push_back(k);

        }

        //start_nest_original = valid_nests.front();
        //end_nest_original = valid_nests.back() + 1;

        goto end_nest_check_modified;

      }

    }

  }

  end_nest_check_modified:

  /** @todo may need a more exact check to pick most optimal or another check 

    For now if only valid, less than or equal and do not cross, or cross and larger.

  */
  if(!valid_nests_original.empty() && (valid_nests_modified.empty()
   || (start_nest_original < valid_nests_modified.front() && valid_nests_original.back() < start_nest_modified)
   || (((start_nest_original >= valid_nests_modified.front() && start_nest_original <= valid_nests_modified.back())
        || (start_nest_modified >= valid_nests_original.front() && start_nest_modified <= valid_nests_original.back()))
      && (valid_nests_original.back() - valid_nests_original.front()) >= (valid_nests_modified.back() - valid_nests_modified.front())))) {

      start_nest_modified = valid_nests_original.front();
      end_nest_modified = valid_nests_original.back() + 1;
      operation = SES_DELETE;

  } else if(!valid_nests_modified.empty()) {

      start_nest_original = valid_nests_modified.front();
      end_nest_original = valid_nests_modified.back() + 1;
      operation = SES_INSERT;

  }

}

void srcdiff_nested::output() {

  srcdiff_whitespace whitespace(out);

  whitespace.output_prefix();

  if(operation == SES_DELETE) {

    unsigned int start_pos = node_sets_original.at(start_original).at(1);

    if(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name == "if" || out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name == "elseif") {

        while(!(out.get_nodes_original().at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
          && out.get_nodes_original().at(start_pos)->name == "then"))
          ++start_pos;

    } else if(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name == "while") {

        while(!(out.get_nodes_original().at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
          && out.get_nodes_original().at(start_pos)->name == "condition"))
          ++start_pos;

        ++start_pos;

    } else if(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name == "for") {

        while(!(out.get_nodes_original().at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
          && out.get_nodes_original().at(start_pos)->name == "control"))
          ++start_pos;

        ++start_pos;

    }

    size_t end_pos = node_sets_original.at(end_original - 1).back();
    if(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name == "elseif") {

      while(end_pos > start_pos && out.get_nodes_original().at(end_pos)->name != "if")
        --end_pos;

    }

    node_sets set = node_sets(out.get_nodes_original(),
                              start_pos, end_pos);

    node_sets nest_set(out.get_nodes_modified());

    for(int i = start_modified; i < end_modified; ++i)
        nest_set.push_back(node_sets_modified.at(i));

      output_change(start_pos, out.last_output_modified());

      whitespace.output_nested(SES_DELETE);

      srcdiff_diff diff(out, set, nest_set);
      diff.output();

      whitespace.output_nested(SES_DELETE);

      output_change(node_sets_original.at(end_original - 1).back() + 1, out.last_output_modified());

  } else {

    unsigned int start_pos = node_sets_modified.at(start_modified).at(1);

    if(out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))->name == "if" || out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))->name == "elseif") {

        while(!(out.get_nodes_modified().at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
          && out.get_nodes_modified().at(start_pos)->name == "then"))
          ++start_pos;

    } else if(out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))->name == "while") {

        while(!(out.get_nodes_modified().at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
          && out.get_nodes_modified().at(start_pos)->name == "condition"))
          ++start_pos;

        ++start_pos;

    } else if(out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))->name == "for") {

        while(!(out.get_nodes_modified().at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
          && out.get_nodes_modified().at(start_pos)->name == "control"))
          ++start_pos;

        ++start_pos;

    }

    size_t end_pos = node_sets_modified.at(end_modified - 1).back();
    if(out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))->name == "elseif") {

      while(end_pos > start_pos && out.get_nodes_modified().at(end_pos)->name != "if")
        --end_pos;

    }

    node_sets set = node_sets(out.get_nodes_modified(),
                              start_pos, end_pos);

    node_sets nest_set(out.get_nodes_original());

    for(int i = start_original; i < end_original; ++i)
        nest_set.push_back(node_sets_original.at(i));

      output_change(out.last_output_original(), start_pos);

      whitespace.output_nested(SES_INSERT);

      srcdiff_diff diff(out, nest_set, set);
      diff.output();

      whitespace.output_nested(SES_INSERT);

      output_change(out.last_output_original(), node_sets_modified.at(end_modified - 1).back() + 1);
  }

  //output_all(rbuf_original, rbuf_modified, wstate);

  //diff_original_start.properties = 0;
  //diff_modified_start.properties = 0;

}
