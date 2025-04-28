// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file class.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include "class.hpp"

#include <construct.hpp>
#include <named_construct.hpp>

namespace convert {

class_t::class_t(const construct& client)
  : rule_checker(client, {"class", "struct", "union", "enum", "interface"}) {}

bool class_t::is_convertable_impl(const construct & modified_construct) const {

    const named_construct & original = dynamic_cast<const named_construct &>(client);
    const named_construct & modified = dynamic_cast<const named_construct &>(modified_construct);

    std::string original_name = original.name() ? original.name()->to_string() : "";
    std::string modified_name = modified.name() ? modified.name()->to_string() : "";

    if(original_name != "" && original_name == modified_name) return true;
    return false;
}

}
