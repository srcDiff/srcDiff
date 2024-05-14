/**
 * @file expr_stmt.cpp
 *
 * @copyright Copyright (C) 2024-2024 srcML, LLC. (www.srcML.org)
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

#include <expr_stmt.hpp>

bool expr_stmt::can_nest(const construct & modified) const {
	return is_similar(modified); 
}

bool expr_stmt::is_matchable_impl(const construct & modified) const {
	const expr_stmt & modified_expr_stmt = dynamic_cast<const expr_stmt &>(modified);

	const expr_t & original_expr = dynamic_cast<const expr_t &>(*children().front());
	const expr_t & modified_expr = dynamic_cast<const expr_t &>(*modified_expr_stmt.children().front());

	if(original_expr.is_single_call() && modified_expr.is_single_call())
		return original_expr.children().front()->is_matchable_impl(*modified_expr.children().front());


	const srcdiff_measure & expr_stmt_measure = *measure(modified);
	return expr_stmt_measure.similarity() > 0 && expr_stmt_measure.difference() <= expr_stmt_measure.max_length();
}
