/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#include "else.hpp"

#include <construct.hpp>
#include <if.hpp>

namespace convert {

else_t::else_t(const construct& client)
  : rule_checker(client, {"if", "else"}) {}

bool else_t::is_tag_convertable(const construct & modified) const {
    static const std::unordered_set<std::string> clause_convertable = { "if", "else" };
    if(clause_convertable.find(modified.root_term_name()) == clause_convertable.end()) return false;

    if(typeid(*this) == typeid(if_t)) return false;
    if(typeid(modified) == typeid(if_t)) return false;
    return true;

}

}
