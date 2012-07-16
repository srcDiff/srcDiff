#include "srcDiffMatch.hpp"
#include "srcDiffMeasure.hpp"
#include <string.h>


void create_linked_list(int olength, int nlength, difference * differences, offset_pair ** matches) {

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

      match->old_offset = differences[i * olength + j].opos;
      match->new_offset = differences[i * olength + j].npos;
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

  *matches = last_match;

  free(olist);
  free(nlist);

}

void match_differences_dynamic(std::vector<xNodePtr> & nodes_old, NodeSets * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, NodeSets * node_sets_new
                               , offset_pair ** matches) {

  /*

    Possible dynamic programming solution.  Sum similarities choosing left top or diagonal,
    picking smallest.  Unmatching has a high cost 1 less than a syntax mismatch.  So, it is chosen over
    a syntax mismatch.  Not sure yet, but left and diagonal probably add cost, and top might be a straight copy
    or a copy plus a unmatch.

    Errata: Now minimizing unmatched then minimizing similarity

    Errata: Now maximizing similarity

  */

  /*
    fprintf(stderr, "HERE\n");

    for(int old_pos = 0; old_pos < edits->length; ++old_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_old.at(node_sets_old->at(edits->offset_sequence_one + old_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");

    for(int new_pos = 0; new_pos < edit_next->length; ++new_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two + new_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");
*/

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  *matches = 0;

  int olength = node_sets_old->size();
  int nlength = node_sets_new->size();

  size_t mem_size = olength * nlength * sizeof(difference);

  difference * differences = (difference *)malloc(mem_size);

  // still need to figure out how to track matching on each path
  for(int i = 0; i < nlength; ++i) {

    for(int j = 0; j < olength; ++j) {

      int similarity = compute_similarity(nodes_old, node_sets_old->at(j)
                                          , nodes_new, node_sets_new->at(i));

      //unsigned long long max_similarity = (unsigned long long)-1;
      int max_similarity = -1;
      int unmatched = 0;

      // check if unmatched
      if(similarity == MAX_INT) {

        similarity = 0;
        unmatched = 1;

      }

      int num_unmatched = MAX_INT;
      int direction = 0;

      bool matched = false;

      // check along x axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(j > 0) {

        //max_similarity = differences[i * olength + (j - 1)].similarity + MAX_INT;
        max_similarity = differences[i * olength + (j - 1)].similarity;
        num_unmatched = differences[i * olength + (j - 1)].num_unmatched + 1;

        matched = false;

        // may be wrong
        int temp_num_unmatched = i + j + (unmatched ? 2 : 0);

        //unsigned long long temp_similarity = MAX_INT * num_unmatched + similarity;

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && similarity > max_similarity)) {

          matched = !unmatched;

          //max_similarity = temp_similarity;
          max_similarity = similarity;
          num_unmatched = temp_num_unmatched;

        }

        direction = 1;

      }

      // check along y axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(i > 0) {

        // may not have been initialized in j > 0
        if(direction == 0)
          direction = 2;

        //unsigned long long temp_similarity = differences[(i - 1) * olength + j].similarity + MAX_INT;
        int temp_similarity = differences[(i - 1) * olength + j].similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + j].num_unmatched + 1;

        // may be wrong
        int temp_num_unmatched_match = i + j + (unmatched ? 2 : 0);


        int temp_matched = false;

        //unsigned long long temp_similarity_match = MAX_INT * num_unmatched + similarity;

        //if(temp_similarity_match < temp_similarity) {
        if(temp_num_unmatched_match < temp_num_unmatched || (temp_num_unmatched_match == temp_num_unmatched && similarity > temp_similarity)) {

          temp_matched = !unmatched;

          temp_similarity = similarity;
          temp_num_unmatched = temp_num_unmatched_match;

        }

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity > max_similarity)) {

          matched = temp_matched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;

          direction = 2;

        }

      }

      // go along diagonal just add similarity and unmatched
      if(i > 0 && j > 0) {

        //unsigned long long temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + (j - 1)].num_unmatched + (unmatched ? 2 : 0);

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity > max_similarity)) {

          matched = !unmatched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;
          direction = 3;

        }

      }

      if(direction == 1) {
        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        //differences[i * olength + (j - 1)].marked = marked_left;
        //differences[i * olength + (j - 1)].last_similarity = last_similarity_left;

      } else if(direction == 2) {
        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        //differences[(i - 1) * olength + j].marked = marked_top;
        //differences[(i - 1) * olength + j].last_similarity = last_similarity_top;

      } else {

        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

      }

      // special case starting node
      if(i == 0 && j == 0) {

        max_similarity = similarity;
        num_unmatched = unmatched;
        if(unmatched)
          ++num_unmatched;

        matched = !unmatched;
      }

      // set if marked
      if(matched) {

        differences[i * olength + j].marked = true;

      } else {

        differences[i * olength + j].marked = false;

      }

      /*
      fprintf(stderr, "HERE\n");
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, matched);
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, num_unmatched);
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, max_similarity);
      fprintf(stderr, "HERE\n");
      */

      // update structure
      differences[i * olength + j].similarity = max_similarity;
      differences[i * olength + j].num_unmatched = num_unmatched;
      differences[i * olength + j].opos = j;
      differences[i * olength + j].npos = i;
      differences[i * olength + j].direction = direction;

    }

  }

  // create match linked list
  create_linked_list(olength, nlength, differences, matches);

  // free memory
  free(differences);

}

