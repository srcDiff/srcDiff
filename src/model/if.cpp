// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file if.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <if.hpp>

#include <srcdiff_match.hpp>

bool if_t::has_real_block() const {
    return bool(block()) && !bool(block()->root_term()->get_attribute("type"));
}

bool if_t::is_block_matchable(const construct & modified) const {
    std::shared_ptr<const construct> original_block = block();
    std::shared_ptr<const construct> modified_block = static_cast<const if_t &>(modified).block();

    if(!original_block || !modified_block) return false;
    return *original_block == *modified_block;

}

bool if_t::is_matchable_impl(const construct & modified_if) const {

    const if_t & modified = static_cast<const if_t &>(modified_if);

    std::string original_condition = condition() ? condition()->to_string() : "";
    std::string modified_condition = modified.condition() ? modified.condition()->to_string() : "";

    if(original_condition == modified_condition) return true;

    return is_block_matchable(modified);
}
