#include <srcdiff_nested.hpp>

#include <shortest_edit_script.h>
#include <srcDiffWhiteSpace.hpp>
#include <srcDiffChange.hpp>
#include <srcDiffOutput.hpp>
#include <srcdiff_diff.hpp>
#include <srcDiffUtility.hpp>
#include <srcDiffMeasure.hpp>

#include <string.h>

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

// diff attribute
extern xAttr diff_type;
extern const char * change;
extern const char * whitespace;

srcdiff_nested::srcdiff_nested(const srcdiff_many & diff, int start_old, int end_old, int start_new, int end_new, int operation) : srcdiff_many(diff), start_old(start_old), end_old(end_old), start_new(start_new), end_new(end_new), operation(operation) {}

int nest_id = 0;

struct nest_info {

  const char * type;

  const char * const * possible_nest_items;

};

// may need to change collection algorithm to gather full and nested of same typ             
const char * const basic_nest_types[]       = { "expr_stmt", "decl_stmt", "return", "comment",                                                                                                    0 };
const char * const block_nest_types[]       = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach",                                                          0 };
const char * const if_nest_types[]          = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach", "else", "elseif",                                        0 };
const char * const then_nest_types[]        = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach", "else", "elseif", "expr", "call", "operator", "literal", 0 };
const char * const else_nest_types[]        = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach", "expr", "call", "operator", "literal",                   0 };
const char * const while_nest_types[]       = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach",                                                          0 };
const char * const for_nest_types[]         = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach",                                                          0 };
const char * const for_control_nest_types[] = { "condition", "comment",                                                                                                                           0 };
const char * const function_nest_types[]    = { "expr_stmt", "decl_stmt", "if", "while", "for", "foreach",                                                                                        0 };
const char * const class_nest_types[]       = { "decl_stmt", "function_decl",                                                                                                                     0 };
const char * const struct_nest_types[]      = { "decl_stmt", "function_decl",                                                                                                                     0 };
const char * const union_nest_types[]       = { "decl_stmt", "function_decl",                                                                                                                     0 };
const char * const call_nest_types[]        = { "expr", "call", "operator", "literal", "name",                                                                                                    0 };
const char * const ternary_nest_types[]     = { "ternary", "call", "operator", "literal", "expr",                                                                                                 0 };
const char * const condition_nest_types[]   = { "expr", "call", "operator", "literal",                                                                                                            0 };
const char * const name_nest_types[]        = { "name",                                                                                                                                           0 };
const char * const try_nest_types[]         = { "expr_stmt", "decl_stmt", "return", "comment", "block", "if", "while", "for", "foreach", "else", "elseif", "try", "catch",                        0 };
const char * const extern_nest_types[]      = { "function_decl", "function", "struct", "struct_decl", "union", "union_decl",                                                       0 };
  
// tags that can have something nested in them (incomplete)    
const nest_info nesting[] = {   

  { "block",         block_nest_types       },
  { "if",            if_nest_types          },
  { "then",          then_nest_types        },
  { "elseif",        if_nest_types          },
  { "else",          else_nest_types        },
  { "while",         while_nest_types       },
  { "for",           for_nest_types         },
  { "foreach",       for_nest_types         },
  { "control",       for_control_nest_types },
  { "function",      function_nest_types    },
  { "class",         class_nest_types       },
  { "struct",        struct_nest_types      },
  { "union",         union_nest_types       },
  { "call",          call_nest_types        },
  { "argument_list", call_nest_types        },
  { "argument",      call_nest_types        },
  { "expr",          call_nest_types        },
  { "ternary",       ternary_nest_types     },
  { "condition",     condition_nest_types   },
  { "name",          name_nest_types        },
  { "try",           try_nest_types         },
  { "catch",         try_nest_types         },
  { "extern",        extern_nest_types      },
  { 0, 0 }

};

int is_block_type(node_set * structure, std::vector<xNodePtr> & nodes) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return -1;

  if(strcmp(nodes.at(structure->at(0))->ns->href, "http://www.sdml.info/srcML/src") != 0)
    return -1;

  for(int i = 0; nesting[i].type; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, nesting[i].type) == 0)
      return i;

  return -1;
}

