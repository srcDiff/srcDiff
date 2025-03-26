// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_match.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <change_matcher.hpp>

#include <nest_differ.hpp>
#include <text_measurer.hpp>
#include <syntax_measurer.hpp>
#include <constants.hpp>

#include <list>

#include <cstring>

namespace srcdiff {

bool is_match(construct::construct_list_view original, construct::construct_list_view modified) {
  const srcdiff::measurer & measure = *original[0]->measure(*modified[0]);

  if(measure.similarity() == MAX_INT) return false;

  if(!original[0]->is_match_similar(*modified[0]) && !original[0]->can_refine_difference(*modified[0]))
    return false;

  if(nest_differ::is_better_nested(original, modified))
    return false;

  return true;

}

change_matcher::change_matcher(const construct::construct_list_view original, const construct::construct_list_view modified)
  : original(original), modified(modified) {}

static  construct::construct_list_view safe_subspan(construct::construct_list_view view, std::size_t start, std::size_t end) {
  if(start < 0 || start > end || end > view.size()) return construct::construct_list_view();
  return view.subspan(start, end - start + 1);
}

/** loop O(D) */
change_list change_matcher::create_linked_list(difference * differences) {

  // create match linked list
  change_list changes;

  int olength = original.size();
  int nlength = modified.size();

  std::vector<bool> olist(olength);
  std::vector<bool> nlist(nlength);

  int original_pos = olength - 1;
  int modified_pos = nlength - 1;

  for(int i = nlength - 1, j = olength - 1; i >= 0 || j >= 0;) {

    difference& diff = differences[i * olength + j];

    // only output marked and if has not already been output
    if(diff.marked && !(olist[j] || nlist[i])) {

      if(original_pos - diff.opos > 0 ||  modified_pos - diff.npos > 0) {
        changes.emplace_front(safe_subspan(original, diff.opos + 1, original_pos),
                              safe_subspan(modified, diff.npos + 1, modified_pos),
                              0, CHANGE, original_pos, modified_pos);
      }

      changes.emplace_front(original.subspan(diff.opos, 1), modified.subspan(diff.npos, 1),
                            diff.similarity, COMMON, diff.opos, diff.npos);

      olist[j] = true;
      nlist[i] = true;

      original_pos = diff.opos - 1;
      modified_pos = diff.npos - 1;

    }

    switch(diff.direction) {

    case 0:

      --i;
      --j;

      break;

    case 1:

      --j;

      break;

    case 2:

      --i;

      break;

    case 3:

      --i;
      --j;

      break;

    default:

      break;

    }

  }

  if(original_pos >= 0 || modified_pos >= 0) {
    changes.emplace_front(safe_subspan(original, 0, original_pos),
                          safe_subspan(modified, 0, modified_pos),
                          0, CHANGE, original_pos, modified_pos);
  }

  return changes;

}

/** loop O(RD^2) */
 change_list change_matcher::match_differences() {

  /*

    Possible dynamic programming solution.  Sum similarities choosing left top or diagonal,
    picking smallest.  Unmatching has a high cost 1 less than a syntax mismatch.  So, it is chosen over
    a syntax mismatch.  Not sure yet, but left and diagonal probably add cost, and top might be a straight copy
    or a copy plus a unmatch.

    Errata: Now minimizing unmatched then minimizing similarity

    Errata: Now maximizing similarity

  */

  int olength = original.size();
  int nlength = modified.size();

  size_t mem_size = olength * nlength * sizeof(difference);

  difference * differences = (difference *)malloc(mem_size);

  // still need to figure out how to track matching on each path
  for(int i = 0; i < nlength; ++i) {

    for(int j = 0; j < olength; ++j) {

      /** loop O(nd) */
      const srcdiff::measurer & measure = *original[j]->measure(*modified[i]);
      int similarity = measure.similarity();

      //unsigned long long max_similarity = (unsigned long long)-1;
      int max_similarity = -1;
      int unmatched = 0;

      // check if unmatched
      /** loop text O(nd) + syntax O(nd) + best match is O(nd) times number of matches */
      construct::construct_list_view original_view = original.subspan(j, original.size() - j);
      construct::construct_list_view modified_view = modified.subspan(i, modified.size() - i);
      if(!is_match(original_view, modified_view) || original[j]->term(0)->get_move() || modified[i]->term(0)->get_move()) {

        similarity = 0;
        unmatched = 2;

      }

      int num_unmatched = MAX_INT;
      int direction = 0;

      bool matched = false;

      // check along x axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(j > 0) {

        max_similarity = differences[i * olength + (j - 1)].similarity;
        num_unmatched = differences[i * olength + (j - 1)].num_unmatched + 1;

        matched = false;

        // may be wrong
        int temp_num_unmatched = i + j + unmatched;

        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && similarity > max_similarity)) {

          matched = !unmatched;

          max_similarity = similarity;
          num_unmatched = temp_num_unmatched;

        }

        direction = 1;

      }

      // check along y axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(i > 0) {

        // may not have been initialized in j > 0
        if(direction == 0) {
          direction = 2;
        }

        int temp_similarity = differences[(i - 1) * olength + j].similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + j].num_unmatched + 1;

        // may be wrong
        int temp_num_unmatched_match = i + j + unmatched;

        int temp_matched = false;

        if(temp_num_unmatched_match < temp_num_unmatched || (temp_num_unmatched_match == temp_num_unmatched && similarity > temp_similarity)) {

          temp_matched = !unmatched;

          temp_similarity = similarity;
          temp_num_unmatched = temp_num_unmatched_match;

        }

        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity > max_similarity)) {

          matched = temp_matched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;

          direction = 2;

        }

      }

      // go along diagonal just add similarity and unmatched
      if(i > 0 && j > 0) {

        int temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + (j - 1)].num_unmatched + unmatched;

        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity > max_similarity)) {

          matched = !unmatched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;
          direction = 3;

        }

      }

      // special case starting node
      if(i == 0 && j == 0) {

        max_similarity = similarity;
        num_unmatched = unmatched;
        matched = !unmatched;

      }

      // update structure
      difference& diff = differences[i * olength + j];

      diff.marked = matched;
      diff.similarity = max_similarity;
      diff.num_unmatched = num_unmatched;
      diff.opos = j;
      diff.npos = i;
      diff.direction = direction;

    }

  }

  // create match linked list
  change_list changes = create_linked_list(differences);

  // free memory
  free(differences);

  return changes;

}

}
