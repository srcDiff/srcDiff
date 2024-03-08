#include <srcdiff_nested.hpp>

#include <srcdiff_constants.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_text_measure.hpp>
#include <srcdiff_match.hpp>
#include <shortest_edit_script.h>
#include <type_query.hpp>

#include <algorithm>
#include <cstring>

srcdiff_nested::srcdiff_nested(const srcdiff_many & diff, int start_original, int end_original, int start_modified, int end_modified, int operation)
  : srcdiff_many(diff), start_original(start_original), end_original(end_original), start_modified(start_modified), end_modified(end_modified), operation(operation) {}

int nest_id = 0;

struct nest_info {

  const char * type;

  const char * const * possible_nest_items;

};

// may need to change collection algorithm to gather full and nested of same type           
const char * const block_nest_types[]        = { "goto", "expr_stmt", "decl_stmt", "return", "comment", "block",
                                                 "if_stmt", "if", "while", "for", "foreach", "else", "switch", "do",
                                                 "try", "catch", "finally", "synchronized", "continue", "break", "goto", 0 };

const char * const ternary_then_nest_types[] = { "goto", "expr_stmt", "decl_stmt", "return", "comment", "block",
                                                 "if", "while", "for", "foreach", "else", "elseif", "switch", "do",
                                                 "try", "catch", "finally", "synchronized",
                                                 "expr", "call", "operator", "literal", "continue", "break", "goto", 0 };

const char * const else_nest_types[]         = { "goto", "expr_stmt", "decl_stmt", "return", "comment", "block",
                                                "if_stmt", "if", "while", "for", "foreach", "switch", "do",
                                                "try", "catch", "finally", "synchronized",
                                                "expr", "call", "operator", "literal", "continue", "break", "goto", 0 };

const char * const class_nest_types[]        = { "function",
                                                 "constructor", 
                                                 "destructor",
                                                 "class", 
                                                 "struct", 
                                                 "union",
                                                 "enum",
                                                 "decl_stmt",                                                
                                                 "function_decl",
                                                 "constructor_decl", 
                                                 "destructor_decl",
                                                 "class_decl",
                                                 "struct_decl",
                                                 "union_decl",
                                                 "enum_decl",
                                                 "typedef",
                                                 0 };

const char * const extern_nest_types[]       = { "decl_stmt", "function_decl", "function", "class", "class_decl",
                                                 "struct", "struct_decl", "union", "union_decl", "typedef", "using", 0 };
const char * const for_control_nest_types[]  = { "condition", "comment",                                             0 };
const char * const call_nest_types[]         = { "expr", "call", "operator", "literal", "name",                      0 };
const char * const ternary_nest_types[]      = { "ternary", "call", "operator", "literal", "expr", "name",           0 };
const char * const condition_nest_types[]    = { "expr", "call", "operator", "literal", "name",                      0 };
const char * const name_nest_types[]         = { "name",                                                             0 };
const char * const decl_nest_types[]         = { "expr",                                                             0 };
const char * const static_nest_types[]       = { "decl_stmt",                                                        0 };

// tags that can have something nested in them (incomplete)    
const nest_info nesting[] = {   

  { "block",         block_nest_types        },
  { "block_content", block_nest_types        },
  { "if_stmt",       block_nest_types        },
  { "if",            block_nest_types        },
  { "then",          ternary_then_nest_types },
  { "else",          else_nest_types         },
  { "while",         block_nest_types        },
  { "for",           block_nest_types        },
  { "foreach",       block_nest_types        },
  { "control",       for_control_nest_types  },
  { "function",      block_nest_types        },

  { "class",         class_nest_types        },
  { "struct",        class_nest_types        },
  { "enum",          class_nest_types        },
  { "public",        class_nest_types        },
  { "private",       class_nest_types        },
  { "protected",     class_nest_types        },

  { "call",          call_nest_types         },
  { "argument_list", call_nest_types         },
  { "argument",      call_nest_types         },
  { "expr",          call_nest_types         },
  { "ternary",       ternary_nest_types      },
  { "condition",     condition_nest_types    },
  { "name",          name_nest_types         },
  { "try",           block_nest_types        },
  { "catch",         block_nest_types        },
  { "extern",        extern_nest_types       },
  { "decl",          decl_nest_types         },
  { "init",          decl_nest_types         },

  // Java
  { "static",        static_nest_types       },
  { "synchronized",  block_nest_types        },
  { "finally",       block_nest_types        },
  { 0, 0 }

};

