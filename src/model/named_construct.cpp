// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file named_construct.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <named_construct.hpp>

std::shared_ptr<const name_t> named_construct::name() const {
    if(name_child) return *name_child;

    name_child = std::static_pointer_cast<const name_t>(find_child("name"));
    if(!name_child) name_child = std::shared_ptr<const name_t>();

    return *name_child;
}

bool named_construct::is_matchable_impl(const construct & modified_construct) const {

    const named_construct & modified = dynamic_cast<const named_construct &>(modified_construct);

    std::string original_name = name() ? name()->to_string() : "";
    std::string modified_name = modified.name() ? modified.name()->to_string() : "";

    if(original_name == modified_name) return true;

    return false;
}
