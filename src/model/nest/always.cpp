// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file always.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include "always.hpp"

#include <construct.hpp>

#include <string>
#include <unordered_set>

namespace nest {

always::always(const construct& client) : rule_checker(client) {
}

bool always::can_nest_internal(const construct& modified) const {
  return true;
}

}
