#include <srcdiff_nested.hpp>

#include <srcdiff_constants.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_text_measure.hpp>
#include <srcdiff_compare.hpp>
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

int is_block_type(const construct & structure) {

  if((xmlReaderTypes)structure.root_term()->type != XML_READER_TYPE_ELEMENT)
    return -1;

  if(structure.root_term()->ns.href != SRCML_SRC_NAMESPACE_HREF)
    return -1;

  for(int i = 0; nesting[i].type; ++i)
    if(structure.root_term_name() == nesting[i].type)
      return i;

  return -1;
}

bool has_internal_structure(const construct & structure, const boost::optional<std::string> & type) {

  if(!type) return false;

  for(unsigned int i = 1; i < structure.size(); ++i) {
    if((xmlReaderTypes)structure.term(i)->type == XML_READER_TYPE_ELEMENT
              && structure.term(i)->name == type)
      return true;
  }

  return false;
}

bool is_nest_type(const construct & structure,
                  const construct & structure_other,
                  int type_index) {

  if((xmlReaderTypes)structure.root_term()->type != XML_READER_TYPE_ELEMENT)
    return false;

    if(structure.root_term()->ns.href != SRCML_SRC_NAMESPACE_HREF)
    return true;

  for(int i = 0; nesting[type_index].possible_nest_items[i]; ++i) {
    if(structure.root_term_name() == nesting[type_index].possible_nest_items[i]
       && has_internal_structure(structure_other, structure.root_term_name()))
      return true;
  }

  return false;
}

