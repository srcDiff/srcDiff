// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file construct_factory.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <construct_factory.hpp>

#include <construct.hpp>
#include <named_construct.hpp>
#include <name.hpp>

#include <conditional.hpp>
#include <condition.hpp>

#include <if_stmt.hpp>
#include <if.hpp>
#include <elseif.hpp>
#include <else.hpp>

#include <for.hpp>

#include <case.hpp>
#include <call.hpp>

#include <identifier_decl.hpp>

#include <expr.hpp>
#include <expr_construct.hpp>
#include <expr_stmt.hpp>

#include <decl_stmt.hpp>

#include <always_match.hpp>

#include <block.hpp>

#include <nest/rule_checker.hpp>
#include <nest/custom.hpp>
#include <nest/similar.hpp>
#include <nest/block.hpp>
#include <nest/name.hpp>
#include <nest/expr.hpp>
#include <nest/always.hpp>
#include <nest/expr_stmt.hpp>

#include <convert/rule_checker.hpp>
#include <convert/custom.hpp>
#include <convert/class.hpp>
#include <convert/conditional.hpp>
#include <convert/else.hpp>
#include <convert/expr_construct.hpp>

#include <unordered_map>
#include <string_view>

typedef std::function<
          std::shared_ptr<construct>(const construct* parent, std::size_t& start)
        > factory_function;

template<class match_rule_checker   = construct, 
         class nest_rule_checker    = nest::rule_checker,
         class convert_rule_checker = convert::rule_checker
        >
factory_function generate_factory() {
  return  [](const construct* parent, std::size_t& start) { 

            std::shared_ptr<construct> product = std::make_shared<match_rule_checker>(parent, start);
            product->set_rule_checkers<nest_rule_checker, convert_rule_checker>();
            return product;
          };
}

typedef nest::custom<"expr", "call", "operator", "literal", "name", "switch">
        expr_nest;

typedef nest::custom<"function", "constructor", "destructor",
                          "function_decl", "constructor_decl", "destructor_decl",
                          "decl_stmt", "typedef"
                          "class", "struct", "union", "enum",
                          "class_decl", "struct_decl", "union_decl", "enum_decl">
        class_nest;

typedef nest::custom<"goto", "expr_stmt", "decl_stmt", "return", "comment", "block",
                          "if", "while", "for", "foreach", "else", "elseif", "switch", "do",
                          "try", "catch", "finally", "synchronized",
                          "expr", "call", "operator", "literal", "continue", "break", "goto">
        then_nest;

typedef nest::custom<"goto", "expr_stmt", "decl_stmt", "return", "comment", "block",
                          "if_stmt", "if", "while", "for", "foreach", "switch", "do",
                          "try", "catch", "finally", "synchronized",
                          "expr", "call", "operator", "literal", "continue", "break", "goto">
        else_nest;

typedef std::unordered_map<std::string_view, factory_function> factory_map_type;

