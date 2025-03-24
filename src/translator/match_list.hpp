// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file match_list.hpp
 *
 * @copyright Copyright (C) 2025-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_MATCH_LIST_HPP
#define INCLUDED_MATCH_LIST_HPP

#include <construct.hpp>
#include <operation.hpp>

#include <memory>
#include <list>

namespace srcdiff {

struct match {
	std::shared_ptr<const construct> original;
	std::shared_ptr<const construct> modified;
	int similarity;
	enum operation operation;

	// Would like to eventually remove this
	int original_pos;
	int modified_pos;

	match(std::shared_ptr<const construct> original, std::shared_ptr<const construct> modified,
		  int similarity, enum operation operation, int original_pos, int modified_pos)
		: original(original), modified(modified), similarity(similarity), operation(operation),
		  original_pos(original_pos), modified_pos(modified_pos) {}
};

class match_list : public std::list<match> {
};

}

#endif