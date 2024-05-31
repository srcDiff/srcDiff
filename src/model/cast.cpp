/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#include <cast.hpp>

#include <unordered_set>
#include <string>

// match rule is in named_construct

// convertable rule
bool cast::is_tag_convertable(const construct & modified) const {
    return modified.root_term_name() == "cast";
}
