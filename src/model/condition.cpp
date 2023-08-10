/**
 * @file condition.cpp
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

#include <condition.hpp>

std::string condition::to_string(bool skip_whitespacee) const {
    std::string str = construct::to_string(skip_whitespacee);

    if(!str.empty() && str.front() == '(') {
        str.erase(str.begin());
    }

    if(!str.empty() && str.back() == ')') {
        str.pop_back();
    }

    return str;
}

bool condition::is_matchable_impl(const construct & modified) const {
    std::string original_condition = to_string();
    std::string modified_condition = modified.to_string();

    return original_condition == modified_condition;
}
