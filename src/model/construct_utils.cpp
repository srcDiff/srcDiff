// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file construct_utils.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <construct_utils.hpp>
#include <construct.hpp>

namespace std {

  size_t hash<construct>::operator()(const construct & element) const {

    size_t result = 2166136261;
    for(std::size_t pos = 0, size = element.size(); pos < size; ++pos) {

        const shared_ptr<srcML::node> & term = element.term(pos);
        const string & hash_item = term->is_text() ? *term->get_content() : term->get_name();
        for(size_t hash_pos = 0, hash_size = hash_item.size(); hash_pos < hash_size; ++hash_pos) {
            result = (result * 16777619) ^ hash_item[hash_pos];
        }

    }

    return result;

  }

  size_t hash<std::shared_ptr<const construct>>::operator()(const std::shared_ptr<const construct> & element) const {
    if(!element) return 0;
    else return element->hash();
  }

  bool equal_to<std::shared_ptr<const construct>>::operator()(std::shared_ptr<const construct> self, std::shared_ptr<const construct> that) const {
    if(self == that) return true;
    if(!self || !that) return false;
    return *self == *that;
  }

}
