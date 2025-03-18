// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file expr_construct.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include "expr_construct.hpp"

#include <expr_construct.hpp>

namespace convert {

expr_construct::expr_construct(const construct& client)
  : rule_checker(client, {"expr_stmt", "return", "decl_stmt"}) {}

bool expr_construct::is_convertable_impl(const construct& modified_construct) const {

    const ::expr_construct& original = dynamic_cast<const ::expr_construct&>(client);
    const ::expr_construct& modified = dynamic_cast<const ::expr_construct&>(modified_construct);

    std::shared_ptr<const construct> original_expr = original.expr(modified);
    std::shared_ptr<const construct> modified_expr = modified.expr(original);

    if(!original_expr || !modified_expr) return false;
    if(!original_expr->is_similar(*modified_expr)) return false;

    const srcdiff::measure & expr_measure = *original_expr->measure(*modified_expr);
    return 2 * expr_measure.similarity() > expr_measure.max_length() && 2 * expr_measure.difference() < expr_measure.max_length();
}

}