bool is_nest_type(node_set * structure, std::vector<xNodePtr> & nodes
                           , node_set * structure_other, std::vector<xNodePtr> & nodes_other, int type_index) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp(nodes.at(structure->at(0))->ns->href, "http://www.sdml.info/srcML/src") != 0)
    return true;

  for(int i = 0; nesting[type_index].possible_nest_items[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, nesting[type_index].possible_nest_items[i]) == 0
       && srcdiff_nested::has_internal_structure(structure_other, nodes_other, (const char *)nodes.at(structure->at(0))->name))
      return true;

  return false;
}



bool srcdiff_nested::has_internal_structure(node_set * structure, std::vector<xNodePtr> & nodes, const char * type) {

  for(unsigned int i = 1; i < structure->size(); ++i)
    if((xmlReaderTypes)nodes.at(structure->at(i))->type == XML_READER_TYPE_ELEMENT
              && strcmp((const char *)nodes.at(structure->at(i))->name, type) == 0)
      return true;

  return false;
}

bool complete_nestable(node_sets & structure_one, std::vector<xNodePtr> & nodes_one
                  , node_set * structure_two, std::vector<xNodePtr> & nodes_two) {

  unsigned int num_nest = 0;

  for(unsigned int i = 0; i < structure_one.size(); ++i) {

    if(srcdiff_nested::is_nestable(structure_one.at(i), nodes_one, structure_two, nodes_two))
       ++num_nest;

  }

  return num_nest == structure_one.size();

}

bool is_match(const xNodePtr node, const void * context) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT && node_compare(node, (xNodePtr)context) == 0;

}

int best_match(std::vector<xNodePtr> & nodes, node_sets & set
               , std::vector<xNodePtr> & nodes_match, node_set * match, int operation) {

  int match_pos = set.size();
  int match_similarity = 0;

  if(set.size() > 0) {

    if(!((set.at(0)->size() > match->size() && (set.at(0)->size()) > (4 * match->size()))
      || (match->size() > set.at(0)->size() && (match->size()) > (4 * set.at(0)->size())))) {

      match_pos = 0;
      match_similarity = compute_similarity(nodes, set.at(0), nodes_match, match);

    }

  } else
    return 1;

  for(unsigned int i = 1; i < set.size(); ++i) {

    if(set.at(i)->size() > match->size() && (set.at(i)->size()) > (4 * match->size()))
      continue;

    if(match->size() > set.at(i)->size() && (match->size()) > (4 * set.at(i)->size()))
      continue;

    int similarity = compute_similarity(nodes, set.at(i), nodes_match, match);
    if(similarity > match_similarity) {

      match_pos = i;
      match_similarity = similarity;

    }

  }

  return match_pos;

}

bool is_nestable_internal(node_set * structure_one, std::vector<xNodePtr> & nodes_one
                 , node_set * structure_two, std::vector<xNodePtr> & nodes_two) {

  int block = is_block_type(structure_two, nodes_two);

  if(block == -1)
    return false;

  // may want to switch to a list of disallows such as can not nest function block
  /** should this be here or in is better nest */
  // if(strcmp(nodes_one.at(structure_one->at(0))->name, "block") == 0
  //   && strcmp(nodes_two.at(structure_two->at(0))->name, "block") == 0
  //   && nodes_one.at(structure_one->at(0))->parent && nodes_two.at(structure_two->at(0))->parent
  //   && strcmp(nodes_one.at(structure_one->at(0))->parent, nodes_two.at(structure_two->at(0))->parent) == 0)
  //   return false;

  /** Only can nest a block into another block if it's parent is a block */
  bool is_block = strcmp(nodes_one.at(structure_one->at(0))->name, "block") == 0
   && strcmp(nodes_two.at(structure_two->at(0))->name, "block") == 0;
  bool parent_is_block = nodes_one.at(structure_one->at(0))->parent
   && strcmp(nodes_one.at(structure_one->at(0))->parent, "block") == 0;
  if(is_block && !parent_is_block) return false;

  /** @todo This is a more general version.  Need to see if better. */
  // bool is_block = strcmp(nodes_one.at(structure_one->at(0))->name, "block") == 0;
  // bool is_then = strcmp(nodes_two.at(structure_two->at(0))->name, "then") == 0;
  // bool parent_is_block = nodes_one.at(structure_one->at(0))->parent
  //  && strcmp(nodes_one.at(structure_one->at(0))->parent, "block") == 0;
  // bool is_else_to_elseif = nodes_one.at(structure_one->at(0))->parent
  //  && strcmp(nodes_one.at(structure_one->at(0))->parent, "else") == 0
  //  && strcmp(nodes_two.at(structure_two->at(0))->name, "if") == 0;
  // if(is_block && !parent_is_block && !is_then && !is_else_to_elseif) return false;

  if(is_nest_type(structure_one, nodes_one, structure_two, nodes_two, block)) {

    return true;

  }

  return false;
}