bool srcdiff_nested::is_match(int & node_pos, const srcml_nodes & nodes, const void * context) {

  const std::shared_ptr<srcml_node> & node = nodes[node_pos];
  const std::shared_ptr<srcml_node> & context_node = *(const std::shared_ptr<srcml_node> *)context;

  return (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT && *node == *context_node;

}

/**
 * best_match
 * @param sets - node set to search for match
 * @param match - set searching for
 *
 * Search sets to find best match for match
 */
int srcdiff_nested::best_match(const construct::construct_list & sets, const construct & match) {

  int match_pos = sets.size();
  int match_similarity = -1;

  for(unsigned int i = 0; i < sets.size(); ++i) {

    if(sets.at(i).size() > match.size() && (sets.at(i).size()) > (4 * match.size())) {
      continue;
    }

    if(match.size() > sets.at(i).size() && (match.size()) > (4 * sets.at(i).size())) {
      continue;
    }

    srcdiff_text_measure measure(sets.at(i), match);
    measure.compute();
    if(measure.similarity() > match_similarity) {

      match_pos = i;
      match_similarity = measure.similarity();

    }

  }

  return match_pos;

}

bool is_nestable_internal(const construct & structure_one,
                          const construct & structure_two) {

  int block = is_block_type(structure_two);

  if(block == -1)
    return false;

  /** Only can nest a block into another block if it's parent is a block */
  bool is_block = structure_one.root_term_name() == "block" && structure_two.root_term_name() == "block";
  bool parent_is_block = structure_one.root_term()->parent && (*structure_one.root_term()->parent)->name == "block";
  if(is_block && !parent_is_block) return false;

  if(is_nest_type(structure_one, structure_two, block)) {

    return true;

  }

  return false;
}

bool srcdiff_nested::is_same_nestable(const construct & structure_one,
                                      const construct & structure_two) {

  if(!is_nestable_internal(structure_one, structure_two))
    return false;

  construct::construct_list set = construct::get_descendent_constructs(structure_two.nodes(), structure_two.get_terms().at(1), structure_two.end_position(), srcdiff_nested::is_match, &structure_one.root_term());

  unsigned int match = best_match(set, structure_one);

  if(match >= set.size())
    return false;

  srcdiff_text_measure match_measure(structure_one, set.at(match));
  match_measure.compute();

  srcdiff_text_measure measure(structure_one, structure_two);
  measure.compute();

  double min_size = measure.min_length();
  double match_min_size = std::min(measure.original_length(), match_measure.modified_length());

  return (match_measure.similarity() >= measure.similarity() && match_measure.difference() <= measure.difference()) 
  || (match_min_size > 50 && min_size > 50 && (match_min_size / match_measure.similarity()) < (0.9 * (min_size / measure.similarity()))
    && !srcdiff_nested::reject_match_nested(match_measure, set.at(match), structure_one));

}

bool srcdiff_nested::is_nestable(const construct & structure_one,
                                 const construct & structure_two) {

  if(*structure_one.root_term() == *structure_two.root_term())
    return is_same_nestable(structure_one, structure_two);
  else
    return is_nestable_internal(structure_one, structure_two);

}

bool is_better_nest_no_recursion(const construct & node_set_outer,
                                 const construct & node_set_inner,
                                 const srcdiff_measure & measure) {

    if(srcdiff_nested::is_nestable(node_set_inner, node_set_outer)) {

      construct::construct_list set = construct::get_descendent_constructs(node_set_outer.nodes(), node_set_outer.get_terms().at(1), node_set_outer.end_position(), srcdiff_nested::is_match,
                                &node_set_inner.root_term());

      int match = srcdiff_nested::best_match(set, node_set_inner);

      if(match < set.size()) {

        srcdiff_text_measure match_measure(set.at(match), node_set_inner);
        match_measure.compute();

        double min_size = measure.min_length();
        double nest_min_size = match_measure.min_length();

        if((match_measure.similarity() >= measure.similarity() && match_measure.difference() <= measure.difference())
         || ((nest_min_size / match_measure.similarity()) < (min_size / measure.similarity())
            && !srcdiff_nested::reject_match_nested(match_measure, node_set_inner, node_set_outer)))
          return true;
    
      }

    }

    return false;

}

bool has_compound_inner(const construct & node_set_outer) {

  if(node_set_outer.root_term()->is_simple) return false;

  for(unsigned int i = 1; i < node_set_outer.size(); ++i) {
    if((xmlReaderTypes)node_set_outer.term(i)->type == XML_READER_TYPE_ELEMENT
      && node_set_outer.term(i)->name == "name" && !node_set_outer.term(i)->is_simple)
      return true;
  }

  return false;

}

bool is_better_nest(const construct & node_set_outer,
                    const construct & node_set_inner,
                    const srcdiff_measure & measure) {

  // do not nest compound name in simple or anything into something that is not compound
  if(node_set_outer.root_term_name() == "name" && node_set_inner.root_term_name() == "name"
    &&    (node_set_outer.root_term()->is_simple
      || (!node_set_inner.root_term()->is_simple && !has_compound_inner(node_set_outer)))) return false;

  // parents and children same do not nest.
  if(srcdiff_nested::is_nestable(node_set_inner, node_set_outer)) {

    construct::construct_list set = construct::get_descendent_constructs(node_set_outer.nodes(), node_set_outer.get_terms().at(1), node_set_outer.end_position(), srcdiff_nested::is_match
                                                           , &node_set_inner.root_term());

    int match = srcdiff_nested::best_match(set, node_set_inner);

    if(match < set.size()) {

      srcdiff_text_measure match_measure(set.at(match), node_set_inner);
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
          && !srcdiff_nested::reject_match_nested(match_measure, node_set_inner, set.at(match)))
       )
        // check if other way is better
        return !is_better_nest_no_recursion(node_set_inner, node_set_outer, match_measure);

    }

  }

  return false;

}

