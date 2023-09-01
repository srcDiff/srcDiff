/**
 * @file block.cpp
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

#include <block.hpp>

#include <srcdiff_match.hpp>

bool block::is_matchable_impl(const construct & modified) const {
    bool is_pseudo_original = bool(find_attribute(term(0), "type"));
    bool is_pseudo_modified = bool(find_attribute(modified.term(0), "type"));

    if(is_pseudo_original == is_pseudo_modified) return true;
    return false;
}
