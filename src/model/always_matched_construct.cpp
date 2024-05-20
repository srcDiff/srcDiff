/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <always_matched_construct.hpp>

#include <unordered_set>

bool always_matched_construct::is_always_match(const std::string & construct_name) {
    static std::unordered_set<std::string> always_match_constructs =
        { "type", "then", "control", "init", "default", "comment", "signals",
          "parameter_list", "krparameter_list", "argument_list", "attribute_list", "association_list", "protocol_list",
          "super_list", "member_init_list", "member_list", "argument", "range", "literal", "operator", "modifier", "number", "file",
          // consider having this used to test similarity instead of block
          "block_content"
      };

      /** "private", "protected", "public", and "condition" are handled in subclass */
      return always_match_constructs.find(construct_name) != always_match_constructs.end();  
}

// what does this function do? unimplemented functionality?
bool always_matched_construct::is_matchable_impl(const construct & modified [[maybe_unused]]) const {
    return true;
}
