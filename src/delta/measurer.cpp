// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file measurer.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <measurer.hpp>

#include <constants.hpp>
#include <construct.hpp>

#include <algorithm>
#include <cassert>

namespace srcdiff {

measurer::measurer(const construct & set_original, const construct & set_modified) 
  : set_original(set_original),
    set_modified(set_modified),
    computed(false),
    a_similarity(0),
    a_original_difference(0),
    a_modified_difference(0),
    original_len(0),
    modified_len(0) {}

int measurer::similarity() const {

  assert(computed);

  return a_similarity;

}

int measurer::difference() const {

  assert(computed);
  if(a_original_difference == MAX_INT) return MAX_INT;

  return a_original_difference + a_modified_difference;

}

int measurer::original_difference() const {

  assert(computed);

  return a_original_difference;

}

int measurer::modified_difference() const {

  assert(computed);

  return a_modified_difference;

}

int measurer::original_length() const {

  assert(computed);

  return original_len;

}

int measurer::modified_length() const {

  assert(computed);

  return modified_len;

}

int measurer::max_length() const {

  assert(computed);

  return std::max(original_len, modified_len);

}

int measurer::min_length() const {

  assert(computed);

  return std::min(original_len, modified_len);

}

void measurer::process_edit_script(const ses::edit_list& edits) {

    a_similarity = 0;
    a_original_difference = 0;
    a_modified_difference = 0;
 
    for(const struct ses::edit& edit : edits) {

      switch(edit.operation) {

        case ses::CHANGE:
          a_original_difference += edit.original_length;
          a_modified_difference += edit.modified_length;
          break;

        case ses::DELETE :
          a_original_difference += edit.original_length;
          break;

        case ses::INSERT :
          a_modified_difference += edit.modified_length;
          break;

        default:
          break;
        }

    }

    int delete_similarity = original_len - a_original_difference;
    int insert_similarity = modified_len - a_modified_difference;

    a_similarity = std::min(delete_similarity, insert_similarity);

    if(a_similarity <= 0) {
      a_similarity = 0;
    }

}

}
