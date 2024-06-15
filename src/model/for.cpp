// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file for.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <for.hpp>

std::shared_ptr<const construct> for_t::control() const {
    if(control_child) return *control_child;

    control_child = find_child("control");
    return *control_child;
}

std::shared_ptr<const construct> for_t::condition() const {
    if(condition_child) return *condition_child;

    condition_child = std::shared_ptr<const construct>();
    for(std::shared_ptr<const construct> child : control()->children()) {
        if(child->root_term_name() == "condition") {
            condition_child = child;
            break;
        }
    }
    return *condition_child;
}

bool for_t::is_matchable_impl(const construct & modified) const {
    return *control() == *static_cast<const for_t &>(modified).control();
}
