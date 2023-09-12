/**
 * @file clause.cpp
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
