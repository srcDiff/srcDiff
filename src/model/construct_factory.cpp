/*
 * SPDX-License-Identifier: GPL-3.0-only

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

#include <unordered_map>
#include <string_view>

typedef std::function<std::shared_ptr<construct>(const construct* parent,
                                                 std::size_t & start)
                     > factory_function;
typedef std::unordered_map<std::string_view, factory_function> factory_map_type;

factory_function default_factory  = [](const construct* parent, std::size_t& start) { return std::make_shared<construct>(parent, start); };
factory_function class_factory    = [](const construct* parent, std::size_t& start) { return std::make_shared<class_t>(parent, start); };
factory_function named_factory    = [](const construct* parent, std::size_t& start) { return std::make_shared<named_construct>(parent, start); };
factory_function name_factory     = [](const construct* parent, std::size_t& start) { return std::make_shared<name_t>(parent, start); };

factory_function conditional_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<conditional>(parent, start); };
factory_function condition_factory   = [](const construct* parent, std::size_t& start) { return std::make_shared<condition>(parent, start); };

factory_function if_stmt_factory   = [](const construct* parent, std::size_t& start) { return std::make_shared<if_stmt>(parent, start); };
factory_function if_factory        = [](const construct* parent, std::size_t& start) { return std::make_shared<if_t>(parent, start); };
factory_function elseif_factory    = [](const construct* parent, std::size_t& start) { return std::make_shared<elseif>(parent, start); };
factory_function else_factory      = [](const construct* parent, std::size_t& start) { return std::make_shared<else_t>(parent, start); };

factory_function for_factory       = [](const construct* parent, std::size_t& start) { return std::make_shared<for_t>(parent, start); };

factory_function case_factory      = [](const construct* parent, std::size_t& start) { return std::make_shared<case_t>(parent, start); };
factory_function call_factory      = [](const construct* parent, std::size_t& start) { return std::make_shared<call>(parent, start); };

factory_function identifier_decl_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<identifier_decl>(parent, start); };

factory_function expr_factory           = [](const construct* parent, std::size_t& start) { return std::make_shared<expr_t>(parent, start); };
factory_function expr_construct_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<expr_construct>(parent, start); };
factory_function expr_stmt_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<expr_stmt>(parent, start); };

factory_function decl_stmt_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<decl_stmt>(parent, start); };

factory_function cast_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<cast>(parent, start); };

factory_function access_region_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<access_region>(parent, start); };

factory_function always_match_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<always_matched_construct>(parent, start); };

factory_function block_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<block>(parent, start); };

factory_function operator_factory = [](const construct* parent, std::size_t& start) { return std::make_shared<operator_t>(parent, start); };
factory_function comment_factory =  [](const construct* parent, std::size_t& start) { return std::make_shared<comment_t>(parent, start); };

factory_map_type factory_map = {

  {"name", name_factory },

  // class-type
  {"class",  class_factory },
  {"struct", class_factory },
  {"union",  class_factory },
  {"enum",   class_factory },

  // function-type
  {"function",         named_factory },
  {"function_decl",    named_factory },
  {"constructor",      named_factory },
  {"constructor_decl", named_factory },
  {"destructor",       named_factory },
  {"destructor_decl",  named_factory },

  // conditionals
  {"while",     conditional_factory },
  {"switch",    conditional_factory },
  {"do",        conditional_factory },
  {"condition", condition_factory },

  {"if_stmt", if_stmt_factory },
  {"if",      if_factory },
  {"elseif",  elseif_factory },
  {"else",    else_factory },

  {"for",     for_factory },
  {"foreach", for_factory },

  {"case", case_factory },
  {"call", call_factory },

  {"decl",      named_factory },
  {"parameter", identifier_decl_factory },
  {"param",     identifier_decl_factory },

  {"expr",      expr_factory },
  {"expr_stmt", expr_stmt_factory },
  {"return",    expr_construct_factory },

  {"decl_stmt", decl_stmt_factory },

  {"cast", cast_factory },

  {"type",          always_match_factory },
  {"then",          always_match_factory },
  {"control",       always_match_factory },
  {"init",          always_match_factory },
  {"default",       always_match_factory },
  {"argument",      always_match_factory },
  {"range",         always_match_factory },
  {"block_content", always_match_factory },
  {"signal",        always_match_factory },

  {"literal",  always_match_factory },
  {"modifier", always_match_factory },

  {"number", always_match_factory },
  {"file",   always_match_factory },
     
  {"parameter_list",   always_match_factory },
  {"krparameter_list", always_match_factory },
  {"argument_list",    always_match_factory },
  {"attribute_list",   always_match_factory },
  {"association_list", always_match_factory },
  {"protocol_list",    always_match_factory },

  {"super_list",       always_match_factory },
  {"member_init_list", always_match_factory },
  {"member_list",      always_match_factory },
  {"super_list",       always_match_factory },

  {"public",    access_region_factory },
  {"private",   access_region_factory },
  {"protected", access_region_factory },

  {"block", block_factory },

  {"operator", operator_factory },
  {"comment",  comment_factory },

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
