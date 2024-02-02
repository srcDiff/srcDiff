/**
 * @file call.cpp
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

#include <call.hpp>

#include <srcdiff_text_measure.hpp>

std::shared_ptr<const construct> call::function_name() const {
    std::shared_ptr<const name_t> full_name = name();
    if(full_name->root_term()->is_simple()) return full_name;

    return full_name->children().back();
}

bool call::is_matchable_impl(const construct & modified) const {
    std::shared_ptr<const construct> original_name = function_name();
    std::shared_ptr<const construct> modified_name = dynamic_cast<const call &>(modified).function_name();

    srcdiff_text_measure text_measure(*original_name, *modified_name, false);
    text_measure.compute();

    return bool(text_measure.similarity());
}