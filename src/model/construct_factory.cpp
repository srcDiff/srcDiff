/**
 * @file construct_factory.cpp
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

#include <srcdiff_match.hpp>

#include <unordered_map>
#include <string_view>

typedef std::function<std::shared_ptr<construct>(const srcml_nodes & node_list,
                                                 int & start,
                                                 std::shared_ptr<srcdiff_output> out)
                     > factory_function;
typedef std::unordered_map<std::string_view, factory_function> factory_map_type;

factory_function default_factory  = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<construct>(node_list, start, out); };
factory_function class_factory    = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<class_t>(node_list, start, out); };
factory_function named_factory    = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<named_construct>(node_list, start, out); };
factory_function name_factory     = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<name_t>(node_list, start, out); };

factory_function conditional_factory = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<conditional>(node_list, start, out); };
factory_function condition_factory   = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<condition>(node_list, start, out); };

factory_function if_stmt_factory   = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<if_stmt>(node_list, start, out); };
factory_function if_factory        = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<if_t>(node_list, start, out); };
factory_function elseif_factory    = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<elseif>(node_list, start, out); };
factory_function else_factory      = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<else_t>(node_list, start, out); };

factory_function for_factory       = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<for_t>(node_list, start, out); };

factory_function case_factory      = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<case_t>(node_list, start, out); };
factory_function call_factory      = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<call>(node_list, start, out); };

factory_function identifier_decl_factory = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<identifier_decl>(node_list, start, out); };

factory_function expr_factory           = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<expr_t>(node_list, start, out); };
factory_function expr_construct_factory = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<expr_construct>(node_list, start, out); };
factory_function expr_stmt_factory = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<expr_stmt>(node_list, start, out); };
factory_function decl_stmt_factory = [](const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) { return std::make_shared<decl_stmt>(node_list, start, out); };

factory_map_type factory_map = {
  // default
  {"construct", default_factory },

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
  {"decl_stmt", identifier_decl_factory },
  {"parameter", identifier_decl_factory },
  {"param",     identifier_decl_factory },

  {"expr",      expr_factory },
  {"expr_stmt", expr_stmt_factory },
  {"return",    expr_construct_factory },
  {"decl_stmt", decl_stmt_factory },

};

std::shared_ptr<construct> create_construct(const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) {

  std::shared_ptr<const srcml_node> node = node_list[start];
  std::string tag_name;
  if(node->ns.prefix) {
    tag_name = *node->ns.prefix + ":";
  }
  tag_name += node->name;

  if(tag_name == "if" && bool(find_attribute(node, "type"))) {
    tag_name = "elseif";
  }

  factory_map_type::const_iterator citr = factory_map.find(tag_name);
  if(citr == factory_map.end()) tag_name = "construct";

  return factory_map[tag_name](node_list, start, out);
}

