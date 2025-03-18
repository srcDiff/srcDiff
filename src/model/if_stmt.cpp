// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file if_stmt.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <if_stmt.hpp>

#include <clause.hpp>
#include <elseif.hpp>

std::shared_ptr<const if_t> if_stmt::find_if() const {
   if(if_child) return *if_child;

    const construct_list & childs = children();
    if(!childs.empty() && childs.front()->root_term_name() == "if") {
        if(childs.front()->root_term()->get_attribute("type")) {
            if_child = std::static_pointer_cast<const elseif>(childs.front())->find_if();
        } else {
            if_child = std::static_pointer_cast<const if_t>(childs.front());
        }
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
    // parens are suggested around these statements
    return condition_matchable 
        &&    ((original_has_block == modified_has_block) 
            || (original_has_else == modified_has_else) 
            || (original_has_block && !modified_has_else) 
            || (modified_has_block && !original_has_else));
}
