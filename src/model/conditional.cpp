// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file conditional.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <conditional.hpp>

// if match rule is in child class

std::shared_ptr<const construct> conditional::condition() const {
    if(condition_child) return *condition_child;

    condition_child = find_child("condition");
    return *condition_child;
}

bool conditional::is_matchable_impl(const construct & modified_construct) const {

    const conditional & modified = static_cast<const conditional &>(modified_construct);

    std::string original_condition = condition() ? condition()->to_string() : "";
    std::string modified_condition = modified.condition() ? modified.condition()->to_string() : "";

    return original_condition == modified_condition;
}
