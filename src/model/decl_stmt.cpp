// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file decl_stmt.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <decl_stmt.hpp>

std::shared_ptr<const expr_t> decl_stmt::expr(const expr_construct & that) const {

	std::shared_ptr<const expr_t> match_expression = that.expr(*this);

	if(!match_expression) return std::shared_ptr<const expr_t>();
	return std::static_pointer_cast<const expr_t>(find_best_descendent(*match_expression));
}
