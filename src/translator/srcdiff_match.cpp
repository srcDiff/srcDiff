// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_match.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_match.hpp>

#include <srcdiff_nested.hpp>
#include <srcdiff_text_measure.hpp>
#include <srcdiff_syntax_measure.hpp>
#include <srcdiff_shortest_edit_script.hpp>
#include <srcdiff_constants.hpp>

#include <list>

#include <cstring>

struct difference {

  //unsigned long long similarity;
  int similarity;
  int num_unmatched;
  bool marked;
  int direction;
  unsigned int opos;
  unsigned int npos;

};

bool is_match(construct::construct_list_view original, construct::construct_list_view modified) {
  const srcdiff_measure & measure = *original[0]->measure(*modified[0]);

  if(measure.similarity() == MAX_INT) return false;

  if(!original[0]->is_match_similar(*modified[0]) && !original[0]->can_refine_difference(*modified[0]))
    return false;

  if(srcdiff_nested::is_better_nested(original, modified))
    return false;

  return true;

}


srcdiff_match::srcdiff_match(const construct::construct_list_view original, const construct::construct_list_view modified)
  : original(original), modified(modified) {}

/** loop O(D) */
static offset_pair * create_linked_list(int olength, int nlength, difference * differences) {

  // create match linked list
  offset_pair * last_match = NULL;

  bool * olist = (bool *)malloc(olength * sizeof(bool));
  memset(olist, 0, olength * sizeof(bool));

  bool * nlist = (bool *)malloc(nlength * sizeof(bool));
  memset(nlist, 0, nlength * sizeof(bool));

  for(int i = nlength - 1, j = olength - 1; i >= 0 || j >= 0;) {

    // only output marked and if has not already been output
    if(differences[i * olength + j].marked && !(olist[j] || nlist[i])) {

      offset_pair * match = new offset_pair;

      match->original_offset = differences[i * olength + j].opos;
      match->modified_offset = differences[i * olength + j].npos;
      match->similarity = differences[i * olength + j].similarity;
      match->next = last_match;

      last_match = match;

      olist[j] = true;
      nlist[i] = true;

    }

    switch(differences[i * olength + j].direction) {

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

  free(olist);
  free(nlist);

  return last_match;

}

/** loop O(RD^2) */
offset_pair * srcdiff_match::match_differences() {

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
      const srcdiff_measure & measure = *original[j]->measure(*modified[i]);
      int similarity = measure.similarity();

      //unsigned long long max_similarity = (unsigned long long)-1;
      int max_similarity = -1;
      int unmatched = 0;

      // check if unmatched
      /** loop text O(nd) + syntax O(nd) + best match is O(nd) times number of matches */
      construct::construct_list_view original_view = original.subspan(j, original.size() - j);
      construct::construct_list_view modified_view = modified.subspan(i, modified.size() - i);
      if(!is_match(original_view, modified_view)) {

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
      differences[i * olength + j].marked = matched;
      differences[i * olength + j].similarity = max_similarity;
      differences[i * olength + j].num_unmatched = num_unmatched;
      differences[i * olength + j].opos = j;
      differences[i * olength + j].npos = i;
      differences[i * olength + j].direction = direction;

    }

  }

  // create match linked list
  offset_pair * matches = create_linked_list(olength, nlength, differences);

  // free memory
  free(differences);

  return matches;

}
