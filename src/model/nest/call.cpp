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

const string_set call_nestable{"expr", "call", "operator", "literal", "name"};

call::call(const construct& client) : rule_checker(client, call_nestable) {
}

}
