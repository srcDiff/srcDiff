/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include "call.hpp"

#include <construct.hpp>

#include <string>
#include <unordered_set>

namespace nest {

call::call(const construct& client) : rule_checker(client) {
}

bool call::can_nest_internal(const construct& modified) const {
  std::unordered_set<std::string> nestable_constructs = {
    "expr", "call", "operator", "literal", "name",
  };

  return nestable_constructs.find(modified.root_term_name()) != nestable_constructs.end();
}

}