bool srcdiff_nested::is_better_nested(const construct::construct_list & construct_list_original, int start_pos_original,
                                      const construct::construct_list & construct_list_modified, int start_pos_modified,
                                      const srcdiff_measure & measure) {

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

bool check_nest_name(const construct & set_original,
                     boost::optional<std::shared_ptr<srcml_node>> parent_original,
                     const construct & set_modified,
                     boost::optional<std::shared_ptr<srcml_node>> parent_modified) {

  int original_pos = set_original.start_position();
  int modified_pos = set_modified.start_position();

  if(set_original.root_term_name() == "text") return false;
  if(set_modified.root_term_name() == "text") return false;

  bool is_call_name_original = parent_original && (*parent_original)->name == "call";
  bool is_expr_name_original = parent_original && (*parent_original)->name == "expr";
  // java does not have an expr in generics
  bool is_argument_name_original = parent_original && (*parent_original)->name == "argument";
  bool is_type_name_original = parent_original && (*parent_original)->name == "type";

  bool is_call_name_modified = parent_modified && (*parent_modified)->name == "call";
  bool is_expr_name_modified = parent_modified && (*parent_modified)->name == "expr";
  // java does not have an expr in generics
  bool is_argument_name_modified = parent_modified && (*parent_modified)->name == "argument";
  bool is_type_name_modified = parent_modified && (*parent_modified)->name == "type";

  if(is_type_name_original && (is_expr_name_modified || is_argument_name_modified))
    return true;
  if(is_type_name_modified && (is_expr_name_original || is_argument_name_original))
    return true;

  if(is_call_name_original && is_expr_name_modified) {

    int simple_name_pos = set_original.start_position();
    if(set_original.nodes().at(simple_name_pos)->name == "name") {

      construct inner_set(set_original.nodes(), simple_name_pos);
      srcdiff_text_measure measure(inner_set, set_modified);
      int count = measure.number_match_beginning();
      return 2 * count >= measure.max_length();

    }

  }

  if(is_call_name_modified && is_expr_name_original) {

    int simple_name_pos = set_modified.start_position();
    if(set_modified.nodes().at(simple_name_pos)->name == "name") {

      construct inner_set(set_modified.nodes(), simple_name_pos);
      srcdiff_text_measure measure(set_original, inner_set);
      int count = measure.number_match_beginning();
      return 2 * count >= measure.max_length();

    }

  }

  return false;

}

bool srcdiff_nested::reject_match_nested(const srcdiff_measure & measure,
                                         const construct & set_original,
                                         const construct & set_modified) {

  int original_pos = set_original.start_position();
  int modified_pos = set_modified.start_position();

  const std::string & original_tag = set_original.root_term_name();
  const std::string & modified_tag = set_modified.root_term_name();

  const std::string & original_uri = set_original.root_term()->ns.href;
  const std::string & modified_uri = set_modified.root_term()->ns.href;

  if(original_tag != modified_tag && !srcdiff_match::is_interchangeable_match(set_original, set_modified)) return true;

  // if interchanging decl_stmt always nest expr into init or argument
  if(original_tag == "expr" && (is_decl_stmt_from_expr(set_original.nodes(), original_pos) || is_decl_stmt_from_expr(set_modified.nodes(), modified_pos))) return false;

  if(original_tag == "name"
    && set_original.root_term()->is_simple != set_modified.root_term()->is_simple
    && !check_nest_name(set_original, set_original.root_term()->parent,
                       set_modified, set_modified.root_term()->parent))
    return true;

  if(original_tag == "then" || original_tag == "block" || original_tag == "block_content"
    || original_tag == "comment"
    || original_tag == "literal" || original_tag == "operator" || original_tag == "modifier"
    || original_tag == "expr" || original_tag == "expr_stmt" || original_tag == "name"
    || original_tag == "number" || original_tag == "file") {

    bool is_reject = srcdiff_match::reject_similarity(measure,
                                                      set_original,
                                                      set_modified);
    return is_reject;

  } else {

    return srcdiff_match::reject_match(measure, set_original, set_modified);

  }

}

static bool check_nested_single_to_many(const construct::construct_list & construct_list_original, int start_original, int end_original
                 , const construct::construct_list & construct_list_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation) {

  int nest_count_original = 0;
  boost::optional<int> pos_original;
  boost::optional<int> similarity_original;
  boost::optional<int> difference_original;
  int is_name_nest_original = 0;
  for(int i = start_original; i < end_original; ++i) {

    if(construct_list_original.at(i).root_term()->move) continue;

    for(int j = start_modified; j < end_modified; ++j) {

      if(construct_list_modified.at(j).root_term()->move) continue;

      if(srcdiff_nested::is_nestable(construct_list_modified.at(j), construct_list_original.at(i))) {

        construct::construct_list set = construct::get_descendent_constructs(construct_list_original.back().nodes(), construct_list_original.at(i).get_terms().at(1), construct_list_original.at(i).end_position(), srcdiff_nested::is_match
                                                             , &construct_list_modified.at(j).root_term());

        int match = srcdiff_nested::best_match(set, construct_list_modified.at(j));

        if(match >= set.size()) continue;

        srcdiff_text_measure measure(set.at(match), construct_list_modified.at(j));
        measure.compute();

        if(srcdiff_nested::reject_match_nested(measure,
                                               set.at(match),
                                               construct_list_modified.at(j))) {
          continue;
        }

        if(construct_list_modified.at(j).root_term_name() == "name"
          && construct_list_modified.at(j).root_term()->parent && (*construct_list_modified.at(j).root_term()->parent)->name == "expr"
          && construct_list_original.at(i).root_term()->parent && (*construct_list_original.at(i).root_term()->parent)->name == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1)) {
          ++is_name_nest_original;
        }

        if(construct_list_modified.at(j).root_term_name() == "name") {

            if(!construct_list_modified.at(j).root_term()->parent || !set.at(match).root_term()->parent) {
              continue;
            }

            boost::optional<std::shared_ptr<srcml_node>> parent_original = set.at(match).root_term()->parent;
            while((*parent_original)->name == "name") {
              parent_original = (*parent_original)->parent;
            }

            boost::optional<std::shared_ptr<srcml_node>> parent_modified = construct_list_modified.at(j).root_term()->parent;
            while((*parent_modified)->name == "name") {
              parent_modified = (*parent_modified)->parent;
            }

            if((*parent_original)->name != (*parent_modified)->name
              && !check_nest_name(set.at(match), parent_original,
                                  construct_list_modified.at(j), parent_modified)) {
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
    pos_original = boost::optional<int>();
    similarity_original = boost::optional<int>();
    difference_original = boost::optional<int>();
  }

  int nest_count_modified = 0;
  boost::optional<int> pos_modified;
  boost::optional<int> similarity_modified;
  boost::optional<int> difference_modified;
  int is_name_nest_modified = 0;
  for(int i = start_modified; i < end_modified; ++i) {

    if(construct_list_modified.at(i).root_term()->move) continue;

    for(int j = start_original; j < end_original; ++j) {

      if(construct_list_original.at(j).root_term()->move) continue;

      if(srcdiff_nested::is_nestable(construct_list_original.at(j), construct_list_modified.at(i))) {

        construct::construct_list set = construct::get_descendent_constructs(construct_list_modified.back().nodes(), construct_list_modified.at(i).get_terms().at(1), construct_list_modified.at(i).end_position(), srcdiff_nested::is_match
                                                             , &construct_list_original.at(j).root_term());

        int match = srcdiff_nested::best_match(set, construct_list_original.at(j));

        if(match >= set.size()) continue;

        srcdiff_text_measure measure(construct_list_original.at(j), set.at(match));
        measure.compute();

        if(srcdiff_nested::reject_match_nested(measure,
                                               construct_list_original.at(j),
                                               set.at(match))) {
          continue;
        }

        if(construct_list_original.at(j).root_term_name() == "name"
          && construct_list_original.at(j).root_term()->parent && (*construct_list_original.at(j).root_term()->parent)->name == "expr"
          && construct_list_modified.at(i).root_term()->parent && (*construct_list_modified.at(i).root_term()->parent)->name == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1)) {
            ++is_name_nest_modified;
          }

        if(construct_list_original.at(j).root_term_name() == "name") {

            if(!construct_list_original.at(j).root_term()->parent || !set.at(match).root_term()->parent) {
              continue;
            }

            boost::optional<std::shared_ptr<srcml_node>> parent_original = construct_list_original.at(j).root_term()->parent;
            while(parent_original && (*parent_original)->name == "name") {
              parent_original = (*parent_original)->parent;
            }

            boost::optional<std::shared_ptr<srcml_node>> parent_modified = set.at(match).root_term()->parent;
            while(parent_modified && (*parent_modified)->name == "name") {
              parent_modified = (*parent_modified)->parent;
            }

            if((*parent_original)->name != (*parent_modified)->name
              && !check_nest_name(construct_list_original.at(j), parent_original,
                                  set.at(match), parent_modified)) {
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
    pos_modified = boost::optional<int>();
    similarity_modified = boost::optional<int>();
    difference_modified = boost::optional<int>();
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

bool srcdiff_nested::check_nestable_predicate(const construct::construct_list & construct_list_outer,
                                              int pos_outer, int start_outer, int end_outer,
                                              const construct::construct_list & construct_list_inner,
                                              int pos_inner, int start_inner, int end_inner) {

  if(construct_list_inner.at(pos_inner).root_term()->move) return true;

  if(!is_nestable(construct_list_inner.at(pos_inner), construct_list_outer.at(pos_outer)))
    return true;

  construct::construct_list set = construct::get_descendent_constructs(construct_list_outer.back().nodes(), construct_list_outer.at(pos_outer).get_terms().at(1),
                            construct_list_outer.at(pos_outer).end_position(), is_match,
                            &construct_list_inner.at(pos_inner).root_term());

  int match_pos = best_match(set, construct_list_inner.at(pos_inner));

  if(match_pos >= set.size()) return true;

  const construct & match = set.at(match_pos);

  srcdiff_text_measure measure(match, construct_list_inner.at(pos_inner));
  measure.compute();

  if(reject_match_nested(measure, match, construct_list_inner.at(pos_inner)))
    return true;

  if(is_better_nest(construct_list_inner.at(pos_inner), construct_list_outer.at(pos_outer), measure))
    return true;

  if(pos_outer + 1 < end_outer && is_better_nest(construct_list_outer.at(pos_outer + 1), construct_list_inner.at(pos_inner), measure))
    return true;

  if(pos_inner + 1 < end_inner && is_better_nest(construct_list_inner.at(pos_inner + 1), construct_list_outer.at(pos_outer), measure))
    return true;

  if(construct_list_inner.at(pos_inner).root_term_name() == "name"
    && construct_list_inner.at(pos_inner).root_term()->parent && (*construct_list_inner.at(pos_inner).root_term()->parent)->name == "expr"
    && construct_list_outer.at(pos_outer).root_term()->parent && (*construct_list_outer.at(pos_outer).root_term()->parent)->name == "expr"
    && ((end_outer - start_outer) > 1 || (end_inner - start_inner) > 1))
    return true;

  if(construct_list_inner.at(pos_inner).root_term_name() == "name") {

      if(!construct_list_inner.at(pos_inner).root_term()->parent || !match.root_term()->parent)
        return true;

      boost::optional<std::shared_ptr<srcml_node>> parent_outer = match.root_term()->parent;
      while((*parent_outer)->name == "name") {
        parent_outer = (*parent_outer)->parent;
      }

      boost::optional<std::shared_ptr<srcml_node>> parent_inner = construct_list_inner.at(pos_inner).root_term()->parent;
      while((*parent_inner)->name == "name") {
        parent_inner = (*parent_inner)->parent;
      }

      if((*parent_outer)->name != (*parent_inner)->name
        && !check_nest_name(match, parent_outer,
                            construct_list_inner.at(pos_inner), parent_inner))
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

  std::vector<int> valid_nests_original;
  std::vector<int> valid_nests_modified;

  for(int i = start_original; i < end_original; ++i) {

    if(construct_list_original.at(i).root_term()->move) continue;

    for(int j = start_modified; j < end_modified; ++j) {

      if(check_nestable_predicate(construct_list_original, i, start_original, end_original,
                                  construct_list_modified, j, start_modified, end_modified))
        continue;

      valid_nests_original.push_back(j);

      start_nest_original = i;
      end_nest_original = i + 1;

      for(int k = j + 1; k < end_modified; ++k) {

        if(check_nestable_predicate(construct_list_original, i, start_original, end_original,
                                    construct_list_modified, k, start_modified, end_modified)) {
          continue;
        }

        valid_nests_original.push_back(k);

      }

      goto end_nest_check_original;

    }

  }

  end_nest_check_original:

  for(int i = start_modified; i < end_modified; ++i) {

    if(construct_list_modified.at(i).root_term()->move) continue;

    for(int j = start_original; j < end_original; ++j) {

      if(check_nestable_predicate(construct_list_modified, i, start_modified, end_modified,
                                  construct_list_original, j, start_original, end_original)) {
        continue;
      }

      valid_nests_modified.push_back(j);

      start_nest_modified = i;
      end_nest_modified = i + 1;

      for(int k = j + 1; k < end_original; ++k) {
      
        if(check_nestable_predicate(construct_list_modified, i, start_modified, end_modified,
                                    construct_list_original, k, start_original, end_original)) {
          continue;
        }

        valid_nests_modified.push_back(k);

      }

      goto end_nest_check_modified;

    }

  }

  end_nest_check_modified:

  /** @todo may need a more exact check to pick most optimal or another check 

    For now if only valid, less than or equal and do not cross, or cross and larger.

  */
  if(!valid_nests_original.empty() && (valid_nests_modified.empty()
   || (start_nest_original < valid_nests_modified.front() && valid_nests_original.back() < start_nest_modified)
   || (!(start_nest_original > valid_nests_modified.back() && valid_nests_original.front() > start_nest_modified)
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

void srcdiff_nested::output_inner(srcdiff_whitespace & whitespace,
                  const construct::construct_list & construct_list_outer,
                  int start_outer,
                  int end_outer,
                  const construct::construct_list & construct_list_inner,
                  int start_inner,
                  int end_inner,
                  int operation) {

  size_t start_pos = construct_list_outer.at(start_outer).get_terms().at(1);
  size_t end_pos = construct_list_outer.at(end_outer - 1).end_position();

  const std::string & structure_outer = construct_list_outer.at(start_outer).root_term_name();
  if(structure_outer == "block_content") {
    // do not skip whitespace
    start_pos = construct_list_outer.at(start_outer).start_position() + 1;

  } else if(structure_outer == "if" && !bool(find_attribute(construct_list_outer.at(start_outer).root_term(), "type"))) {

    advance_to_child(construct_list_outer.back().nodes(), start_pos, XML_READER_TYPE_ELEMENT, "block");

  } else if(structure_outer == "while") {

    advance_to_child(construct_list_outer.back().nodes(), start_pos, XML_READER_TYPE_END_ELEMENT, "condition");
    ++start_pos;

  } else if(structure_outer == "for") {

    advance_to_child(construct_list_outer.back().nodes(), start_pos, XML_READER_TYPE_END_ELEMENT, "control");
    ++start_pos;

  } else if(is_class_type(structure_outer)) {

    advance_to_child(construct_list_outer.back().nodes(), start_pos, XML_READER_TYPE_ELEMENT, "block");
    ++start_pos;

    end_pos = start_pos - 1;
    advance_to_child(construct_list_outer.back().nodes(), end_pos, XML_READER_TYPE_END_ELEMENT, "block");

  }

  construct::construct_list set = construct::get_descendent_constructs(construct_list_outer.back().nodes(),
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
    srcdiff_change::output_change(out, construct_list_outer.at(end_outer - 1).end_position() + 1, out->last_output_modified());
  }
  else {
    srcdiff_change::output_change(out, out->last_output_original(), construct_list_outer.at(end_outer - 1).end_position() + 1);
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
