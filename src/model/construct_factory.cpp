/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#include <construct_factory.hpp>

#include <construct.hpp>
#include <named_construct.hpp>
#include <class.hpp>
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

#include <cast.hpp>

#include <access_region.hpp>

#include <always_matched_construct.hpp>

#include <block.hpp>

#include <operator.hpp>
#include <comment.hpp>

#include <srcdiff_match.hpp>

#include <nest/rule_checker.hpp>
#include <nest/custom_nest.hpp>
#include <nest/block.hpp>

#include <unordered_map>
#include <string_view>

typedef std::function<
          std::shared_ptr<construct>(const construct* parent, std::size_t& start)
        > factory_function;

template<class match_rule_checker = construct, class nest_rule_checker = nest::rule_checker>
factory_function generate_factory() {
  return  [](const construct* parent, std::size_t& start) { 
            return std::make_shared<match_rule_checker>(parent, start, std::shared_ptr<nest_rule_checker>());
          };
}

typedef nest::custom_nest<"expr", "call", "operator", "literal", "name">
        call_nest;
typedef nest::custom_nest<"function", "constructor", "destructor",
                          "function_decl", "constructor_decl", "destructor_decl",
                          "decl_stmt", "typedef"
                          "class", "struct", "union", "enum",
                          "class_decl", "struct_decl", "union_decl", "enum_decl">
        class_nest;
typedef nest::custom_nest<"goto", "expr_stmt", "decl_stmt", "return", "comment", "block",
                          "if", "while", "for", "foreach", "else", "elseif", "switch", "do",
                          "try", "catch", "finally", "synchronized",
                          "expr", "call", "operator", "literal", "continue", "break", "goto">
        then_nest;

typedef nest::custom_nest<"goto", "expr_stmt", "decl_stmt", "return", "comment", "block",
                          "if_stmt", "if", "while", "for", "foreach", "switch", "do",
                          "try", "catch", "finally", "synchronized",
                          "expr", "call", "operator", "literal", "continue", "break", "goto">
        else_nest;

typedef nest::custom_nest<"decl_stmt", "function_decl", "function", "class", "class_decl",
                          "struct", "struct_decl", "union", "union_decl", "typedef", "using">
        extern_nest;

typedef std::unordered_map<std::string_view, factory_function> factory_map_type;

factory_function default_factory  = generate_factory<construct>();
factory_map_type factory_map = {

  {"name", generate_factory<name_t, nest::custom_nest<"name">>() },

  // // class-type
  {"class",  generate_factory<class_t, class_nest>() },
  {"struct", generate_factory<class_t, class_nest>() },
  {"union",  generate_factory<class_t, class_nest>() },
  {"enum",   generate_factory<class_t, class_nest>() },

  // access regions
  {"public",    generate_factory<access_region, class_nest>() },
  {"private",   generate_factory<access_region, class_nest>() },
  {"protected", generate_factory<access_region, class_nest>() },

  // function-type
  {"function",         generate_factory<named_construct, nest::block>() },
  {"function_decl",    generate_factory<named_construct>() },
  {"constructor",      generate_factory<named_construct>() },
  {"constructor_decl", generate_factory<named_construct>() },
  {"destructor",       generate_factory<named_construct>() },
  {"destructor_decl",  generate_factory<named_construct>() },

  // // conditionals
  {"while",     generate_factory<conditional, nest::block>() },
  {"switch",    generate_factory<conditional>() },
  {"do",        generate_factory<conditional>() },
  {"condition", generate_factory<condition, nest::custom_nest<"expr", "call", "operator", "literal", "name">>() },

  {"if_stmt", generate_factory<if_stmt, nest::block>() },
  {"if",      generate_factory<if_t, nest::block>() },
  {"elseif",  generate_factory<elseif, nest::block>() },
  {"else",    generate_factory<else_t, else_nest>() },

  {"for",     generate_factory<for_t, nest::block>() },
  {"foreach", generate_factory<for_t, nest::block>() },

  {"case", generate_factory<case_t>() },

  {"call",          generate_factory<call, call_nest>() },
  {"argument_list", generate_factory<always_matched_construct, call_nest>() },
  {"argument",      generate_factory<always_matched_construct, call_nest>() },
  {"expr",          generate_factory<expr_t, call_nest>() },

  {"decl",      generate_factory<named_construct, nest::custom_nest<"expr">>() },
  {"parameter", generate_factory<identifier_decl>() },
  {"param",     generate_factory<identifier_decl>() },

  {"expr_stmt", generate_factory<expr_stmt>() },
  {"return",    generate_factory<expr_construct>() },

  {"decl_stmt", generate_factory<decl_stmt>() },

  {"cast", generate_factory<cast>() },

  {"type",          generate_factory<always_matched_construct>() },
  {"then",          generate_factory<always_matched_construct, then_nest>() },
  {"control",       generate_factory<always_matched_construct, nest::custom_nest<"condition", "comment">>() },
  {"init",          generate_factory<always_matched_construct, nest::custom_nest<"expr">>() },
  {"default",       generate_factory<always_matched_construct>() },
  {"range",         generate_factory<always_matched_construct>() },
  {"signal",        generate_factory<always_matched_construct>() },

  {"literal",  generate_factory<always_matched_construct>() },
  {"modifier", generate_factory<always_matched_construct>() },

  {"number", generate_factory<always_matched_construct>() },
  {"file",   generate_factory<always_matched_construct>() },
     
  {"parameter_list",   generate_factory<always_matched_construct>() },
  {"krparameter_list", generate_factory<always_matched_construct>() },
  {"attribute_list",   generate_factory<always_matched_construct>() },
  {"association_list", generate_factory<always_matched_construct>() },
  {"protocol_list",    generate_factory<always_matched_construct>() },

  {"super_list",       generate_factory<always_matched_construct>() },
  {"member_init_list", generate_factory<always_matched_construct>() },
  {"member_list",      generate_factory<always_matched_construct>() },
  {"super_list",       generate_factory<always_matched_construct>() },

  {"block", generate_factory<block, nest::block>() },
  {"block_content", generate_factory<always_matched_construct, nest::block>() },

  {"operator", generate_factory<operator_t>() },
  {"comment",  generate_factory<comment_t>() },

  // nest only
  {"try",      generate_factory<construct, nest::block>() },
  {"catch",    generate_factory<construct, nest::block>() },
  {"finally",  generate_factory<construct, nest::block>() },

  {"synchronized", generate_factory<construct, nest::block>() },

  {"static", generate_factory<construct, nest::custom_nest<"decl_stmt">>() },

  {"ternary", generate_factory<construct, nest::custom_nest<"ternary", "call", "operator", "literal", "expr", "name">>() },

  {"extern", generate_factory<construct, extern_nest>() },

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
