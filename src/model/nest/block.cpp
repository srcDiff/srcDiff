/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include "block.hpp"

#include <construct.hpp>

#include <string>
#include <unordered_set>

namespace nest {

block::block(const construct& client) : rule_checker(client) {
}

bool block::is_nestable_internal(const construct& modified) const {
    std::unordered_set<std::string> nestable_constructs = {
      "goto", "expr_stmt", "decl_stmt", "return", "comment", "block",
      "if_stmt", "if", "while", "for", "foreach", "else", "switch", "do",
      "try", "catch", "finally", "synchronized", "continue", "break", "goto"
    };

    /** Only can nest a block into another block if it's parent is a block */
    bool is_block = client.root_term_name() == "block" && modified.root_term_name() == "block";
    bool parent_is_block = client.root_term()->get_parent() && client.root_term()->get_parent()->get_name() == "block";
    if(is_block && !parent_is_block) return false;

    return nestable_constructs.find(modified.root_term_name()) != nestable_constructs.end();
}

}