int is_block_type(std::shared_ptr<const construct> & structure) {

  if(structure->root_term()->get_type() != srcML::node_type::START)
    return -1;

  if(structure->root_term()->get_namespace()->get_uri() != SRCML_SRC_NAMESPACE_HREF)
    return -1;

  for(int i = 0; nesting[i].type; ++i)
    if(structure->root_term_name() == nesting[i].type)
      return i;

  return -1;
}

bool has_internal_structure(std::shared_ptr<const construct> & structure, const std::optional<std::string> & type) {

  if(!type) return false;

  for(unsigned int i = 1; i < structure->size(); ++i) {
    if(structure->term(i)->get_type() == srcML::node_type::START
              && structure->term(i)->get_name() == type)
      return true;
  }

  return false;
}

bool is_nest_type(std::shared_ptr<const construct> & structure,
                  std::shared_ptr<const construct> & structure_other,
                  int type_index) {

  if(structure->root_term()->get_type() != srcML::node_type::START)
    return false;

  if(structure->root_term()->get_namespace()->get_uri() != SRCML_SRC_NAMESPACE_HREF)
    return true;

  for(int i = 0; nesting[type_index].possible_nest_items[i]; ++i) {
    if(structure->root_term_name() == nesting[type_index].possible_nest_items[i]
       && has_internal_structure(structure_other, structure->root_term_name()))
      return true;
  }

  return false;
}

bool is_nestable_internal(std::shared_ptr<const construct> & structure_one,
                          std::shared_ptr<const construct> & structure_two) {

  int block = is_block_type(structure_two);

  if(block == -1)
    return false;

  /** Only can nest a block into another block if it's parent is a block */
  bool is_block = structure_one->root_term_name() == "block" && structure_two->root_term_name() == "block";
  bool parent_is_block = structure_one->root_term()->get_parent() && (*structure_one->root_term()->get_parent())->get_name() == "block";
  if(is_block && !parent_is_block) return false;

  if(is_nest_type(structure_one, structure_two, block)) {

    return true;

  }

  return false;
}

bool srcdiff_nested::is_same_nestable(std::shared_ptr<const construct> structure_one,
                                      std::shared_ptr<const construct> structure_two) {

  if(!is_nestable_internal(structure_one, structure_two)) return false;

  std::shared_ptr<const construct> best_match = structure_two->find_best_descendent(structure_one);
  if(!best_match) return false;

  srcdiff_text_measure match_measure(*structure_one, *best_match);
  match_measure.compute();

  srcdiff_text_measure measure(*structure_one, *structure_two);
  measure.compute();

  double min_size = measure.min_length();
  double match_min_size = std::min(measure.original_length(), match_measure.modified_length());

  return (match_measure.similarity() >= measure.similarity() && match_measure.difference() <= measure.difference()) 
  || (match_min_size > 50 && min_size > 50 && (match_min_size / match_measure.similarity()) < (0.9 * (min_size / measure.similarity()))
    && best_match->can_nest(*structure_one));

}

bool srcdiff_nested::is_nestable(std::shared_ptr<const construct> structure_one,
                                 std::shared_ptr<const construct> structure_two) {

  if(*structure_one->root_term() == *structure_two->root_term())
    return is_same_nestable(structure_one, structure_two);
  else
    return is_nestable_internal(structure_one, structure_two);

}

bool has_compound_inner(std::shared_ptr<const construct> & node_set_outer) {

  if(node_set_outer->root_term()->is_simple()) return false;

  for(unsigned int i = 1; i < node_set_outer->size(); ++i) {
    if(node_set_outer->term(i)->get_type() == srcML::node_type::START
      && node_set_outer->term(i)->get_name() == "name" && !node_set_outer->term(i)->is_simple())
      return true;
  }

  return false;

}

