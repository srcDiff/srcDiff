// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file rule_checker.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include "rule_checker.hpp"

#include <construct.hpp>

namespace convert {

rule_checker::rule_checker(const construct& client, const string_set& convertable_constructs)
  : client(client), convertable_constructs(convertable_constructs) {}

bool rule_checker::is_tag_convertable(const construct & modified) const {
  return convertable_constructs.find(modified.root_term_name()) != convertable_constructs.end();;;
}

bool rule_checker::is_convertable(const construct & modified) const {
  if(is_convertable_impl(modified)) return true;
  return client.is_similar(modified);
}

bool rule_checker::is_convertable_impl(const construct & modified [[maybe_unused]]) const {
    return false;
}

}
