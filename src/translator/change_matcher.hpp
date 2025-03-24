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
#include <match_list.hpp>

namespace srcdiff {

class change_matcher {

public:
  change_matcher(const construct::construct_list_view original, const construct::construct_list_view modified);
  match_list match_differences();

protected:

  struct difference {
    int similarity;
    int num_unmatched;
    bool marked;
    int direction;
    unsigned int opos;
    unsigned int npos;
  };
  match_list create_linked_list(difference * differences);

protected:

  const construct::construct_list_view original;
  const construct::construct_list_view modified;

};

}

#endif
