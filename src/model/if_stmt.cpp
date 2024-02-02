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
#include <else.hpp>
#include <clause.hpp>

#include <srcdiff_match.hpp>

std::shared_ptr<const if_t> if_stmt::find_if() const {
   if(if_child) return *if_child;

    const construct_list & childs = children();
    if(!childs.empty() && childs.front()->root_term_name() == "if") {
        if_child = std::static_pointer_cast<const if_t>(childs.front());
    } else {
        if_child = std::shared_ptr<const if_t>();
    }

    return *if_child; 
}

std::shared_ptr<const else_t> if_stmt::find_else() const {
   if(else_child) return *else_child;

    else_child = std::shared_ptr<const else_t>();
    for(construct_list::const_reverse_iterator ritr = children().rbegin(); ritr != children().rend(); ++ritr) {
        std::shared_ptr<const construct> child = *ritr;
        if(child->root_term_name() == "else" 
            || (child->root_term_name() == "if" && bool(child->root_term()->get_attribute("type")))) {
            else_child = std::static_pointer_cast<const else_t>(child);
            break;
        }
    }

    return *else_child;
}



std::shared_ptr<const construct> if_stmt::condition() const {
    if(condition_child) return *condition_child;

    condition_child = find_if()->condition();
    return *condition_child;
}


bool if_stmt::is_syntax_similar_impl(const construct & modified_clause) const {
    const if_stmt & modified = static_cast<const if_stmt &>(modified_clause);
    std::shared_ptr<const construct> original_block = std::static_pointer_cast<const clause>(children().front())->block();
    std::shared_ptr<const construct> modified_block = std::static_pointer_cast<const clause>(modified.children().front())->block();
    return original_block->is_syntax_similar_impl(*modified_block);
}

bool if_stmt::is_matchable_impl(const construct & modified_construct) const {

    const if_stmt & modified = (const if_stmt &)modified_construct;
    if(!find_if() || !modified.find_if()) return false;

    bool original_has_block = bool(static_cast<const if_t &>(*find_if()).has_real_block());
    bool modified_has_block = bool(static_cast<const if_t &>(*modified.find_if()).has_real_block());

    if(static_cast<const if_t &>(*find_if()).is_block_matchable(*modified.find_if())) return true;

    bool condition_matchable = condition() && modified.condition() && condition()->to_string() == modified.condition()->to_string();
    bool original_has_else = bool(find_else());
    bool modified_has_else = bool(modified.find_else());

    return condition_matchable 
        &&    (original_has_block == modified_has_block 
            || original_has_else == modified_has_else 
            || original_has_block && !modified_has_else 
            || modified_has_block && !original_has_else);
}
