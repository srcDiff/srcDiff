/**
 * @file always_matched_construct.cpp
 *
 * @copyright Copyright (C) 2023-2023 srcML, LLC. (www.srcML.org)
 *
 * srcDiff is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * srcDiff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
bool always_matched_construct::is_matchable_impl(const construct & modified) const {
    return true;
}