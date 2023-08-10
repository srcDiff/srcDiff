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

std::shared_ptr<const construct> if_stmt::condition() const {
    if(condition_child) return *condition_child

    const construct_list & children = children();
    if(!children.empty() && children.front().root_term_name() == "if") {
        condition_child = childlren.front()->condition();
    }

    return *condition_child;
}

virtual bool is_matchable_impl(const construct & modified) const {
    const construct_list & original_children = children();
    const construct_list & modified_children = modified.children();

    if(original_children.empty() || modified_children.empty()) return false;

    bool condition_machable = condition() && modified.condition() && condition->is_matchable_impl(modified.condition());

    return false;
}


// else if(original_tag == "if_stmt") {

//     std::shared_ptr<construct> first_original = get_first_child(*this);
//     std::shared_ptr<construct> first_modified = get_first_child(modified);

//     if(is_child_if(*first_original) && is_child_if(*first_modified)) {

//       /** todo play with getting and checking a match with all conditions */
//       std::string original_condition = get_condition(nodes(), original_pos);
//       std::string modified_condition = get_condition(modified.nodes(), modified_pos);

//       bool original_has_block = conditional_has_block(*first_original);
//       bool modified_has_block = conditional_has_block(*first_modified);

//       bool original_has_else = if_stmt_has_else(*this);
//       bool modified_has_else = if_stmt_has_else(modified);

//       if(if_block_equal(*first_original, *first_modified)
//         || (original_condition == modified_condition
//           && ( original_has_block == modified_has_block 
//             || original_has_else == modified_has_else 
//             || (original_has_block && !modified_has_else) 
//             || (modified_has_block && !original_has_else)))) {
//         return true;
//       }
//     }

//   } 