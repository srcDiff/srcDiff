/**
 * @file if_stmt.cpp
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

#include <if_stmt.hpp>

#include <if.hpp>

#include <srcdiff_match.hpp>

std::shared_ptr<const construct> if_stmt::find_if() const {
   if(if_child) return *if_child;

    const construct_list & childs = children();
    if(!childs.empty() && childs.front()->root_term_name() == "if") {
        if_child = childs.front();
    }

    return *if_child; 
}

std::shared_ptr<const construct> if_stmt::find_else() const {
   if(else_child) return *else_child;

    for(construct_list::const_reverse_iterator ritr = children().rbegin(); ritr != children().rend(); ++ritr) {
        std::shared_ptr<const construct> child = *ritr;
        if(child->root_term_name() == "else" 
            || (child->root_term_name() == "if" && bool(find_attribute(child->term(0), "type")))) {
            else_child = child;
            break;
        }
    }
    return *else_child;
}



std::shared_ptr<const construct> if_stmt::condition() const {
    if(condition_child) return *condition_child;

    if(find_if()) {
        condition_child = find_if()->condition();
    }

    return *condition_child;
}

bool if_stmt::is_matchable_impl(const construct & modified) const {

    const if_stmt & modified_stmt = (const if_stmt &)modified;
    if(!find_if() || !modified_stmt.find_if()) return false;

    bool original_has_block = bool(static_cast<const if_t &>(*find_if()).block());
    bool modified_has_block = bool(static_cast<const if_t &>(*modified_stmt.find_if()).block());

    if(static_cast<const if_t &>(*find_if()).is_block_matchable(*modified_stmt.find_if())) return true;

    bool condition_matchable = condition() && modified_stmt.condition() && condition()->is_matchable_impl(*modified_stmt.condition());
    bool original_has_else = bool(find_else());
    bool modified_has_else = bool(modified_stmt.find_else());

    return condition_matchable 
        &&    (original_has_block == modified_has_block 
            || original_has_else == modified_has_else 
            || original_has_block && !modified_has_else 
            || modified_has_block && !original_has_else);
}
