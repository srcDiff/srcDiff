/**
 * @file expr_construct.cpp
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

#include <expr_construct.hpp>

#include <unordered_set>
#include <string>

std::shared_ptr<const expr_t> expr_construct::expr() const {
    if(expr_child) return *expr_child;

    expr_child = std::static_pointer_cast<const expr_t>(find_child("expr"));
    return *expr_child;
}

bool expr_construct::is_tag_convertable(const construct & modified) const {
    static const std::unordered_set<std::string> expr_convertable = { "expr_stmt", "return", "decl_stmt" };
    return expr_convertable.find(modified.root_term_name()) != expr_convertable.end();    
}
bool expr_construct::is_convertable_impl(const construct & modified) const {
    return false;
}
