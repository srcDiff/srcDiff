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
#include <srcdiff_nested.hpp>

std::shared_ptr<const construct> block::block_content() const {
    if(block_content_child) return *block_content_child;

    block_content_child = find_child("block_content");
    return *block_content_child;
}

bool block::is_matchable_impl(const construct & modified) const {
    bool is_pseudo_original = bool(find_attribute(term(0), "type"));
    bool is_pseudo_modified = bool(find_attribute(modified.term(0), "type"));

    if(is_pseudo_original == is_pseudo_modified) return true;

	const srcdiff_measure & measure = *this->measure(modified);
	if(!measure.similarity()) return false;

	construct_list original_stmts;
	construct_list modified_stmts;
	int match_operation = SES_INSERT;

	const block & modified_block = static_cast<const block &>(modified);

	if(is_pseudo_original) {
		original_stmts = block_content()->children();
		int start_pos = modified_block.start_position();
		modified_stmts.push_back(std::make_shared<block>(modified_block.nodes(), start_pos, modified_block.out));
	} else {
		int start_pos = start_position();
		original_stmts.push_back(std::make_shared<block>(nodes(), start_pos, out));
		modified_stmts = modified_block.block_content()->children();
		match_operation = SES_DELETE;

	}

    int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
    srcdiff_nested::check_nestable(original_stmts, 0, original_stmts.size(), modified_stmts, 0, modified_stmts.size(),
                     			   start_nest_original, end_nest_original, start_nest_modified , end_nest_modified, operation);

    return match_operation == operation;
}