void match_differences_dynamic(std::vector<xNodePtr> & nodes_old, NodeSets * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, NodeSets * node_sets_new
                               , edit * edit_script, offset_pair ** matches) {

  /*

    Possible dynamic programming solution.  Sum similarities choosing left top or diagonal,
    picking smallest.  Unmatching has a high cost 1 less than a syntax mismatch.  So, it is chosen over
    a syntax mismatch.  Not sure yet, but left and diagonal probably add cost, and top might be a straight copy
    or a copy plus a unmatch.

    Errata: Now minimizing unmatched then minimizing similarity

    Errata: Now maximizing similarity

  */

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  /*
    fprintf(stderr, "HERE\n");

    for(int old_pos = 0; old_pos < edits->length; ++old_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_old.at(node_sets_old->at(edits->offset_sequence_one + old_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");

    for(int new_pos = 0; new_pos < edit_next->length; ++new_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two + new_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");
*/

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  *matches = 0;

  int olength = edits->length;
  int nlength = edit_next->length;

  size_t mem_size = olength * nlength * sizeof(difference);

  difference * differences = (difference *)malloc(mem_size);

  // still need to figure out how to track matching on each path
  for(int i = 0; i < nlength; ++i) {

    for(int j = 0; j < olength; ++j) {

      int similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + j)
                                          , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + i));

      //unsigned long long max_similarity = (unsigned long long)-1;
      int max_similarity = -1;
      int unmatched = 0;

      // check if unmatched
      if(similarity == MAX_INT) {

        similarity = 0;
        unmatched = 1;

      }

      int num_unmatched = MAX_INT;
      int direction = 0;

      bool matched = false;

      // check along x axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(j > 0) {

        //max_similarity = differences[i * olength + (j - 1)].similarity + MAX_INT;
        max_similarity = differences[i * olength + (j - 1)].similarity;
        num_unmatched = differences[i * olength + (j - 1)].num_unmatched + 1;

        matched = false;

        // may be wrong
        int temp_num_unmatched = i + j + unmatched;
        if(unmatched)
          ++temp_num_unmatched;

        //unsigned long long temp_similarity = MAX_INT * num_unmatched + similarity;

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && similarity > max_similarity)) {

          matched = true;

          //max_similarity = temp_similarity;
          max_similarity = similarity;
          num_unmatched = temp_num_unmatched;

        }

        direction = 1;

      }

      // check along y axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(i > 0) {

        // may not have been initialized in j > 0
        if(direction == 0)
          direction = 2;

        //unsigned long long temp_similarity = differences[(i - 1) * olength + j].similarity + MAX_INT;
        int temp_similarity = differences[(i - 1) * olength + j].similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + j].num_unmatched + 1;

        // may be wrong
        int temp_num_unmatched_match = i + j + unmatched;
        if(unmatched)
          ++temp_num_unmatched_match;

        int temp_matched = false;

        //unsigned long long temp_similarity_match = MAX_INT * num_unmatched + similarity;

        //if(temp_similarity_match < temp_similarity) {
        if(temp_num_unmatched_match < temp_num_unmatched || (temp_num_unmatched_match == temp_num_unmatched && similarity > temp_similarity)) {

          temp_matched = true;

          temp_similarity = similarity;
          temp_num_unmatched = temp_num_unmatched_match;

        }

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && similarity >= max_similarity)) {

          matched = temp_matched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;

          direction = 2;

        }

      }

      // go along diagonal just add similarity and unmatched
      if(i > 0 && j > 0) {

        //unsigned long long temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + (j - 1)].num_unmatched + unmatched;
        if(unmatched)
          ++temp_num_unmatched;

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity > max_similarity)) {

          matched = !unmatched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;
          direction = 3;

        }

      }

      if(direction == 1) {
        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        //differences[i * olength + (j - 1)].marked = marked_left;
        //differences[i * olength + (j - 1)].last_similarity = last_similarity_left;

      } else if(direction == 2) {
        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        //differences[(i - 1) * olength + j].marked = marked_top;
        //differences[(i - 1) * olength + j].last_similarity = last_similarity_top;

      } else {

        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

      }

      // special case starting node
      if(i == 0 && j == 0) {

        max_similarity = similarity;
        num_unmatched = unmatched;
        if(unmatched)
          ++num_unmatched;

        matched = !unmatched;
      }

      // set if marked
      if(matched) {

        differences[i * olength + j].marked = true;

      } else {

        differences[i * olength + j].marked = false;

      }

      /*
      fprintf(stderr, "HERE\n");
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, matched);
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, num_unmatched);
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, max_similarity);
      fprintf(stderr, "HERE\n");
      */

      // update structure
      differences[i * olength + j].similarity = max_similarity;
      differences[i * olength + j].num_unmatched = num_unmatched;
      differences[i * olength + j].opos = j;
      differences[i * olength + j].npos = i;
      differences[i * olength + j].direction = direction;

    }

  }

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

      match->old_offset = differences[i * olength + j].opos;
      match->new_offset = differences[i * olength + j].npos;
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

  *matches = last_match;

  free(olist);
  free(nlist);

  // free memory
  free(differences);

}

