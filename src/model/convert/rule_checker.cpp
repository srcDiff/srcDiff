/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#include "rule_checker.hpp"

#include <construct.hpp>

namespace convert {

rule_checker::rule_checker(const construct& client) : client(client) {}

bool rule_checker::is_tag_convertable(const construct & modified [[maybe_unused]]) const {
  return false;
}

bool rule_checker::is_convertable(const construct & modified) const {
  if(is_convertable_impl(modified)) return true;
  return client.is_similar(modified);
}

bool rule_checker::is_convertable_impl(const construct & modified [[maybe_unused]]) const {
    return false;
}

}
