/**
 * @file conditional.cpp
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

#include <conditional.hpp>

const std::unordered_set<std::string> conditional::conditional_convertable = { "if_stmt", "while", "for", "foreach" };

// if match rule is in child class

std::shared_ptr<const construct> conditional::condition() const {
    if(condition_child) return *condition_child;

    condition_child = find_child("condition");
    return *condition_child;
}

bool conditional::is_matchable_impl(const construct & modified) const {
    std::string original_condition = condition() ? condition()->to_string() : "";
    std::string modified_condition = modified.condition() ? modified.condition()->to_string() : "";

    return original_condition == modified_condition;
}

// convertable rule
bool conditional::is_tag_convertable(const construct & modified) const {
   return conditional_convertable.find(modified.root_term_name()) != conditional_convertable.end();
}

bool conditional::is_convertable_impl(const construct & modified) const {
    std::string original_condition = condition() ? condition()->to_string() : "";
    std::string modified_condition = modified.condition() ? modified.condition()->to_string() : "";

    return original_condition == modified_condition;
}

