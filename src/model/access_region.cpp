/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <access_region.hpp>

#include <unordered_set>
#include <string>

// match rule is in named_construct

// convertable rule
bool access_region::is_tag_convertable(const construct & modified) const {
    static const std::unordered_set<std::string> access_convertable = { "public", "private", "protected" };
    return access_convertable.find(modified.root_term_name()) != access_convertable.end();
}
