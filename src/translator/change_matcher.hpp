// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file change_matcher.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CHANGE_MATCHER_HPP
#define INCLUDED_CHANGE_MATCHER_HPP

#include <construct.hpp>
#include <measurer.hpp>

namespace srcdiff {

struct offset_pair {

  int original_offset;
  int original_length;
  int modified_offset;
  int modified_length;
  int similarity;
  offset_pair * next;

  std::pair<int, int> totals() {

    std::pair<int, int> sub_totals = std::make_pair(0, 0);
    for(offset_pair * current = this; current; current = current->next) {

      sub_totals.first += 1;
      sub_totals.second += current->similarity;

    }

    return sub_totals;

  }

};

class change_matcher {

public:
  change_matcher(const construct::construct_list_view original, const construct::construct_list_view modified);
  offset_pair * match_differences();

protected:

  const construct::construct_list_view original;
  const construct::construct_list_view modified;

};

}

#endif
