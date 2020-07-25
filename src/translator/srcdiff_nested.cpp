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

int is_block_type(const node_set & structure) {

  if((xmlReaderTypes)structure.nodes().at(structure.at(0))->type != XML_READER_TYPE_ELEMENT)
    return -1;

  if(structure.nodes().at(structure.at(0))->ns.href != SRCML_SRC_NAMESPACE_HREF)
    return -1;

  for(int i = 0; nesting[i].type; ++i)
    if(structure.nodes().at(structure.at(0))->name == nesting[i].type)
      return i;

  return -1;
}

bool has_internal_structure(const node_set & structure, const boost::optional<std::string> & type) {

  if(!type) return false;

  for(unsigned int i = 1; i < structure.size(); ++i)
    if((xmlReaderTypes)structure.nodes().at(structure.at(i))->type == XML_READER_TYPE_ELEMENT
              && structure.nodes().at(structure.at(i))->name == type)
      return true;

  return false;
}

bool is_nest_type(const node_set & structure,
                  const node_set & structure_other,
                  int type_index) {

  if((xmlReaderTypes)structure.nodes().at(structure.at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

    if(structure.nodes().at(structure.at(0))->ns.href != SRCML_SRC_NAMESPACE_HREF)
    return true;

  for(int i = 0; nesting[type_index].possible_nest_items[i]; ++i)
    if(structure.nodes().at(structure.at(0))->name == nesting[type_index].possible_nest_items[i]
       && has_internal_structure(structure_other, structure.nodes().at(structure.at(0))->name))
      return true;

  return false;
}

bool srcdiff_nested::is_match(int & node_pos, const srcml_nodes & nodes, const void * context) {

  const std::shared_ptr<srcml_node> & node = nodes[node_pos];

  const std::shared_ptr<srcml_node> & context_node = *(const std::shared_ptr<srcml_node> *)context;

  return (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT && srcdiff_compare::node_compare(node, context_node) == 0;

}

/**
 * best_match
 * @param sets - node set to search for match
 * @param match - set searching for
 *
 * Search sets to find best match for match
 */
int srcdiff_nested::best_match(const node_sets & sets, const node_set & match) {

  int match_pos = sets.size();
  int match_similarity = -1;

  for(unsigned int i = 0; i < sets.size(); ++i) {

    if(sets.at(i).size() > match.size() && (sets.at(i).size()) > (4 * match.size()))
      continue;

    if(match.size() > sets.at(i).size() && (match.size()) > (4 * sets.at(i).size()))
      continue;

    srcdiff_text_measure measure(sets.at(i), match);
    measure.compute();
    if(measure.similarity() > match_similarity) {

      match_pos = i;
      match_similarity = measure.similarity();

    }

  }

  return match_pos;

}

bool is_nestable_internal(const node_set & structure_one,
                          const node_set & structure_two) {

  int block = is_block_type(structure_two);

  if(block == -1)
    return false;

  /** Only can nest a block into another block if it's parent is a block */
  bool is_block = structure_one.nodes().at(structure_one.at(0))->name == "block" && structure_two.nodes().at(structure_two.at(0))->name == "block";
  bool parent_is_block = structure_one.nodes().at(structure_one.at(0))->parent && (*structure_one.nodes().at(structure_one.at(0))->parent)->name == "block";
  if(is_block && !parent_is_block) return false;

  if(is_nest_type(structure_one, structure_two, block)) {

    return true;

  }

  return false;
}

bool srcdiff_nested::is_same_nestable(const node_set & structure_one,
                                      const node_set & structure_two) {

  if(!is_nestable_internal(structure_one, structure_two))
    return false;

  //unsigned int similarity = compute_similarity(structure_one.nodes(), structure_one, structure_two.nodes(), structure_two);

  node_sets set = node_sets(structure_two.nodes(), structure_two.at(1), structure_two.back(), srcdiff_nested::is_match
                                                             , &structure_one.nodes().at(structure_one.at(0)));

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
//   && match_measure.difference() < 1.5 * difference
    && !srcdiff_nested::reject_match_nested(match_measure, set.at(match), structure_one));

}

bool srcdiff_nested::is_nestable(const node_set & structure_one,
                                 const node_set & structure_two) {

  if(srcdiff_compare::node_compare(structure_one.nodes().at(structure_one.at(0)), structure_two.nodes().at(structure_two.at(0))) == 0)
    return is_same_nestable(structure_one, structure_two);
  else
    return is_nestable_internal(structure_one, structure_two);

}

bool is_better_nest_no_recursion(const node_set & node_set_outer,
                                 const node_set & node_set_inner,
                                 const srcdiff_measure & measure) {

    if(srcdiff_nested::is_nestable(node_set_inner, node_set_outer)) {

      node_sets set = node_sets(node_set_outer.nodes(), node_set_outer.at(1), node_set_outer.back(), srcdiff_nested::is_match,
                                &node_set_inner.nodes().at(node_set_inner.at(0)));

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

bool has_compound_inner(const node_set & node_set_outer) {

  if(node_set_outer.nodes().at(node_set_outer.at(0))->is_simple) return false;

  for(unsigned int i = 1; i < node_set_outer.size(); ++i)
    if((xmlReaderTypes)node_set_outer.nodes().at(node_set_outer.at(i))->type == XML_READER_TYPE_ELEMENT
      && node_set_outer.nodes().at(node_set_outer.at(i))->name == "name" && !node_set_outer.nodes().at(node_set_outer.at(i))->is_simple)
      return true;

  return false;

}

bool is_better_nest(const node_set & node_set_outer,
                    const node_set & node_set_inner,
                    const srcdiff_measure & measure) {

  // do not nest compound name in simple or anything into something that is not compound
  if(node_set_outer.nodes().at(node_set_outer.at(0))->name == "name" && node_set_inner.nodes().at(node_set_inner.at(0))->name == "name"
    &&    (node_set_outer.nodes().at(node_set_outer.at(0))->is_simple
      || (!node_set_inner.nodes().at(node_set_inner.at(0))->is_simple && !has_compound_inner(node_set_outer)))) return false;

  // parents and children same do not nest.
  if(srcdiff_nested::is_nestable(node_set_inner, node_set_outer)) {

    node_sets set = node_sets(node_set_outer.nodes(), node_set_outer.at(1), node_set_outer.back(), srcdiff_nested::is_match
                                                           , &node_set_inner.nodes().at(node_set_inner.at(0)));

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
          && !srcdiff_nested::reject_match_nested(match_measure, node_set_inner, node_set_outer))
       )
        // check if other way is better
        return !is_better_nest_no_recursion(node_set_inner, node_set_outer, match_measure);

    }

  }

  return false;

}

bool srcdiff_nested::is_better_nested(const node_sets & node_sets_original, int start_pos_original,
                                      const node_sets & node_sets_modified, int start_pos_modified,
                                      const srcdiff_measure & measure) {

  for(int pos = start_pos_original; pos < node_sets_original.size(); ++pos) {

    int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
    check_nestable(node_sets_original, pos, pos + 1
                 , node_sets_modified, start_pos_modified, start_pos_modified + 1
                 , start_nest_original, end_nest_original, start_nest_modified, end_nest_modified
                 , operation);
    if(operation == SES_COMMON) continue;
    if(is_better_nest(node_sets_original.at(pos), node_sets_modified.at(start_pos_modified), measure))
      return true;

  }

  for(int pos = start_pos_modified; pos < node_sets_modified.size(); ++pos) {

    int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
    check_nestable(node_sets_original, start_pos_original, start_pos_original + 1
                 , node_sets_modified, pos, pos + 1
                 , start_nest_original, end_nest_original, start_nest_modified, end_nest_modified
                 , operation);
    if(operation == SES_COMMON) continue;
    if(is_better_nest(node_sets_modified.at(pos), node_sets_original.at(start_pos_original), measure))
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

bool check_nest_name(const node_set & set_original,
                     boost::optional<std::shared_ptr<srcml_node>> parent_original,
                     const node_set & set_modified,
                     boost::optional<std::shared_ptr<srcml_node>> parent_modified) {

  int original_pos = set_original.at(0);
  int modified_pos = set_modified.at(0);

  if(set_original.nodes().at(original_pos)->name == "text") return false;
  if(set_modified.nodes().at(modified_pos)->name == "text") return false;

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

    int simple_name_pos = set_original.at(0);
    if(set_original.nodes().at(simple_name_pos)->name == "name") {

      node_set inner_set(set_original.nodes(), simple_name_pos);
      srcdiff_text_measure measure(inner_set, set_modified);
      int count = measure.number_match_beginning();
      return 2 * count >= measure.max_length();

    }

  }

  if(is_call_name_modified && is_expr_name_original) {

    int simple_name_pos = set_modified.at(0);
    if(set_modified.nodes().at(simple_name_pos)->name == "name") {

      node_set inner_set(set_modified.nodes(), simple_name_pos);
      srcdiff_text_measure measure(set_original, inner_set);
      int count = measure.number_match_beginning();
      return 2 * count >= measure.max_length();

    }

  }

  return false;

}

bool srcdiff_nested::reject_match_nested(const srcdiff_measure & measure,
                                         const node_set & set_original,
                                         const node_set & set_modified) {

  int original_pos = set_original.at(0);
  int modified_pos = set_modified.at(0);

  const std::string & original_tag = set_original.nodes().at(original_pos)->name;
  const std::string & modified_tag = set_modified.nodes().at(modified_pos)->name;

  const std::string & original_uri = set_original.nodes().at(original_pos)->ns.href;
  const std::string & modified_uri = set_modified.nodes().at(modified_pos)->ns.href;

  if(original_tag != modified_tag && !srcdiff_match::is_interchangeable_match(original_tag, original_uri, modified_tag, modified_uri)) return true;

  // if interchanging decl_stmt always nest expr into init or argument
  if(original_tag == "expr" && (is_decl_stmt_from_expr(set_original.nodes(), original_pos) || is_decl_stmt_from_expr(set_modified.nodes(), modified_pos))) return false;

  if(original_tag == "name"
    && set_original.nodes().at(original_pos)->is_simple != set_modified.nodes().at(modified_pos)->is_simple
    && !check_nest_name(set_original, set_original.nodes().at(original_pos)->parent,
                       set_modified, set_modified.nodes().at(modified_pos)->parent))
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

static bool check_nested_single_to_many(const node_sets & node_sets_original, int start_original, int end_original
                 , const node_sets & node_sets_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation) {

  int nest_count_original = 0;
  boost::optional<int> pos_original;
  boost::optional<int> similarity_original;
  boost::optional<int> difference_original;
  int is_name_nest_original = 0;
  for(int i = start_original; i < end_original; ++i) {

    if(node_sets_original.nodes().at(node_sets_original.at(i).at(0))->move) continue;

    for(int j = start_modified; j < end_modified; ++j) {

      if(node_sets_modified.nodes().at(node_sets_modified.at(j).at(0))->move) continue;

      if(srcdiff_nested::is_nestable(node_sets_modified.at(j), node_sets_original.at(i))) {

        node_sets set = node_sets(node_sets_original.nodes(), node_sets_original.at(i).at(1), node_sets_original.at(i).back(), srcdiff_nested::is_match
                                                             , &node_sets_modified.nodes().at(node_sets_modified.at(j).at(0)));

        int match = srcdiff_nested::best_match(set, node_sets_modified.at(j));

        if(match >= set.size()) continue;

        srcdiff_text_measure measure(set.at(match), node_sets_modified.at(j));
        measure.compute();

        if(srcdiff_nested::reject_match_nested(measure,
                                               set.at(match),
                                               node_sets_modified.at(j)))
          continue;

        if(node_sets_modified.nodes().at(node_sets_modified.at(j).at(0))->name == "name"
          && node_sets_modified.nodes().at(node_sets_modified.at(j).at(0))->parent && (*node_sets_modified.nodes().at(node_sets_modified.at(j).at(0))->parent)->name == "expr"
          && node_sets_original.nodes().at(node_sets_original.at(i).at(0))->parent && (*node_sets_original.nodes().at(node_sets_original.at(i).at(0))->parent)->name == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1)) {
          ++is_name_nest_original;
        }

        if(node_sets_modified.nodes().at(node_sets_modified.at(j).at(0))->name == "name") {

            if(!node_sets_modified.nodes().at(node_sets_modified.at(j).at(0))->parent || !node_sets_original.nodes().at(set.at(match).at(0))->parent)
              continue;

            boost::optional<std::shared_ptr<srcml_node>> parent_original = node_sets_original.nodes().at(set.at(match).at(0))->parent;
            while((*parent_original)->name == "name")
              parent_original = (*parent_original)->parent;

            boost::optional<std::shared_ptr<srcml_node>> parent_modified = node_sets_modified.nodes().at(node_sets_modified.at(j).at(0))->parent;
            while((*parent_modified)->name == "name")
              parent_modified = (*parent_modified)->parent;

            if((*parent_original)->name != (*parent_modified)->name
              && !check_nest_name(set.at(match), parent_original,
                                  node_sets_modified.at(j), parent_modified))
              continue;

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

    if(node_sets_modified.nodes().at(node_sets_modified.at(i).at(0))->move) continue;

    for(int j = start_original; j < end_original; ++j) {

      if(node_sets_original.nodes().at(node_sets_original.at(j).at(0))->move) continue;

      if(srcdiff_nested::is_nestable(node_sets_original.at(j), node_sets_modified.at(i))) {

        node_sets set = node_sets(node_sets_modified.nodes(), node_sets_modified.at(i).at(1), node_sets_modified.at(i).back(), srcdiff_nested::is_match
                                                             , &node_sets_original.nodes().at(node_sets_original.at(j).at(0)));

        int match = srcdiff_nested::best_match(set, node_sets_original.at(j));

        if(match >= set.size()) continue;

        srcdiff_text_measure measure(node_sets_original.at(j), set.at(match));
        measure.compute();

        if(srcdiff_nested::reject_match_nested(measure,
                                               node_sets_original.at(j),
                                               set.at(match)))
          continue;

        if(node_sets_original.nodes().at(node_sets_original.at(j).at(0))->name == "name"
          && node_sets_original.nodes().at(node_sets_original.at(j).at(0))->parent && (*node_sets_original.nodes().at(node_sets_original.at(j).at(0))->parent)->name == "expr"
          && node_sets_modified.nodes().at(node_sets_modified.at(i).at(0))->parent && (*node_sets_modified.nodes().at(node_sets_modified.at(i).at(0))->parent)->name == "expr"
          && ((end_original - start_original) > 1 || (end_modified - start_modified) > 1)) {
            ++is_name_nest_modified;
          }

        if(node_sets_original.nodes().at(node_sets_original.at(j).at(0))->name == "name") {

            if(!node_sets_original.nodes().at(node_sets_original.at(j).at(0))->parent || !node_sets_modified.nodes().at(set.at(match).at(0))->parent)
              continue;

            boost::optional<std::shared_ptr<srcml_node>> parent_original = node_sets_original.nodes().at(node_sets_original.at(j).at(0))->parent;
            while(parent_original && (*parent_original)->name == "name")
              parent_original = (*parent_original)->parent;

            boost::optional<std::shared_ptr<srcml_node>> parent_modified = node_sets_modified.nodes().at(set.at(match).at(0))->parent;
            while(parent_modified && (*parent_modified)->name == "name")
              parent_modified = (*parent_modified)->parent;

            if((*parent_original)->name != (*parent_modified)->name
              && !check_nest_name(node_sets_original.at(j), parent_original,
                                  set.at(match), parent_modified))
              continue;

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

bool srcdiff_nested::check_nestable_predicate(const node_sets & node_sets_outer,
                                              int pos_outer, int start_outer, int end_outer,
                                              const node_sets & node_sets_inner,
                                              int pos_inner, int start_inner, int end_inner) {

  if(node_sets_inner.nodes().at(node_sets_inner.at(pos_inner).at(0))->move) return true;

  if(!is_nestable(node_sets_inner.at(pos_inner), node_sets_outer.at(pos_outer)))
    return true;

  node_sets set = node_sets(node_sets_outer.nodes(), node_sets_outer.at(pos_outer).at(1),
                            node_sets_outer.at(pos_outer).back(), is_match,
                            &node_sets_inner.nodes().at(node_sets_inner.at(pos_inner).at(0)));

  int match_pos = best_match(set, node_sets_inner.at(pos_inner));

  if(match_pos >= set.size()) return true;

  const node_set & match = set.at(match_pos);

  srcdiff_text_measure measure(match, node_sets_inner.at(pos_inner));
  measure.compute();

  if(reject_match_nested(measure, match, node_sets_inner.at(pos_inner)))
    return true;

  if(is_better_nest(node_sets_inner.at(pos_inner), node_sets_outer.at(pos_outer), measure))
    return true;

  if(pos_outer + 1 < end_outer && is_better_nest(node_sets_outer.at(pos_outer + 1), node_sets_inner.at(pos_inner), measure))
    return true;

  if(pos_inner + 1 < end_inner && is_better_nest(node_sets_inner.at(pos_inner + 1), node_sets_outer.at(pos_outer), measure))
    return true;

  if(node_sets_inner.nodes().at(node_sets_inner.at(pos_inner).at(0))->name == "name"
    && node_sets_inner.nodes().at(node_sets_inner.at(pos_inner).at(0))->parent && (*node_sets_inner.nodes().at(node_sets_inner.at(pos_inner).at(0))->parent)->name == "expr"
    && node_sets_outer.nodes().at(node_sets_outer.at(pos_outer).at(0))->parent && (*node_sets_outer.nodes().at(node_sets_outer.at(pos_outer).at(0))->parent)->name == "expr"
    && ((end_outer - start_outer) > 1 || (end_inner - start_inner) > 1))
    return true;

  if(node_sets_inner.nodes().at(node_sets_inner.at(pos_inner).at(0))->name == "name") {

      if(!node_sets_inner.nodes().at(node_sets_inner.at(pos_inner).at(0))->parent || !set.nodes().at(match.at(0))->parent)
        return true;

      boost::optional<std::shared_ptr<srcml_node>> parent_outer = set.nodes().at(match.at(0))->parent;
      while((*parent_outer)->name == "name")
        parent_outer = (*parent_outer)->parent;

      boost::optional<std::shared_ptr<srcml_node>> parent_inner = node_sets_inner.nodes().at(node_sets_inner.at(pos_inner).at(0))->parent;
      while((*parent_inner)->name == "name")
        parent_inner = (*parent_inner)->parent;

      if((*parent_outer)->name != (*parent_inner)->name
        && !check_nest_name(match, parent_outer,
                            node_sets_inner.at(pos_inner), parent_inner))
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

void srcdiff_nested::check_nestable(const node_sets & node_sets_original, int start_original, int end_original
                 , const node_sets & node_sets_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation) {

  start_nest_original = start_original;  
  end_nest_original = start_original;  

  start_nest_modified = start_modified;  
  end_nest_modified = start_modified;

  operation = SES_COMMON;

  if((end_original - start_original) == 1 || (end_modified - start_modified) == 1) {

    if(check_nested_single_to_many(node_sets_original, start_original, end_original,
                                node_sets_modified, start_modified, end_modified,
                                start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation))
      return;

  }

  std::vector<int> valid_nests_original;
  std::vector<int> valid_nests_modified;

  for(int i = start_original; i < end_original; ++i) {

    if(node_sets_original.nodes().at(node_sets_original.at(i).at(0))->move) continue;

    for(int j = start_modified; j < end_modified; ++j) {

      if(check_nestable_predicate(node_sets_original, i, start_original, end_original,
                                  node_sets_modified, j, start_modified, end_modified))
        continue;

      valid_nests_original.push_back(j);

      start_nest_original = i;
      end_nest_original = i + 1;

      for(int k = j + 1; k < end_modified; ++k) {

        if(check_nestable_predicate(node_sets_original, i, start_original, end_original,
                                    node_sets_modified, k, start_modified, end_modified))
          continue;

        valid_nests_original.push_back(k);

      }

      goto end_nest_check_original;

    }

  }

  end_nest_check_original:

  for(int i = start_modified; i < end_modified; ++i) {

    if(node_sets_modified.nodes().at(node_sets_modified.at(i).at(0))->move) continue;

    for(int j = start_original; j < end_original; ++j) {

      if(check_nestable_predicate(node_sets_modified, i, start_modified, end_modified,
                                  node_sets_original, j, start_original, end_original))
        continue;

      valid_nests_modified.push_back(j);

      start_nest_modified = i;
      end_nest_modified = i + 1;

      for(int k = j + 1; k < end_original; ++k) {
      
        if(check_nestable_predicate(node_sets_modified, i, start_modified, end_modified,
                                    node_sets_original, k, start_original, end_original))
          continue;

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
                  const node_sets & node_sets_outer,
                  int start_outer,
                  int end_outer,
                  const node_sets & node_sets_inner,
                  int start_inner,
                  int end_inner,
                  int operation) {

  size_t start_pos = node_sets_outer.at(start_outer).at(1);
  size_t end_pos = node_sets_outer.at(end_outer - 1).back();

  const std::string & structure_outer = node_sets_outer.nodes().at(node_sets_outer.at(start_outer).at(0))->name;
  if(structure_outer == "block_content") {
    // do not skip whitespace
    start_pos = node_sets_outer.at(start_outer).at(0) + 1;

  } else if(structure_outer == "if" || structure_outer == "elseif") {

    advance_to_child(node_sets_outer.nodes(), start_pos, (xmlElementType)XML_READER_TYPE_ELEMENT, "block");

  } else if(structure_outer == "while") {

    advance_to_child(node_sets_outer.nodes(), start_pos, (xmlElementType)XML_READER_TYPE_END_ELEMENT, "condition");
    ++start_pos;

  } else if(structure_outer == "for") {

    advance_to_child(node_sets_outer.nodes(), start_pos, (xmlElementType)XML_READER_TYPE_END_ELEMENT, "control");
    ++start_pos;

  } else if(is_class_type(structure_outer)) {

    advance_to_child(node_sets_outer.nodes(), start_pos, (xmlElementType)XML_READER_TYPE_ELEMENT, "block");
    ++start_pos;

    end_pos = start_pos - 1;
    advance_to_child(node_sets_outer.nodes(), end_pos, (xmlElementType)XML_READER_TYPE_END_ELEMENT, "block");

  }

  node_sets set = node_sets(node_sets_outer.nodes(),
                            start_pos, end_pos);

  node_sets nest_set(node_sets_inner.nodes());

  for(int i = start_inner; i < end_inner; ++i)
      nest_set.push_back(node_sets_inner.at(i));

  if(operation == SES_DELETE)
    output_change(start_pos, out.last_output_modified());
  else
    output_change(out.last_output_original(), start_pos);

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

  if(operation == SES_DELETE)
    output_change(node_sets_outer.at(end_outer - 1).back() + 1, out.last_output_modified());
  else
    output_change(out.last_output_original(), node_sets_outer.at(end_outer - 1).back() + 1);

}

void srcdiff_nested::output() {

  srcdiff_whitespace whitespace(out);

  whitespace.output_prefix();

  if(operation == SES_DELETE)
    output_inner(whitespace,
                 node_sets_original,
                 start_original,
                 end_original,
                 node_sets_modified,
                 start_modified,
                 end_modified,
                 operation);

  else
    output_inner(whitespace,
                 node_sets_modified,
                 start_modified,
                 end_modified,
                 node_sets_original,
                 start_original,
                 end_original,
                 operation);

}
