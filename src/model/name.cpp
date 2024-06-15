// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file name.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <name.hpp>

bool name_t::is_matchable_impl(const construct & modified) const {

    if(root_term()->is_simple() && modified.root_term()->is_simple()) return true;

    return false;
}
