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

#include <elseif.hpp>


std::shared_ptr<const construct> elseif::find_if() const {
   if(if_child) return *if_child;

    if_child = std::shared_ptr<const construct>();
    for(construct_list::const_reverse_iterator ritr = children().rbegin(); ritr != children().rend(); ++ritr) {
        std::shared_ptr<const construct> child = *ritr;
        if(child->root_term_name() == "if") {
            if_child = child;
            break;
        }
    }

    return *if_child; 
}

std::shared_ptr<const construct> elseif::condition() const {
    return find_if()->condition();
}

std::shared_ptr<const construct> elseif::block() const {
    return static_cast<const if_t &>(*find_if()).block();
}
