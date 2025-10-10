// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file specifier.cpp
 *
 * @copyright Copyright (C) 2025-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <specifier.hpp>

const std::unordered_set<std::string> access_specifiers
 = { "public", "private", "protected", };

// what does this function do? unimplemented functionality?
bool specifier::is_matchable_impl(const construct& modified) const {
    const std::string original_str = to_string(true);
    const std::string modified_str = modified.to_string(true);
    if(access_specifiers.contains(original_str) && access_specifiers.contains(modified_str)) {
        return true;
    }

    return false;
}
