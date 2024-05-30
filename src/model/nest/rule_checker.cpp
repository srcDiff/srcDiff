/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#include "rule_checker.hpp"

#include <construct.hpp>
#include <srcdiff_text_measure.hpp>

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
const nest_info nestable[] = {   

  { "then",          ternary_then_nest_types },
  { "else",          else_nest_types         },
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
  { "extern",        extern_nest_types       },
  { "decl",          decl_nest_types         },
  { "init",          decl_nest_types         },

  // Java
  { "static",        static_nest_types       },
  { "synchronized",  block_nest_types        },
  { "finally",       block_nest_types        },
  { 0, 0 }

};

int is_block_type(const construct& structure) {

  if(structure.root_term()->get_namespace()->get_uri() != SRCML_SRC_NAMESPACE_HREF)
    return -1;

  for(int i = 0; nestable[i].type; ++i)
    if(structure.root_term_name() == nestable[i].type)
      return i;

  return -1;
}

bool has_internal_structure(const construct& structure, const std::optional<std::string> & type) {

  if(!type) return false;

  for(unsigned int i = 1; i < structure.size(); ++i) {
    if(structure.term(i)->is_start()
       && structure.term(i)->get_name() == type)
      return true;
  }

  return false;
}

bool is_nest_type(const construct& structure,
                  const construct& structure_other,
                  int type_index) {

  if(structure.root_term()->get_namespace()->get_uri() != SRCML_SRC_NAMESPACE_HREF)
    return true;

  for(int i = 0; nestable[type_index].possible_nest_items[i]; ++i) {
    if(structure.root_term_name() == nestable[type_index].possible_nest_items[i]
       && has_internal_structure(structure_other, structure.root_term_name()))
      return true;
  }

  return false;
}



namespace nest {

rule_checker::rule_checker(const construct& client)
    : client(client) {}

bool rule_checker::can_nest(const construct& modified) const {
    if(*client.root_term() == *modified.root_term())
        return can_nest_same(modified);
    else
        return can_nest_internal(modified);
}

bool rule_checker::can_nest_internal(const construct& modified) const {

  int block = is_block_type(client);
  if(block == -1) return false;

  return is_nest_type(modified, client, block);
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
