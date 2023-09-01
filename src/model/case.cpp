/**
 * @file case.cpp
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