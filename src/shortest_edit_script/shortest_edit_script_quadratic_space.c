// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script_quadratic_space.c
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <shortest_edit_script.h>
#include <shortest_edit_script_private.h>

#include <stdlib.h>
#include <string.h>

/*
  Finds the shortest edit script between two sequences.
   
  Parameter sequence_one_end    The size of the first sequence
  Parameter sequence_one         The first sequence
  Parameter sequence_two_end    The size of the second sequence
  Parameter sequence_two         The second sequence
  Parameter edit_script          The shortest edit script

  Returns Then number of edits or an error code (-1 malloc, -2 otherwise) 
*/
int shortest_edit_script_inner(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end,
  struct edit_t ** edit_script, struct edit_t ** last_edit,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {

  // max edit distance
  int max_distance = (sequence_one_end - sequence_one_start) + (sequence_two_end - sequence_two_start) + 1;

  int max_diagonals = 2 * max_distance + 1;

  // last row with edit distance for each diagonal
  struct point * last_distance = (struct point *)calloc(max_diagonals, sizeof(struct point));

  // hold all allocates
  struct edit_t ** edit_pointers = (struct edit_t **)calloc(max_diagonals, sizeof(struct edit_t *));

  int num_edits = -1;

  // internal script of edits
  struct edit_t ** script = (struct edit_t **)calloc(max_diagonals, sizeof(struct edit_t *));
  if(script == NULL) {

    (*edit_script) = NULL;
    return -2;

  }

  // initialization, slide 0 along 0 diagonal and find 1st edit
  int column = sequence_one_start;
  int row = sequence_two_start;
  for(; column < sequence_one_end && row < sequence_two_end && compare(accessor(column, sequence_one, context), accessor(row, sequence_two, context), context) == 0; ++column, ++row)
    ;

  // center to start building differences
  int center = max_distance + 1;

  // set 0 diagonal's position and set beginning of script
  last_distance[center].x = column;
  last_distance[center].y = row;
  script[center] = NULL;

  // identical files
  if(column >= sequence_one_end && row >= sequence_two_end) {
    
    if(edit_script) (*edit_script) = NULL;;
    if(last_edit) (*last_edit) = NULL;;

    return 0;

  }

  // for each edit distance
  int distance;
  for(distance = 1; distance < max_distance; ++distance) {

    // for each possible diagonal
    int diagonal;
    for(diagonal = -distance; diagonal <= distance; diagonal += 2) {
      // locate next edit
      ++num_edits;
      int edit_array = num_edits / (max_distance + 1);
      int edit = num_edits % (max_distance + 1);

      if(edit == 0) {
        if((edit_pointers[edit_array] = (struct edit_t *)calloc(max_distance + 1, sizeof(struct edit_t))) == NULL) {

          // clean allocates
          int i;
          for(i = 0; i < edit_array; ++i)
            free(edit_pointers[i]);

          // no script on error
          (*edit_script) = NULL;
          free(script);

          return -1;

        }
      }

      int diagonal_pos = diagonal + center;

      // move down if no right distance or has farthest down the diagonal
      if(diagonal == -distance || (diagonal != distance && (last_distance[diagonal_pos - 1].x < last_distance[diagonal_pos + 1].x))) {

        // move down (set delete operation) and append edit
        column = last_distance[diagonal_pos + 1].x;
        row= last_distance[diagonal_pos + 1].y + 1;
        edit_pointers[edit_array][edit].operation = SES_INSERT;
        edit_pointers[edit_array][edit].previous = script[diagonal_pos + 1];

      } else {

        // move right (set insert operation) and append edit
        column = last_distance[diagonal_pos - 1].x + 1;
        row = last_distance[diagonal_pos - 1].y;
        edit_pointers[edit_array][edit].operation = SES_DELETE;
        edit_pointers[edit_array][edit].previous = script[diagonal_pos - 1];

      }

      edit_pointers[edit_array][edit].offset_one = column;
      edit_pointers[edit_array][edit].offset_two = row;

      // update the script
      script[diagonal_pos] = &edit_pointers[edit_array][edit];

      // slide down the diagonal
      while(column < sequence_one_end && row < sequence_two_end && compare(accessor(column, sequence_one, context), accessor(row, sequence_two, context), context) == 0) {

        ++column;
        ++row;

      }

      // update diagonal's last distance with position
      last_distance[diagonal_pos].x = column;
      last_distance[diagonal_pos].y = row;

      // reached lower right (finished)
      if(column == sequence_one_end && row == sequence_two_end) {

        // make shortest edit script
        int edit_distance = make_edit_script(&edit_pointers[edit_array][edit], edit_script, last_edit);

        free(last_distance);

        // clean allocates
        int i;
        for(i = 0; i <= edit_array; ++i) {
          free(edit_pointers[i]);
        }

        free(edit_pointers);

        free(script);

        return edit_distance;
      }

    }

  }

  free(last_distance);

  int edit_array = num_edits / (max_distance + 1);

  // clean allocates
  int i;
  for(i = 0; i <= edit_array; ++i) {
    free(edit_pointers[i]);
  }

  free(edit_pointers);

  free(script);

  // no edit script on error
  (*edit_script) = NULL;
  (*last_edit) = NULL;

  return -2;
}

int shortest_edit_script(const void * sequence_one, int sequence_one_end, const void * sequence_two, int sequence_two_end,
  struct edit_t ** edit_script, 
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {

  return shortest_edit_script_inner(sequence_one, 0, sequence_one_end, sequence_two, 0, sequence_two_end, edit_script, 0, compare, accessor, context);

}
