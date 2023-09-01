/**
 * @file access_region.cpp
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

#include <access_region.hpp>

#include <unordered_set>
#include <string>

// match rule is in named_construct

// convertable rule
bool access_region::is_tag_convertable(const construct & modified) const {
    static const std::unordered_set<std::string> access_convertable = { "public", "private", "protected" };
    return access_convertable.find(modified.root_term_name()) != access_convertable.end();
}
