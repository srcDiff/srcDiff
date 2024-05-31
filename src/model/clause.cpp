/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <clause.hpp>

#include <if.hpp>

std::shared_ptr<const construct> clause::block() const {
    if(block_child) return *block_child;

    block_child = find_child("block");
    return *block_child;
}

bool clause::is_tag_convertable(const construct & modified) const {
    static const std::unordered_set<std::string> clause_convertable = { "if", "else" };
    if(clause_convertable.find(modified.root_term_name()) == clause_convertable.end()) return false;

    if(typeid(*this) == typeid(if_t)) return false;
    if(typeid(modified) == typeid(if_t)) return false;
    return true;

}