void match_differences_dynamic_unordered(std::vector<xNodePtr> & nodes_old, NodeSets * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, NodeSets * node_sets_new
                               , edit * edit_script, offset_pair ** matches) {

  /*

    Possible dynamic programming solution.  Sum similarities choosing left top or diagonal,
    picking smallest.  Unmatching has a high cost 1 less than a syntax mismatch.  So, it is chosen over
    a syntax mismatch.  Not sure yet, but left and diagonal probably add cost, and top might be a straight copy
    or a copy plus a unmatch.

    Errata: Now minimizing unmatched then minimizing similarity

    Errata: Now maximizing similarity

  */

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  /*
    fprintf(stderr, "HERE\n");

    for(int old_pos = 0; old_pos < edits->length; ++old_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_old.at(node_sets_old->at(edits->offset_sequence_one + old_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");

    for(int new_pos = 0; new_pos < edit_next->length; ++new_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two + new_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");
*/

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  *matches = 0;

  int olength = edits->length;
  int nlength = edit_next->length;

  size_t mem_size = olength * nlength * sizeof(difference);

  difference * differences = (difference *)malloc(mem_size);

  // still need to figure out how to track matching on each path
  for(int i = 0; i < nlength; ++i) {

    for(int j = 0; j < olength; ++j) {

      int similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + j)
                                          , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + i));
      // update structure
      differences[i * olength + j].similarity = similarity;
      differences[i * olength + j].opos = j;
      differences[i * olength + j].npos = i;

    }

  }

  std::vector<int> pos;

  for(int i = 0; i < nlength; ++i) {

    int max_similarity = -1;

    int max_pos = -1;

    for(int j = 0; j < olength; ++j) {

      if(differences[i * olength + j].similarity > max_similarity) {

        max_similarity = differences[i * olength + j].similarity;
        max_pos = j;

      }

    }

    pos.push_back(max_pos);

 }

  // create match linked list
  offset_pair * last_match = NULL;

  bool * olist = (bool *)malloc(olength * sizeof(bool));
  memset(olist, 0, olength * sizeof(bool));

  bool * nlist = (bool *)malloc(nlength * sizeof(bool));
  memset(nlist, 0, nlength * sizeof(bool));

  for(int i = 0; i < nlength; ++i) {

    // only output marked and if has not already been output
    if(!(olist[pos.at(i)] || nlist[i])) {

      offset_pair * match = new offset_pair;

      match->old_offset = pos.at(i);
      match->new_offset = i;
      //match->similarity = differences[i * olength + j].similarity;
      match->next = last_match;

      last_match = match;

      olist[pos.at(i)] = true;
      nlist[i] = true;

    }

  }

  *matches = last_match;

  free(olist);
  free(nlist);

  // free memory
  free(differences);

}

