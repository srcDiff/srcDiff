/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#include <class.hpp>

#include <unordered_set>
#include <string>

// match rule is in named_construct

// convertable rule
bool class_t::is_tag_convertable(const construct & modified) const {
    static const std::unordered_set<std::string> class_convertable = { "class", "struct", "union", "enum" };
    return class_convertable.find(modified.root_term_name()) != class_convertable.end();
}

bool class_t::is_convertable_impl(const construct & modified_construct) const {

    const class_t & modified = dynamic_cast<const class_t &>(modified_construct);

    std::string original_name = name() ? name()->to_string() : "";
    std::string modified_name = modified.name() ? modified.name()->to_string() : "";

    if(original_name != "" && original_name == modified_name) return true;
    return false;
}
