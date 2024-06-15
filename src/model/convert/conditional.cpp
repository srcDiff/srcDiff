// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file conditional.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include "conditional.hpp"

#include <construct.hpp>
#include <conditional.hpp>

namespace convert {

conditional::conditional(const construct& client)
  : rule_checker(client, {"if_stmt", "while", "for", "foreach"}) {}

bool conditional::is_convertable_impl(const construct & modified_construct) const {

    const ::conditional & original = static_cast<const ::conditional &>(client);
    const ::conditional & modified = static_cast<const ::conditional &>(modified_construct);

    std::string original_condition = original.condition() ? original.condition()->to_string() : "";
    std::string modified_condition = modified.condition() ? modified.condition()->to_string() : "";

    return original_condition == modified_condition;
}


}
