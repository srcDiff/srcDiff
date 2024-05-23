/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#include <conditional.hpp>

conditional::conditional(const construct* parent, std::size_t& start)
    : construct(parent, start), condition_child() {
}

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

// convertable rule
bool conditional::is_tag_convertable(const construct & modified) const {
   static const std::unordered_set<std::string> conditional_convertable = { "if_stmt", "while", "for", "foreach" };
   return conditional_convertable.find(modified.root_term_name()) != conditional_convertable.end();
}

bool conditional::is_convertable_impl(const construct & modified_construct) const {

    const conditional & modified = static_cast<const conditional &>(modified_construct);

    std::string original_condition = condition() ? condition()->to_string() : "";
    std::string modified_condition = modified.condition() ? modified.condition()->to_string() : "";

    return original_condition == modified_condition;
}

