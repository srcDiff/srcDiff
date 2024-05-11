/**
 * @file expr.cpp
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

#include <expr.hpp>

bool expr_t::is_single_call() const {
    if(children().size() != 1) return false;
    return children().front()->root_term_name() == "call";
}

// bool expr_t::can_nest(const construct & modified) const {
//     return is_similar(modified);
// }

bool expr_t::is_matchable_impl(const construct & modified) const {
    const srcdiff_measure & expr_measure = *measure(modified);
    if(expr_measure.similarity() > 0 && expr_measure.difference() <= expr_measure.max_length()) return true;

    if((*root_term()->get_parent())->get_name() == "argument") return true;
    if((*modified.root_term()->get_parent())->get_name() == "argument") return true;

    return children().size() == 1 && children()[0]->root_term_name() == "name";
}
