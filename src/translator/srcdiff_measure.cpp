// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_measure.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_measure.hpp>

#include <srcdiff_constants.hpp>
#include <construct.hpp>

#include <algorithm>
#include <cassert>

srcdiff_measure::srcdiff_measure(const construct & set_original,
                                 const construct & set_modified) 
  : set_original(set_original),
    set_modified(set_modified),
    computed(false),
    a_similarity(0),
    a_original_difference(0),
    a_modified_difference(0),
    original_len(0),
    modified_len(0) {}

int srcdiff_measure::similarity() const {

  assert(computed);

  return a_similarity;

}

int srcdiff_measure::difference() const {

  assert(computed);
  if(a_original_difference == MAX_INT) return MAX_INT;

  return a_original_difference + a_modified_difference;

}

int srcdiff_measure::original_difference() const {

  assert(computed);

  return a_original_difference;

}

int srcdiff_measure::modified_difference() const {

  assert(computed);

  return a_modified_difference;

}

int srcdiff_measure::original_length() const {

  assert(computed);

  return original_len;

}

int srcdiff_measure::modified_length() const {

  assert(computed);

  return modified_len;

}

int srcdiff_measure::max_length() const {

  assert(computed);

  return std::max(original_len, modified_len);

}

int srcdiff_measure::min_length() const {

  assert(computed);

  return std::min(original_len, modified_len);

}

void srcdiff_measure::process_edit_script(const edit_t * edit_script) {

    a_similarity = 0;
    a_original_difference = 0;
    a_modified_difference = 0;
 
    for(const edit_t * edits = edit_script; edits; edits = edits->next) {

      switch(edits->operation) {

        case SES_DELETE :

          a_original_difference += edits->length;
          break;

        case SES_INSERT :

          a_modified_difference += edits->length;
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
