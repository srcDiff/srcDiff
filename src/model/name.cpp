/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <name.hpp>

std::string name_t::simple_name() const {
    if(root_term()->is_simple()) return to_string();

    const construct_list childs = children();
    if(childs.empty() || childs[0]->root_term_name() != "name") return "";

    return childs[0]->to_string();
}

bool name_t::is_matchable_impl(const construct & modified) const {

    if(root_term()->is_simple() && modified.root_term()->is_simple()) return true;

    return false;
}