bool is_better_nest(std::shared_ptr<const construct> node_set_outer,
                    std::shared_ptr<const construct> node_set_inner,
                    const srcdiff_measure & measure, bool recurse = true) {

  // do not nest compound name in simple or anything into something that is not compound
  if(node_set_outer->root_term_name() == "name" && node_set_inner->root_term_name() == "name"
    &&    (node_set_outer->root_term()->is_simple()
      || (!node_set_inner->root_term()->is_simple() && !has_compound_inner(node_set_outer)))) return false;

  // parents and children same do not nest.
  if(srcdiff_nested::is_nestable(node_set_inner, node_set_outer)) {

    std::shared_ptr<const construct> best_match = node_set_outer->find_best_descendent(node_set_inner);

    if(best_match) {

      srcdiff_text_measure match_measure(*best_match, *node_set_inner);
      match_measure.compute();

      double min_size = measure.min_length();
      double nest_min_size = match_measure.min_length();

      /** in addition to being a valid match
          must have > or = similarity < or = difference or a better ratio of size to similarity
      */
      if((match_measure.similarity() >= measure.similarity() && match_measure.difference() <= measure.difference())
       || ((nest_min_size / match_measure.similarity()) < (min_size / measure.similarity())
        // old code used node_set_outer (i.e., is it interchangeable) this seemed wrong
        // fixes test case, but it failed because interchange not implemented (passes now)
        // that interchange implemented
          && node_set_inner->can_nest(*best_match))
       ) {
        // check if other way is better
        return recurse? !is_better_nest(node_set_inner, node_set_outer, match_measure, false) : true;
      }

    }

  }

  return false;

}

bool srcdiff_nested::is_better_nested(const construct::construct_list & construct_list_original, int start_pos_original,
                                      const construct::construct_list & construct_list_modified, int start_pos_modified) {

  const srcdiff_measure & measure = *construct_list_original.at(start_pos_original)
                            ->measure(*construct_list_modified.at(start_pos_modified));

  // comparison of signed vs unsigned, pos is signed while size is a size_t aka long unsigned int
  for(int pos = start_pos_original; pos < construct_list_original.size(); ++pos) {

    int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
    check_nestable(construct_list_original, pos, pos + 1
                 , construct_list_modified, start_pos_modified, start_pos_modified + 1
                 , start_nest_original, end_nest_original, start_nest_modified, end_nest_modified
                 , operation);
    if(operation == SES_COMMON) continue;
    if(is_better_nest(construct_list_original.at(pos), construct_list_modified.at(start_pos_modified), measure)) {
      return true;
    }

  }

  for(int pos = start_pos_modified; pos < construct_list_modified.size(); ++pos) {

    int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
    check_nestable(construct_list_original, start_pos_original, start_pos_original + 1
                 , construct_list_modified, pos, pos + 1
                 , start_nest_original, end_nest_original, start_nest_modified, end_nest_modified
                 , operation);
    if(operation == SES_COMMON) continue;
    if(is_better_nest(construct_list_modified.at(pos), construct_list_original.at(start_pos_original), measure)) {
      return true;
    }

  }

  return false;

}

bool srcdiff_nested::is_decl_stmt_from_expr(const srcml_nodes & nodes, int pos) {

  if((*nodes.at(pos)->get_parent())->get_name() == "init") {

    return bool((*nodes.at(pos)->get_parent())->get_parent()) && bool((*(*nodes.at(pos)->get_parent())->get_parent())->get_parent())
            && (*(*(*nodes.at(pos)->get_parent())->get_parent())->get_parent())->get_name() == "decl_stmt";

  }

  if((*nodes.at(pos)->get_parent())->get_name() == "argument") {

    return bool((*nodes.at(pos)->get_parent())->get_parent()) && bool((*(*nodes.at(pos)->get_parent())->get_parent())->get_parent())
            && bool((*(*(*nodes.at(pos)->get_parent())->get_parent())->get_parent())->get_parent())
            && (*(*(*(*nodes.at(pos)->get_parent())->get_parent())->get_parent())->get_parent())->get_name() == "decl_stmt";

  }

  return false;

}

