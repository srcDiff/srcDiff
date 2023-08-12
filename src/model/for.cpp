/**
 * @file for.cpp
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

#include <for.hpp>

std::shared_ptr<const construct> for_t::control() const {
    if(control_child) return *control_child;

    control_child = std::shared_ptr<const construct>();
    for(std::shared_ptr<const construct> child : children()) {
        if(child->root_term_name() == "control") {
            control_child = child;
            break;
        }
    }
    return *control_child;
}

std::shared_ptr<const construct> for_t::condition() const {
    if(condition_child) return *condition_child;

    condition_child = std::shared_ptr<const construct>();
    for(std::shared_ptr<const construct> child : control()->children()) {
        if(child->root_term_name() == "condition") {
            condition_child = child;
            break;
        }
    }
    return *condition_child;
}

bool for_t::is_matchable_impl(const construct & modified) const {
    return *control() == *static_cast<const for_t &>(modified).control();
}
