/**
 * @file function.cpp
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

#include <function.hpp>

#include <name.hpp>

//temp, probably
#include <srcdiff_match_internal.hpp>

bool function::is_matchable_impl(const construct & modified) const {

    /// @todo maybe expand children instead and grab name construct and compare?
    const construct_list & original_children = children();
    std::string original_name;
    for(std::shared_ptr<const construct> child : original_children) {
        if(typeid(*child) == typeid(name)) {
            original_name = child->to_string();
            break;
        }
    }

    const construct_list & modified_children = modified.children();
    std::string modified_name;
    for(std::shared_ptr<const construct> child : modified_children) {
        if(typeid(*child) == typeid(name)) {
            modified_name = child->to_string();
            break;
        }
    }

    if(original_name == modified_name) return true;

    return false;
}