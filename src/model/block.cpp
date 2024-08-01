// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file block.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <block.hpp>

#include <srcdiff_match.hpp>

#include <nest/block.hpp>
#include <srcdiff_nested.hpp>

#include <srcdiff_syntax_measure.hpp>
#include <construct_factory.hpp>

std::shared_ptr<const construct> block::block_content() const {
    if(block_content_child) return *block_content_child;

    block_content_child = find_child("block_content");
    return *block_content_child;
}

bool block::is_syntax_similar_impl(const construct & modified_block) const {
    const block & modified = static_cast<const block &>(modified_block);

    srcdiff_syntax_measure syntax_measure(*block_content(), *modified.block_content());
    syntax_measure.compute();

    // max_child_length is unused
    int min_child_length = syntax_measure.min_length();
    //int max_child_length = syntax_measure.max_length();
    if(min_child_length > 1) { 
      if(2 * syntax_measure.similarity() >= min_child_length && syntax_measure.difference() <= min_child_length)
        return true;

    }

    return false;

}

bool block::is_matchable_impl(const construct & modified) const {
    bool is_pseudo_original = bool(term(0)->get_attribute("type"));
    bool is_pseudo_modified = bool(modified.term(0)->get_attribute("type"));

    if(is_pseudo_original == is_pseudo_modified) return true;

    const srcdiff_measure & measure = *this->measure(modified);
    if(!measure.similarity()) return false;

    construct_list original_stmts;
    construct_list modified_stmts;
    int match_operation = SES_INSERT;

    const block & modified_block = static_cast<const block &>(modified);

    if(is_pseudo_original) {
        original_stmts = block_content()->children();
        std::size_t start_pos = modified_block.start_position();
        modified_stmts.push_back(create_construct(modified_block.parent(), start_pos));
    } else {
        std::size_t start_pos = start_position();
        original_stmts.push_back(create_construct(parent(), start_pos));
        modified_stmts = modified_block.block_content()->children();
        match_operation = SES_DELETE;

    }

    nest_result nesting = srcdiff_nested::check_nestable(construct::construct_list_view(&original_stmts.front(), original_stmts.size()),
                                                         construct::construct_list_view(&modified_stmts.front(), modified_stmts.size()));

    return match_operation == nesting.operation;
}