factory_function default_factory  = generate_factory<construct>();
factory_map_type factory_map = {

  {"name", generate_factory<name_t, nest::name_t>() },

  // // class-type
  {"class",      generate_factory<named_construct, class_nest, convert::class_t>() },
  {"struct",     generate_factory<named_construct, class_nest, convert::class_t>() },
  {"union",      generate_factory<named_construct, class_nest, convert::class_t>() },
  {"enum",       generate_factory<named_construct, class_nest, convert::class_t>() },
  {"interface",  generate_factory<named_construct, class_nest, convert::class_t>() },

  // access regions
  {"public",    generate_factory<always_match, class_nest, convert::custom<"public", "private", "protected">>() },
  {"private",   generate_factory<always_match, class_nest, convert::custom<"public", "private", "protected">>() },
  {"protected", generate_factory<always_match, class_nest, convert::custom<"public", "private", "protected">>() },

  // function-type
  {"function",         generate_factory<named_construct, nest::block>() },
  {"function_decl",    generate_factory<named_construct>() },
  {"constructor",      generate_factory<named_construct>() },
  {"constructor_decl", generate_factory<named_construct>() },
  {"destructor",       generate_factory<named_construct>() },
  {"destructor_decl",  generate_factory<named_construct>() },

  // // conditionals
  {"while",     generate_factory<conditional, nest::block, convert::conditional>() },
  {"switch",    generate_factory<conditional, nest::rule_checker>() },
  {"do",        generate_factory<conditional, nest::rule_checker>() },
  {"condition", generate_factory<condition,   expr_nest>() },

  {"if_stmt", generate_factory<if_stmt, nest::block, convert::conditional>() },
  {"if",      generate_factory<if_t,    nest::block>() },
  {"elseif",  generate_factory<elseif,  nest::block, convert::else_t>() },
  {"else",    generate_factory<else_t,  else_nest, convert::else_t>() },

  {"for",     generate_factory<for_t, nest::block, convert::conditional>() },
  {"foreach", generate_factory<for_t, nest::block, convert::conditional>() },

  {"case", generate_factory<case_t>() },

  {"call",          generate_factory<call,         expr_nest>() },
  {"argument_list", generate_factory<always_match, expr_nest>() },
  {"argument",      generate_factory<always_match, expr_nest>() },
  {"expr",          generate_factory<expr_t,       nest::expr_t>() },

  {"decl",      generate_factory<named_construct, nest::custom<"expr", "switch">>() },
  {"parameter", generate_factory<identifier_decl>() },
  {"param",     generate_factory<identifier_decl>() },

  {"expr_stmt", generate_factory<expr_stmt,      nest::expr_stmt,        convert::expr_construct>() },
  {"return",    generate_factory<expr_construct, nest::custom<"switch">, convert::expr_construct>() },

  {"decl_stmt", generate_factory<decl_stmt, nest::custom<"switch">, convert::expr_construct>() },

  {"cast", generate_factory<construct, nest::rule_checker, convert::custom<"cast">>() },

  {"type",          generate_factory<always_match>() },
  {"then",          generate_factory<always_match, then_nest>() },
  {"control",       generate_factory<always_match, nest::custom<"condition", "comment", "switch">>() },
  {"init",          generate_factory<always_match, nest::custom<"expr", "switch">>() },
  {"default",       generate_factory<always_match>() },
  {"range",         generate_factory<always_match>() },
  {"signal",        generate_factory<always_match>() },

  {"literal",  generate_factory<always_match>() },
  {"modifier", generate_factory<always_match>() },

  {"number", generate_factory<always_match>() },
  {"file",   generate_factory<always_match>() },
     
  {"parameter_list",   generate_factory<always_match>() },
  {"krparameter_list", generate_factory<always_match>() },
  {"attribute_list",   generate_factory<always_match>() },
  {"association_list", generate_factory<always_match>() },
  {"protocol_list",    generate_factory<always_match>() },

  {"super_list",       generate_factory<always_match>() },
  {"member_init_list", generate_factory<always_match>() },
  {"member_list",      generate_factory<always_match>() },
  {"super_list",       generate_factory<always_match>() },

  {"block", generate_factory<block, nest::block>() },
  {"block_content", generate_factory<always_match, nest::block>() },

  {"operator", generate_factory<always_match, nest::similar>() },
  {"comment",  generate_factory<always_match, nest::similar>() },

  // nest only
  {"try",      generate_factory<construct, nest::block>() },
  {"catch",    generate_factory<construct, nest::block>() },
  {"finally",  generate_factory<construct, nest::block>() },

  {"synchronized", generate_factory<construct, nest::block>() },


  {"ternary", generate_factory<construct, nest::custom<"ternary", "call", "operator", "literal", "expr", "name">>() },

  {"extern", generate_factory<construct, nest::always>() },


  // java
  {"static", generate_factory<construct, nest::custom<"decl_stmt">>() },

  {"extends",    generate_factory<construct, nest::rule_checker, convert::custom<"extends", "implements", "permits">>() },
  {"implements", generate_factory<construct, nest::rule_checker, convert::custom<"extends", "implements", "permits">>() },
  {"permits",    generate_factory<construct, nest::rule_checker, convert::custom<"extends", "implements", "permits">>() },

};

std::shared_ptr<construct> create_construct(const construct* parent, std::size_t & start) {

  std::shared_ptr<const srcML::node> node = parent->nodes()[start];
  std::string tag_name;
  if(node->get_namespace()->get_prefix()) {
    tag_name = *node->get_namespace()->get_prefix() + ":";
  }
  tag_name += node->get_name();

  if(tag_name == "if" && bool(node->get_attribute("type"))) {
    tag_name = "elseif";
  }

  factory_map_type::const_iterator citr = factory_map.find(tag_name);
  if(citr != factory_map.end()) return citr->second(parent, start);

  return default_factory(parent, start);
}
