// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file case.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <case.hpp>

std::shared_ptr<const construct> case_t::expr() const {
    if(expr_child) return *expr_child;

    expr_child = find_child("expr");
    return *expr_child;
}


bool case_t::is_matchable_impl(const construct & modified) const {

    std::shared_ptr<const construct> original_expr = expr();
    std::shared_ptr<const construct> modified_expr = static_cast<const case_t &>(modified).expr();

    if(!original_expr && !modified_expr) return true;
    return original_expr && modified_expr && original_expr->to_string() == modified_expr->to_string();
}