void match_differences(std::vector<xNodePtr> & nodes_old, NodeSets * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, NodeSets * node_sets_new
                       , edit * edit_script, offset_pair ** matches) {


  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  *matches = 0;

  struct offset_pair * curmatch = 0;

  if(edits->length > edit_next->length) {
    for(int old_pos = 0, new_pos = 0; old_pos < edits->length && new_pos < edit_next->length; ++old_pos, ++new_pos) {

      int min_similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + old_pos)
                                              , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + new_pos));
      int save_pos = old_pos;
      for(int pos = old_pos + 1; pos < edits->length; ++pos) {

        int similarity = 0;
        if((similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + pos)
                                            , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + new_pos))) < min_similarity) {

          old_pos = pos;
          min_similarity = similarity;
        }

      }

      int olength = node_sets_old->at(edits->offset_sequence_one + old_pos)->size();
      int nlength = node_sets_new->at(edit_next->offset_sequence_two + new_pos)->size();

      int max_size = 2 * olength;
      if(nlength > olength)
        max_size = 2 * nlength;

      if(min_similarity < max_size) {

        offset_pair * match = new offset_pair;
        match->old_offset = old_pos;
        match->new_offset = new_pos;
        match->similarity = min_similarity;
        match->next = NULL;

        if(*matches == 0) {

          *matches = match;
          curmatch = match;

        } else {

          curmatch->next = match;
          curmatch = curmatch->next;

        }
      } else {

        old_pos = save_pos - 1;

      }
    }

  } else {

    for(int old_pos = 0, new_pos = 0; old_pos < edits->length && new_pos < edit_next->length; ++old_pos, ++new_pos) {

      int min_similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + old_pos)
                                              , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + new_pos));

      int save_pos = new_pos;
      for(int pos = new_pos + 1; pos < edit_next->length; ++pos) {

        int similarity = 0;
        if((similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + old_pos)
                                            , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + pos))) < min_similarity) {

          new_pos = pos;
          min_similarity = similarity;
        }

      }

      int olength = node_sets_old->at(edits->offset_sequence_one + old_pos)->size();
      int nlength = node_sets_new->at(edit_next->offset_sequence_two + new_pos)->size();

      int max_size = 2 * olength;
      if(nlength > olength)
        max_size = 2 * nlength;

      if(min_similarity < max_size) {

        offset_pair * match = new offset_pair;
        match->old_offset = old_pos;
        match->new_offset = new_pos;
        match->similarity = min_similarity;
        match->next = NULL;

        if(*matches == 0) {

          *matches = match;
          curmatch = match;

        } else {

          curmatch->next = match;
          curmatch = curmatch->next;

        }
      } else {

        new_pos = save_pos - 1;

      }
    }

  }

}