bool srcdiff_nested::check_nest_name(const construct & set_original,
                     std::optional<std::shared_ptr<srcML::node>> parent_original,
                     const construct & set_modified,
                     std::optional<std::shared_ptr<srcML::node>> parent_modified) {

  // both are unused
  //int original_pos = set_original.start_position();
  //int modified_pos = set_modified.start_position();

  if(set_original.root_term_name() == "text") return false;
  if(set_modified.root_term_name() == "text") return false;

  bool is_call_name_original = parent_original && (*parent_original)->get_name() == "call";
  bool is_expr_name_original = parent_original && (*parent_original)->get_name() == "expr";
  // java does not have an expr in generics
  bool is_argument_name_original = parent_original && (*parent_original)->get_name() == "argument";
  bool is_type_name_original = parent_original && (*parent_original)->get_name() == "type";

  bool is_call_name_modified = parent_modified && (*parent_modified)->get_name() == "call";
  bool is_expr_name_modified = parent_modified && (*parent_modified)->get_name() == "expr";
  // java does not have an expr in generics
  bool is_argument_name_modified = parent_modified && (*parent_modified)->get_name() == "argument";
  bool is_type_name_modified = parent_modified && (*parent_modified)->get_name() == "type";

  if(is_type_name_original && (is_expr_name_modified || is_argument_name_modified))
    return true;
  if(is_type_name_modified && (is_expr_name_original || is_argument_name_original))
    return true;

  if(is_call_name_original && is_expr_name_modified) {

    int simple_name_pos = set_original.start_position();
    if(set_original.nodes().at(simple_name_pos)->get_name() == "name") {

      std::shared_ptr<construct> inner_set = std::make_shared<construct>(set_original.nodes(), simple_name_pos);
      srcdiff_text_measure measure(*inner_set, set_modified);
      int count = measure.number_match_beginning();
      return 2 * count >= measure.max_length();

    }

  }

  if(is_call_name_modified && is_expr_name_original) {

    int simple_name_pos = set_modified.start_position();
    if(set_modified.nodes().at(simple_name_pos)->get_name() == "name") {

      std::shared_ptr<construct> inner_set = std::make_shared<construct>(set_modified.nodes(), simple_name_pos);
      srcdiff_text_measure measure(set_original, *inner_set);
      int count = measure.number_match_beginning();
      return 2 * count >= measure.max_length();

    }

  }

  return false;

}

