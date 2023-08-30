/**
 * @file class.cpp
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

#include <class.hpp>

const std::unordered_set<std::string> class_t::class_convertable = { "class", "struct", "union", "enum" };

// match rule is in named_construct

// convertable rule
bool class_t::is_tag_convertable(const construct & modified) const {
    return class_convertable.find(modified.root_term_name()) != class_convertable.end();
}

bool class_t::is_convertable_impl(const construct & modified_construct) const {

    const class_t & modified = static_cast<const class_t &>(modified_construct);

    std::string original_name = name() ? name()->to_string() : "";
    std::string modified_name = modified.name() ? modified.name()->to_string() : "";

    if(original_name != "" && original_name == modified_name) return true;
    return false;
}