bool srcdiff_nested::is_same_nestable(node_set * structure_one, std::vector<xNodePtr> & nodes_one
                      , node_set * structure_two, std::vector<xNodePtr> & nodes_two) {

  if(!is_nestable_internal(structure_one, nodes_one, structure_two, nodes_two))
    return false;

  //unsigned int similarity = compute_similarity(nodes_one, structure_one, nodes_two, structure_two);

  node_sets set = node_sets(nodes_two, structure_two->at(1), structure_two->back(), is_match
                                                             , nodes_one.at(structure_one->at(0)));

  unsigned int match = best_match(nodes_two, set, nodes_one, structure_one, SESDELETE);

  if(match >= set.size())
    return false;

  int match_similarity, match_difference, size_one, size_match;
  compute_measures(nodes_one, structure_one, nodes_two, set.at(match), match_similarity, match_difference, size_one, size_match);
  int similarity, difference, size_two;
  compute_measures(nodes_one, structure_one, nodes_two, structure_two, similarity, difference, size_one, size_two);

  double min_size = size_one < size_two ? size_one : size_two;
  double match_min_size = size_one < size_match ? size_one : size_match;

  return (match_similarity >= similarity && match_difference <= difference) 
  || (match_min_size > 50 && min_size > 50 && (match_min_size / match_similarity) < (0.9 * (min_size / similarity))
//   && match_difference < 1.5 * difference
    && !srcdiff_nested::reject_match_nested(match_similarity, match_difference, size_match, size_one, nodes_two, set.at(match), nodes_one, structure_one));

}

bool srcdiff_nested::is_nestable(node_set * structure_one, std::vector<xNodePtr> & nodes_one
                 , node_set * structure_two, std::vector<xNodePtr> & nodes_two) {

  if(node_compare(nodes_one.at(structure_one->at(0)), nodes_two.at(structure_two->at(0))) == 0)
    return is_same_nestable(structure_one, nodes_one, structure_two, nodes_two);
  else
    return is_nestable_internal(structure_one, nodes_one, structure_two, nodes_two);

}

