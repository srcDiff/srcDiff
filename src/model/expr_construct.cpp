/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <expr_construct.hpp>

#include <unordered_set>
#include <string>

std::shared_ptr<const expr_t> expr_construct::expr(const expr_construct & that [[maybe_unused]]) const {
    if(expr_child) return *expr_child;

    expr_child = std::static_pointer_cast<const expr_t>(find_child("expr"));
    return *expr_child;
}

bool expr_construct::is_tag_convertable(const construct & modified) const {
    static const std::unordered_set<std::string> expr_convertable = { "expr_stmt", "return", "decl_stmt" };
    return expr_convertable.find(modified.root_term_name()) != expr_convertable.end();    
}
bool expr_construct::is_convertable_impl(const construct & modified_construct) const {

    const expr_construct & modified = dynamic_cast<const expr_construct &>(modified_construct);

    std::shared_ptr<const construct> original_expr = expr(modified);
    std::shared_ptr<const construct> modified_expr = modified.expr(*this);

    if(!original_expr || !modified_expr) return false;
    if(!original_expr->is_similar(*modified_expr)) return false;

    const srcdiff_measure & expr_measure = *original_expr->measure(*modified_expr);
    return 2 * expr_measure.similarity() > expr_measure.max_length() && 2 * expr_measure.difference() < expr_measure.max_length();
}
