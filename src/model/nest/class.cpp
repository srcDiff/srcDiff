/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include "class.hpp"

#include <construct.hpp>

#include <string>
#include <unordered_set>

namespace nest {

string_set class_nestable{
    "function", "constructor", "destructor",
    "function_decl", "constructor_decl", "destructor_decl",
    "decl_stmt", "typedef"
    "class", "struct", "union", "enum",
    "class_decl", "struct_decl", "union_decl", "enum_decl"                                                
  };

class_t::class_t(const construct& client) : rule_checker(client, class_nestable) {
}

}