bool is_better_nest_no_recursion(std::vector<xNodePtr> & nodes_outer, node_set * node_set_outer,
                    std::vector<xNodePtr> & nodes_inner, node_set * node_set_inner,
                    int similarity, int difference, int text_outer_length, int text_inner_length) {

    if(srcdiff_nested::is_nestable(node_set_inner, nodes_inner, node_set_outer, nodes_outer)) {

      node_sets set = node_sets(nodes_outer, node_set_outer->at(1), node_set_outer->back(), is_match
                                                             , nodes_inner.at(node_set_inner->at(0)));

      int match = best_match(nodes_outer, set, nodes_inner, node_set_inner, SESDELETE);

      if(match < set.size()) {

        int nest_similarity, nest_difference, nest_text_outer_length, nest_text_inner_length;
        compute_measures(nodes_outer, set.at(match), nodes_inner, node_set_inner,
          nest_similarity, nest_difference, nest_text_outer_length, nest_text_inner_length);

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

bool is_better_nest(std::vector<xNodePtr> & nodes_outer, node_set * node_set_outer,
                    std::vector<xNodePtr> & nodes_inner, node_set * node_set_inner,
                    int similarity, int difference, int text_outer_length, int text_inner_length) {
// parents and children same do not nest.
    if(srcdiff_nested::is_nestable(node_set_inner, nodes_inner, node_set_outer, nodes_outer)) {

      node_sets set = node_sets(nodes_outer, node_set_outer->at(1), node_set_outer->back(), is_match
                                                             , nodes_inner.at(node_set_inner->at(0)));

      int match = best_match(nodes_outer, set, nodes_inner, node_set_inner, SESDELETE);

      if(match < set.size()) {

        int nest_similarity, nest_difference, nest_text_outer_length, nest_text_inner_length;
        compute_measures(nodes_outer, set.at(match), nodes_inner, node_set_inner,
          nest_similarity, nest_difference, nest_text_outer_length, nest_text_inner_length);

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

bool srcdiff_nested::is_better_nested(std::vector<xNodePtr> & nodes_old, node_sets * node_sets_old, int start_pos_old,
                    std::vector<xNodePtr> & nodes_new, node_sets * node_sets_new, int start_pos_new,
                    int similarity, int difference, int text_old_length, int text_new_length) {

  for(int pos = start_pos_old; pos < node_sets_old->size(); ++pos) {

    if(is_better_nest(nodes_old, node_sets_old->at(pos), nodes_new, node_sets_new->at(start_pos_new), similarity, difference, text_old_length, text_new_length))
      return true;

  }

  for(int pos = start_pos_new; pos < node_sets_new->size(); ++pos) {

    if(is_better_nest(nodes_new, node_sets_new->at(pos), nodes_old, node_sets_old->at(start_pos_old), similarity, difference, text_old_length, text_new_length))
      return true;

  }

  return false;

}

bool srcdiff_nested::reject_match_nested(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new) {

  int old_pos = set_old->at(0);
  int new_pos = set_new->at(0);

  std::string old_tag = nodes_old.at(old_pos)->name;
  std::string new_tag = nodes_new.at(new_pos)->name;

  if(old_tag != new_tag && !is_interchangeable_match(old_tag, new_tag)) return true;

  if(old_tag == "then" || old_tag == "block" || old_tag == "comment"
    || old_tag == "literal" || old_tag == "operator" || old_tag == "modifier"
    || old_tag == "expr" || old_tag == "name") {


    return reject_similarity(similarity, difference, text_old_length, text_new_length,
      nodes_old, set_old, nodes_new, set_new);

    // int min_child_length = children_length_old < children_length_new ? children_length_old : children_length_new;
    // int max_child_length = children_length_old < children_length_new ? children_length_new : children_length_old;

    // if(min_child_length > 1) { 

    //   if(min_child_length < 3 && 2 * syntax_similarity >= min_child_length && syntax_difference <= min_child_length)
    //     return false;

    //   if(min_child_length > 2 && 3 * syntax_similarity >= 2 * min_child_length && syntax_difference <= min_child_length) 
    //     return false;

    // }

    // int min_size = text_old_length < text_new_length ? text_old_length : text_new_length;
    // int max_size = text_old_length < text_new_length ? text_new_length : text_old_length;

    // if(min_size <= 2)
    //   return 2 * similarity < min_size || difference > min_size;
    // else if(min_size <= 3)
    //   return 3 * similarity < 2 * min_size || difference > min_size;
    // else
    //   return 10 * similarity < 7 * min_size || difference > min_size;

  } else {

    return reject_match(similarity, difference, text_old_length, text_new_length, nodes_old, set_old, nodes_new, set_new);

  }

}

void srcdiff_nested::check_nestable(node_sets * node_sets_old, std::vector<xNodePtr> & nodes_old, int start_old, int end_old
                 , node_sets * node_sets_new, std::vector<xNodePtr> & nodes_new, int start_new, int end_new
                 , int & start_nest_old, int & end_nest_old, int & start_nest_new, int & end_nest_new
                 , int & operation) {

  start_nest_old = start_old;  
  end_nest_old = start_old;  

  start_nest_new = start_new;  
  end_nest_new = start_new;  

  std::vector<int> valid_nests_old;
  std::vector<int> valid_nests_new;

  for(int i = start_old; i < end_old; ++i) {

    if(nodes_old.at(node_sets_old->at(i)->at(0))->move) continue;

    for(int j = start_new; j < end_new; ++j) {

      if(nodes_new.at(node_sets_new->at(j)->at(0))->move) continue;

      if(is_nestable(node_sets_new->at(j), nodes_new, node_sets_old->at(i), nodes_old)) {

        node_sets set = node_sets(nodes_old, node_sets_old->at(i)->at(1), node_sets_old->at(i)->back(), is_match
                                                             , nodes_new.at(node_sets_new->at(j)->at(0)));

        int match = best_match(nodes_old, set, nodes_new, node_sets_new->at(j), SESDELETE);

        if(match >= set.size()) continue;

        int similarity, difference, text_old_length, text_new_length;
        compute_measures(nodes_old, set.at(match), nodes_new, node_sets_new->at(j),
          similarity, difference, text_old_length, text_new_length);

        if(reject_match_nested(similarity, difference, text_old_length, text_new_length,
          nodes_old, set.at(match), nodes_new, node_sets_new->at(j))
          || is_better_nest(nodes_new, node_sets_new->at(j), nodes_old, node_sets_old->at(i), similarity, difference, text_new_length, text_old_length)
          || (i + 1 < end_old && is_better_nest(nodes_old, node_sets_old->at(i + 1), nodes_new, node_sets_new->at(j), similarity, difference, text_old_length, text_new_length))
          || (j + 1 < end_new && is_better_nest(nodes_new, node_sets_new->at(j + 1), nodes_old, node_sets_old->at(i), similarity, difference, text_new_length, text_old_length))
          )
          continue;

        if(strcmp(nodes_new.at(node_sets_new->at(j)->at(0))->name, "name") == 0
          && strcmp(nodes_new.at(node_sets_new->at(j)->at(0))->parent, "expr") == 0
          && strcmp(nodes_old.at(node_sets_old->at(i)->at(0))->parent, "expr") == 0 && ((end_old - start_old) > 1 || (end_new - start_new) > 1))
          continue;

        valid_nests_old.push_back(j);

        start_nest_old = i;
        end_nest_old = i + 1;

        for(int k = j + 1; k < end_new; ++k) {

          if(nodes_new.at(node_sets_new->at(k)->at(0))->move) continue;

          if(!is_nestable(node_sets_new->at(k), nodes_new, node_sets_old->at(i), nodes_old)) continue;

          node_sets set = node_sets(nodes_old, node_sets_old->at(i)->at(1), node_sets_old->at(i)->back(), is_match
                                                               , nodes_new.at(node_sets_new->at(k)->at(0)));

          int match = best_match(nodes_old, set, nodes_new, node_sets_new->at(k), SESDELETE);

          if(match >= set.size()) continue;

          int similarity, difference, text_old_length, text_new_length;
          compute_measures(nodes_old, set.at(match), nodes_new, node_sets_new->at(k),
            similarity, difference, text_old_length, text_new_length);

          if(reject_match_nested(similarity, difference, text_old_length, text_new_length,
            nodes_old, set.at(match), nodes_new, node_sets_new->at(k)))
            continue;

          if(strcmp(nodes_new.at(node_sets_new->at(k)->at(0))->name, "name") == 0
            && strcmp(nodes_new.at(node_sets_new->at(k)->at(0))->parent, "expr") == 0
            && strcmp(nodes_old.at(node_sets_old->at(i)->at(0))->parent, "expr") == 0 && ((end_old - start_old) > 1 || (end_new - start_new) > 1))
            continue;

          valid_nests_old.push_back(k);

        }

        goto end_nest_check_old;

      }

    }

  }

  end_nest_check_old:

  for(int i = start_new; i < end_new; ++i) {

    if(nodes_new.at(node_sets_new->at(i)->at(0))->move) continue;

    for(int j = start_old; j < end_old; ++j) {

      if(nodes_old.at(node_sets_old->at(j)->at(0))->move) continue;

      if(is_nestable(node_sets_old->at(j), nodes_old, node_sets_new->at(i), nodes_new)) {

        node_sets set = node_sets(nodes_new, node_sets_new->at(i)->at(1), node_sets_new->at(i)->back(), is_match
                                                             , nodes_old.at(node_sets_old->at(j)->at(0)));

        int match = best_match(nodes_new, set, nodes_old, node_sets_old->at(j), SESINSERT);

        if(match >= set.size()) continue;

        int similarity, difference, text_old_length, text_new_length;
        compute_measures(nodes_old, node_sets_old->at(j), nodes_new, set.at(match),
          similarity, difference, text_old_length, text_new_length);

        if(reject_match_nested(similarity, difference, text_old_length, text_new_length,
          nodes_old, node_sets_old->at(j), nodes_new, set.at(match))
          || (i + 1 < end_new && is_better_nest(nodes_new, node_sets_new->at(i + 1), nodes_old, node_sets_old->at(j), similarity, difference, text_new_length, text_old_length)))
          continue;

        if(strcmp(nodes_old.at(node_sets_old->at(j)->at(0))->name, "name") == 0
          && strcmp(nodes_old.at(node_sets_old->at(j)->at(0))->parent, "expr") == 0
          && strcmp(nodes_new.at(node_sets_new->at(i)->at(0))->parent, "expr") == 0 && ((end_old - start_old) > 1 || (end_new - start_new) > 1))
          continue;

        valid_nests_new.push_back(j);

        start_nest_new = i;
        end_nest_new = i + 1;

        for(int k = j + 1; k < end_old; ++k) {

          if(nodes_old.at(node_sets_old->at(k)->at(0))->move) continue;
        
          if(!is_nestable(node_sets_old->at(k), nodes_old, node_sets_new->at(i), nodes_new)) continue;

            node_sets set = node_sets(nodes_new, node_sets_new->at(i)->at(1), node_sets_new->at(i)->back(), is_match
                                                             , nodes_old.at(node_sets_old->at(k)->at(0)));

            int match = best_match(nodes_new, set, nodes_old, node_sets_old->at(k), SESINSERT);

            if(match >= set.size()) continue;

            int similarity, difference, text_old_length, text_new_length;
            compute_measures(nodes_old, node_sets_old->at(k), nodes_new, set.at(match),
              similarity, difference, text_old_length, text_new_length);

            if(reject_match_nested(similarity, difference, text_old_length, text_new_length,
              nodes_old, node_sets_old->at(k), nodes_new, set.at(match)))
              continue;

            if(strcmp(nodes_old.at(node_sets_old->at(k)->at(0))->name, "name") == 0 
              && strcmp(nodes_old.at(node_sets_old->at(k)->at(0))->parent, "expr") == 0
              && strcmp(nodes_new.at(node_sets_new->at(i)->at(0))->parent, "expr") == 0 && ((end_old - start_old) > 1 || (end_new - start_new) > 1))
              continue;

          valid_nests_new.push_back(k);

        }

        //start_nest_old = valid_nests.front();
        //end_nest_old = valid_nests.back() + 1;

        goto end_nest_check_new;

      }

    }

  }

  end_nest_check_new:

  /** @todo may need a more exact check to pick most optimal or another check 

    For now if only valid, less than or equal and do not cross, or cross and larger.

  */
  if(!valid_nests_old.empty() && (valid_nests_new.empty()
   || ((start_nest_old - start_old) <= (start_nest_new - start_new) 
      && start_nest_old < valid_nests_new.front() && valid_nests_old.back() < start_nest_new)
   || (((start_nest_old >= valid_nests_new.front() && start_nest_old <= valid_nests_new.back())
        || (start_nest_new >= valid_nests_old.front() && start_nest_new <= valid_nests_old.back()))
      && (valid_nests_old.back() - valid_nests_old.front()) >= (valid_nests_new.back() - valid_nests_new.front())))) {

      start_nest_new = valid_nests_old.front();
       end_nest_new = valid_nests_old.back() + 1;
       operation = SESDELETE;

  } else if(!valid_nests_new.empty()) {

      start_nest_old = valid_nests_new.front();
      end_nest_old = valid_nests_new.back() + 1;
      operation = SESINSERT;

  }

}

void set_nestable(node_sets * node_sets_old, std::vector<xNodePtr> & nodes_old, int start_old, int end_old
                 , node_sets * node_sets_new, std::vector<xNodePtr> & nodes_new, int start_new, int end_new) {

  ++nest_id;

  for(int i = start_old; i < end_old; ++i) {

    nodes_old.at(node_sets_old->at(i)->at(0))->nest = nest_id;
    nodes_old.at(node_sets_old->at(i)->back())->nest = nest_id;

  }

  for(int i = start_new; i < end_new; ++i) {

    nodes_new.at(node_sets_new->at(i)->at(0))->nest = nest_id;
    nodes_new.at(node_sets_new->at(i)->back())->nest = nest_id;

  }

}

void clear_nestable(node_set * structure_one, std::vector<xNodePtr> & nodes_one
                 , node_set * structure_two, std::vector<xNodePtr> & nodes_two) {

  nodes_one.at(structure_one->at(0))->nest = 0;
  nodes_one.at(structure_one->back())->nest = 0;
  nodes_two.at(structure_two->at(0))->nest = 0;
  nodes_two.at(structure_two->back())->nest = 0;

}

void clear_nestable(node_sets * node_sets_old, std::vector<xNodePtr> & nodes_old, int start_old, int end_old
                 , node_sets * node_sets_new, std::vector<xNodePtr> & nodes_new, int start_new, int end_new) {

  for(int i = start_old; i < end_old; ++i) {

    nodes_old.at(node_sets_old->at(i)->at(0))->nest = 0;
    nodes_old.at(node_sets_old->at(i)->back())->nest = 0;

  }

  for(int i = start_new; i < end_new; ++i) {

    nodes_new.at(node_sets_new->at(i)->at(0))->nest = 0;
    nodes_new.at(node_sets_new->at(i)->back())->nest = 0;

  }

}

void srcdiff_nested::output() {

  clear_nestable(node_sets_old, rbuf_old.nodes, start_old, end_old, node_sets_new, rbuf_new.nodes, start_new, end_new);

  output_white_space_prefix(rbuf_old, rbuf_new, wstate);

  if(operation == SESDELETE) {

    unsigned int end_pos = node_sets_old->at(start_old)->at(1);

    if(strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "if") == 0 || strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "elseif") == 0) {

        while(!(rbuf_old.nodes.at(end_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
          && strcmp(rbuf_old.nodes.at(end_pos)->name, "then") == 0))

          ++end_pos;

    } else if(strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "while") == 0) {

        while(!(rbuf_old.nodes.at(end_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
          && strcmp(rbuf_old.nodes.at(end_pos)->name, "condition") == 0))
          ++end_pos;

        ++end_pos;

    } else if(strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "for") == 0) {

        while(!(rbuf_old.nodes.at(end_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
          && strcmp(rbuf_old.nodes.at(end_pos)->name, "control") == 0))
          ++end_pos;

        ++end_pos;
        ++end_pos;

    }

    node_sets set = node_sets(rbuf_old.nodes,
      //node_sets_old->at(start_old)->at(1),
      end_pos,
      node_sets_old->at(end_old - 1)->back());

    node_sets nest_set(rbuf_new.nodes);

    for(int i = start_new; i < end_new; ++i)
        nest_set.push_back(new node_set(*node_sets_new->at(i)));

      output_change(rbuf_old, end_pos, rbuf_new, rbuf_new.last_output, wstate);

      output_white_space_nested(rbuf_old, rbuf_new, SESDELETE, wstate);

      srcdiff_diff diff(rbuf_old, rbuf_new, wstate, &set, &nest_set);
      diff.output();

      output_white_space_nested(rbuf_old, rbuf_new, SESDELETE, wstate);

      output_change(rbuf_old, node_sets_old->at(end_old - 1)->back() + 1, rbuf_new, rbuf_new.last_output, wstate);

  } else {

    unsigned int end_pos = node_sets_new->at(start_new)->at(1);

    if(strcmp(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0))->name, "if") == 0 || strcmp(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0))->name, "elseif") == 0) {

        while(!(rbuf_new.nodes.at(end_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
          && strcmp(rbuf_new.nodes.at(end_pos)->name, "then") == 0))
          ++end_pos;

    } else if(strcmp(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0))->name, "while") == 0) {

        while(!(rbuf_new.nodes.at(end_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
          && strcmp(rbuf_new.nodes.at(end_pos)->name, "condition") == 0))
          ++end_pos;

        ++end_pos;

    } else if(strcmp(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0))->name, "for") == 0) {

        while(!(rbuf_new.nodes.at(end_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
          && strcmp(rbuf_new.nodes.at(end_pos)->name, "control") == 0))
          ++end_pos;

        ++end_pos;
        ++end_pos;

    }

    node_sets set = node_sets(rbuf_new.nodes,
      //node_sets_old->at(start_old)->at(1),
      end_pos,
      node_sets_new->at(end_new - 1)->back());

    node_sets nest_set(rbuf_old.nodes);

    for(int i = start_old; i < end_old; ++i)
        nest_set.push_back(new node_set(*node_sets_old->at(i)));

      output_change(rbuf_old, rbuf_old.last_output, rbuf_new, end_pos, wstate);

      output_white_space_nested(rbuf_old, rbuf_new, SESINSERT, wstate);

      srcdiff_diff diff(rbuf_old, rbuf_new, wstate, &nest_set, &set);
      diff.output();

      output_white_space_nested(rbuf_old, rbuf_new, SESINSERT, wstate);

      output_change(rbuf_old, rbuf_old.last_output, rbuf_new, node_sets_new->at(end_new - 1)->back() + 1, wstate);
  }

  //output_white_space_all(rbuf_old, rbuf_new, wstate);

  //diff_old_start.properties = 0;
  //diff_new_start.properties = 0;

}