static bool check_nested_single_to_many(const construct::construct_list & construct_list_original, int start_original, int end_original
                 , const construct::construct_list & construct_list_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation) {

  int nest_count_original = 0;
  std::optional<int> pos_original;
  std::optional<int> similarity_original;
  std::optional<int> difference_original;
  int is_name_nest_original = 0;
  for(int i = start_original; i < end_original; ++i) {

    if(construct_list_original.at(i)->root_term()->get_move()) continue;

    for(int j = start_modified; j < end_modified; ++j) {

      if(construct_list_modified.at(j)->root_term()->get_move()) continue;

      if(srcdiff_nested::is_nestable(construct_list_modified.at(j), construct_list_original.at(i))) {

        std::shared_ptr<const construct> best_match = construct_list_original.at(i)->find_best_descendent(construct_list_modified.at(j));
        if(!best_match) continue;

        srcdiff_text_measure measure(*best_match, *construct_list_modified.at(j));
        measure.compute();

        if(!best_match->can_nest(*construct_list_modified.at(j))) {
          continue;
        }

        if(construct_list_modified.at(j)->root_term_name() == "name"
          && construct_list_modified.at(j)->root_term()->get_parent() && (*construct_list_modified.at(j)->root_term()->get_parent())->get_name() == "expr"
          && construct_list_original.at(i)->root_term()->get_parent() && (*construct_list_original.at(i)->root_term()->get_parent())->get_name() == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1)) {
          ++is_name_nest_original;
        }

        if(construct_list_modified.at(j)->root_term_name() == "name") {

            if(!construct_list_modified.at(j)->root_term()->get_parent() || !best_match->root_term()->get_parent()) {
              continue;
            }

            std::optional<std::shared_ptr<srcML::node>> parent_original = best_match->root_term()->get_parent();
            while((*parent_original)->get_name() == "name") {
              parent_original = (*parent_original)->get_parent();
            }

            std::optional<std::shared_ptr<srcML::node>> parent_modified = construct_list_modified.at(j)->root_term()->get_parent();
            while((*parent_modified)->get_name() == "name") {
              parent_modified = (*parent_modified)->get_parent();
            }

            if((*parent_original)->get_name() != (*parent_modified)->get_name()
              && !srcdiff_nested::check_nest_name(*best_match, parent_original,
                                  *construct_list_modified.at(j), parent_modified)) {
              continue;
            }

        }

        if(!bool(pos_original) || measure.similarity() > similarity_original) {

          ++nest_count_original;
          pos_original = i;
          similarity_original = measure.similarity();
          difference_original = measure.difference();

        }

      }

    }

  }

  if(nest_count_original > 1 && is_name_nest_original > 1) {
    nest_count_original = 0;
    pos_original = std::optional<int>();
    similarity_original = std::optional<int>();
    difference_original = std::optional<int>();
  }

  int nest_count_modified = 0;
  std::optional<int> pos_modified;
  std::optional<int> similarity_modified;
  std::optional<int> difference_modified;
  int is_name_nest_modified = 0;
  for(int i = start_modified; i < end_modified; ++i) {

    if(construct_list_modified.at(i)->root_term()->get_move()) continue;

    for(int j = start_original; j < end_original; ++j) {

      if(construct_list_original.at(j)->root_term()->get_move()) continue;

      if(srcdiff_nested::is_nestable(construct_list_original.at(j), construct_list_modified.at(i))) {

        std::shared_ptr<const construct> best_match = construct_list_modified.at(i)->find_best_descendent(construct_list_original.at(j));
        if(!best_match) continue;

        srcdiff_text_measure measure(*construct_list_original.at(j), *best_match);
        measure.compute();

        if(!construct_list_original.at(j)->can_nest(*best_match)) {
          continue;
        }

        if(construct_list_original.at(j)->root_term_name() == "name"
          && construct_list_original.at(j)->root_term()->get_parent() && (*construct_list_original.at(j)->root_term()->get_parent())->get_name() == "expr"
          && construct_list_modified.at(i)->root_term()->get_parent() && (*construct_list_modified.at(i)->root_term()->get_parent())->get_name() == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1)) {
            ++is_name_nest_modified;
          }

        if(construct_list_original.at(j)->root_term_name() == "name") {

            if(!construct_list_original.at(j)->root_term()->get_parent() || !best_match->root_term()->get_parent()) {
              continue;
            }

            std::optional<std::shared_ptr<srcML::node>> parent_original = construct_list_original.at(j)->root_term()->get_parent();
            while(parent_original && (*parent_original)->get_name() == "name") {
              parent_original = (*parent_original)->get_parent();
            }

            std::optional<std::shared_ptr<srcML::node>> parent_modified = best_match->root_term()->get_parent();
            while(parent_modified && (*parent_modified)->get_name() == "name") {
              parent_modified = (*parent_modified)->get_parent();
            }

            if((*parent_original)->get_name() != (*parent_modified)->get_name()
              && !srcdiff_nested::check_nest_name(*construct_list_original.at(j), parent_original,
                                  *best_match, parent_modified)) {
              continue;
            }

        }

        if(!bool(pos_modified) || measure.similarity() > similarity_modified) {

          ++nest_count_modified;
          pos_modified = i;
          similarity_modified = measure.similarity();
          difference_modified = measure.difference();

        }

      }

    }

  }

  if(nest_count_modified > 1 && is_name_nest_modified > 1) {
    nest_count_modified = 0;
    pos_modified = std::optional<int>();
    similarity_modified = std::optional<int>();
    difference_modified = std::optional<int>();
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

bool srcdiff_nested::check_nestable_predicate(construct::construct_list_view construct_list_outer,
                                              construct::construct_list_view construct_list_inner) {

  if(construct_list_inner[0]->root_term()->get_move()) return true;

  if(!is_nestable(construct_list_inner[0], construct_list_outer[0]))
    return true;

  std::shared_ptr<const construct> best_match = construct_list_outer[0]->find_best_descendent(construct_list_inner[0]);
  if(!best_match) return true;

  srcdiff_text_measure measure(*best_match, *construct_list_inner[0]);
  measure.compute();

  if(!best_match->can_nest(*construct_list_inner[0]))
    return true;

  if(is_better_nest(construct_list_inner[0], construct_list_outer[0], measure))
    return true;

  if(construct_list_outer.size() > 1 && is_better_nest(construct_list_outer[1], construct_list_inner[0], measure))
    return true;

  if(construct_list_inner.size() > 1 && is_better_nest(construct_list_inner[1], construct_list_outer[0], measure))
    return true;

  if(construct_list_inner[0]->root_term_name() == "name"
    && construct_list_inner[0]->root_term()->get_parent() && (*construct_list_inner[0]->root_term()->get_parent())->get_name() == "expr"
    && construct_list_outer[0]->root_term()->get_parent() && (*construct_list_outer[0]->root_term()->get_parent())->get_name() == "expr"
    && (construct_list_outer.size() > 1 || construct_list_inner.size() > 1))
    return true;

  if(construct_list_inner[0]->root_term_name() == "name") {

      if(!construct_list_inner[0]->root_term()->get_parent() || !best_match->root_term()->get_parent())
        return true;

      std::optional<std::shared_ptr<srcML::node>> parent_outer = best_match->root_term()->get_parent();
      while((*parent_outer)->get_name() == "name") {
        parent_outer = (*parent_outer)->get_parent();
      }

      std::optional<std::shared_ptr<srcML::node>> parent_inner = construct_list_inner[0]->root_term()->get_parent();
      while((*parent_inner)->get_name() == "name") {
        parent_inner = (*parent_inner)->get_parent();
      }

      if((*parent_outer)->get_name() != (*parent_inner)->get_name()
        && !srcdiff_nested::check_nest_name(*best_match, parent_outer,
           *construct_list_inner[0], parent_inner))
        return true;

  }

  return false;


}

/**
 *
 * @todo need to make this more robust using dynamic programming.
 * Probably need one for both ways. Collect all statements of type one, and all the possible matches (statements of those types in order)
 * and run dynamic programming algorithm.  Run both ways and use number matched and similarity to choose best.
 *
 */

std::tuple<std::vector<int>, int, int> srcdiff_nested::check_nestable(construct::construct_list_view parent_list, construct::construct_list_view child_list) {

  for(size_t i = 0; i < parent_list.size(); ++i) {

    if(parent_list[i]->root_term()->get_move()) continue;

    for(size_t j = 0; j < child_list.size(); ++j) {

      if(check_nestable_predicate(construct::construct_list_view(&parent_list[i], parent_list.size() - i),
                                  construct::construct_list_view(&child_list[j], child_list.size() - j))) {
        continue;
      }

      std::tuple<std::vector<int>, int, int> nestings = std::make_tuple(std::vector<int>(), i, i + 1);
      std::get<0>(nestings).push_back(j);
      // comparison of signed vs unsigned
      for(int k = j + 1; k < child_list.size(); ++k) {

        if(check_nestable_predicate(construct::construct_list_view(&parent_list[i], parent_list.size() - i),
                                    construct::construct_list_view(&child_list[k], child_list.size() - k))) {
          continue;
        }

        std::get<0>(nestings).push_back(k);

      }

      return nestings;
    }
  }

  return std::make_tuple(std::vector<int>(), 0, 0);

}

void srcdiff_nested::check_nestable(const construct::construct_list & construct_list_original, int start_original, int end_original
                 , const construct::construct_list & construct_list_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation) {

  start_nest_original = start_original;  
  end_nest_original = start_original;
  start_nest_modified = start_modified;  
  end_nest_modified = start_modified;

  operation = SES_COMMON;

  if((end_original - start_original) == 1 || (end_modified - start_modified) == 1) {

    if(check_nested_single_to_many(construct_list_original, start_original, end_original,
                                construct_list_modified, start_modified, end_modified,
                                start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation))
      return;

  }

  construct::construct_list_view original_view = construct::construct_list_view(&construct_list_original.at(start_original), end_original - start_original);
  construct::construct_list_view modified_view = construct::construct_list_view(&construct_list_modified.at(start_modified), end_modified - start_modified);

  std::tuple<std::vector<int>, int, int> original_check = check_nestable(original_view, modified_view);

  const std::vector<int> & valid_nests_original = std::get<0>(original_check);
  if(valid_nests_original.size()) {
    start_nest_original = start_original + std::get<1>(original_check);  
    end_nest_original  = start_original + std::get<2>(original_check); 
  }

  std::tuple<std::vector<int>, int, int> modified_check = check_nestable(modified_view, original_view);

  const std::vector<int> & valid_nests_modified = std::get<0>(modified_check);
  if(valid_nests_modified.size()) {
    start_nest_modified = start_modified + std::get<1>(modified_check);  
    end_nest_modified  = start_modified + std::get<2>(modified_check); 
  }
  /** @todo may need a more exact check to pick most optimal or another check 

    For now if only valid, less than or equal and do not cross, or cross and larger.

  */
  if(!valid_nests_original.empty() && (valid_nests_modified.empty()
   || (start_nest_original < valid_nests_modified.front() && valid_nests_original.back() < start_nest_modified)
   || (!(start_nest_original > valid_nests_modified.back() && valid_nests_original.front() > start_nest_modified)
      && (valid_nests_original.back() - valid_nests_original.front()) >= (valid_nests_modified.back() - valid_nests_modified.front())))) {

      start_nest_modified = start_modified + valid_nests_original.front();
      end_nest_modified = start_modified + valid_nests_original.back() + 1;
      operation = SES_DELETE;

  } else if(!valid_nests_modified.empty()) {

      start_nest_original = start_original + valid_nests_modified.front();
      end_nest_original = start_original + valid_nests_modified.back() + 1;
      operation = SES_INSERT;

  }

}

void srcdiff_nested::output_inner(srcdiff_whitespace & whitespace,
                  const construct::construct_list & construct_list_outer,
                  int start_outer,
                  int end_outer,
                  const construct::construct_list & construct_list_inner,
                  int start_inner,
                  int end_inner,
                  int operation) {

  size_t start_pos = construct_list_outer.at(start_outer)->get_terms().at(1);
  size_t end_pos = construct_list_outer.at(end_outer - 1)->end_position();

  const std::string & structure_outer = construct_list_outer.at(start_outer)->root_term_name();
  if(structure_outer == "block_content") {
    // do not skip whitespace
    start_pos = construct_list_outer.at(start_outer)->start_position() + 1;

  } else if(structure_outer == "if" && !bool(construct_list_outer.at(start_outer)->root_term()->get_attribute("type"))) {

    advance_to_child(construct_list_outer.back()->nodes(), start_pos, srcML::node_type::START, "block");

  } else if(structure_outer == "while") {

    advance_to_child(construct_list_outer.back()->nodes(), start_pos, srcML::node_type::END, "condition");
    ++start_pos;

  } else if(structure_outer == "for") {

    advance_to_child(construct_list_outer.back()->nodes(), start_pos, srcML::node_type::END, "control");
    ++start_pos;

  } else if(is_class_type(structure_outer)) {

    advance_to_child(construct_list_outer.back()->nodes(), start_pos, srcML::node_type::START, "block");
    ++start_pos;

    end_pos = start_pos - 1;
    advance_to_child(construct_list_outer.back()->nodes(), end_pos, srcML::node_type::END, "block");

  }

  construct::construct_list set = construct::get_descendent_constructs(construct_list_outer.back()->nodes(),
                            start_pos, end_pos);

  construct::construct_list nest_set;

  for(int i = start_inner; i < end_inner; ++i) {
      nest_set.push_back(construct_list_inner.at(i));
  }

  if(operation == SES_DELETE) {
    srcdiff_change::output_change(out, start_pos, out->last_output_modified());
  }
  else {
    srcdiff_change::output_change(out, out->last_output_original(), start_pos);
  }

  if(structure_outer == "block_content") {
    whitespace.output_prefix();
  } else {
    whitespace.output_nested(operation);
  }

  if(operation == SES_DELETE) {

    srcdiff_diff diff(out, set, nest_set);
    diff.output();

  } else {

    srcdiff_diff diff(out, nest_set, set);
    diff.output();

  }

  if(structure_outer == "block_content") {
    whitespace.output_prefix();
  } else {
    whitespace.output_nested(operation);
  }

  if(operation == SES_DELETE) {
    srcdiff_change::output_change(out, construct_list_outer.at(end_outer - 1)->end_position() + 1, out->last_output_modified());
  }
  else {
    srcdiff_change::output_change(out, out->last_output_original(), construct_list_outer.at(end_outer - 1)->end_position() + 1);
  }

}

void srcdiff_nested::output() {

  srcdiff_whitespace whitespace(*out);

  whitespace.output_prefix();

  if(operation == SES_DELETE)
    output_inner(whitespace,
                 construct_list_original,
                 start_original,
                 end_original,
                 construct_list_modified,
                 start_modified,
                 end_modified,
                 operation);

  else
    output_inner(whitespace,
                 construct_list_modified,
                 start_modified,
                 end_modified,
                 construct_list_original,
                 start_original,
                 end_original,
                 operation);

}
