/**
 * @file if.cpp
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

#include <if.hpp>

#include <srcdiff_match.hpp>

bool if_t::has_real_block() const {
    return bool(block()) && !bool(find_attribute(block()->root_term(), "type"));
}

bool if_t::is_block_matchable(const construct & modified) const {
    std::shared_ptr<const construct> original_block = block();
    std::shared_ptr<const construct> modified_block = static_cast<const if_t &>(modified).block();

    if(!original_block || !modified_block) return false;
    return *original_block == *modified_block;

}

bool if_t::is_matchable_impl(const construct & modified_if) const {

    const if_t & modified = static_cast<const if_t &>(modified_if);

    std::string original_condition = condition() ? condition()->to_string() : "";
    std::string modified_condition = modified.condition() ? modified.condition()->to_string() : "";

    if(original_condition == modified_condition) return true;

    return is_block_matchable(modified);
}
