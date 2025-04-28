// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file expr.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include "expr.hpp"

#include <construct.hpp>

namespace nest {

expr_t::expr_t(const construct& client)
  : rule_checker(client, {"expr", "call", "operator", "literal", "name", "switch"}) {
}

bool expr_t::check_nest(const construct & modified) const {

    if(client.parent_term_name() == "init"     && client.parent_term()->get_parent()->get_parent()->get_name()               == "decl_stmt") return true;
    if(client.parent_term_name() == "argument" && client.parent_term()->get_parent()->get_parent()->get_parent()->get_name() == "decl_stmt") return true;

    if(modified.parent_term_name() == "init"     && modified.parent_term()->get_parent()->get_parent()->get_name()               == "decl_stmt") return true;
    if(modified.parent_term_name() == "argument" && modified.parent_term()->get_parent()->get_parent()->get_parent()->get_name() == "decl_stmt") return true;

    return client.is_similar(modified);
}

}
