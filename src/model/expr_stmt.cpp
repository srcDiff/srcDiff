/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
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
