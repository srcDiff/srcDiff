/*
  shortest_edit_sequence.c

  Compute the shortest edit script between two sequences.

  Miller and Myers Shortest Edit Script Algorithm.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <shortest_edit_script.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*
  Finds the shortest edit script between two sequences.
   
  Parameter sequence_one_size    The size of the first sequence
  Parameter sequence_one         The first sequence
  Parameter sequence_two_size    The size of the second sequence
  Parameter sequence_two         The second sequence
  Parameter edit_script          The shortest edit script

  Returns Then number of edits or an error code (-1 malloc, -2 otherwise) 
*/
int shortest_edit_script_linear_space(int sequence_one_size, const void * sequence_one, int sequence_two_size, const void * sequence_two,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), struct edit ** edit_script, const void * context) {


  // center to start building differences
  int center = sequence_one_size;

  // max edit distance
  int max_distance = sequence_one_size + sequence_two_size;

  // last row with edit distance for each diagonal
  int last_distance[max_distance + 1];

  // hold all allocates
  struct edit * edit_pointers[sequence_two_size + 1];

  int num_edits = -1;

  // internal script of edits
  //struct edit * script[max_distance + 1];
  struct edit ** script = (struct edit **)calloc(max_distance + 1, sizeof(struct edit *));
  if(script == NULL) {

    (*edit_script) = NULL;
    return -2;

  }

  // initialization, slide 0 along 0 diagonal and find 1st edit
  int row = 0;
  int column = 0;
  for(; row < sequence_one_size && row < sequence_two_size && compare(accessor(row, sequence_one, context), accessor(row, sequence_two, context), context) == 0; ++row)
    ;

  // set 0 diagonal's row of distance and set beginning of script
  last_distance[center] = row;
  script[center] = NULL;

  // set starting diagonal bounds
  int lower_bound;
  if(row == sequence_one_size)
    lower_bound = center + 1;
  else
    lower_bound = center - 1;

  int upper_bound;
  if(row == sequence_two_size)
    upper_bound = center - 1;
  else
    upper_bound = center + 1;

  // the files are identical
  if(lower_bound > upper_bound) {
    
    (*edit_script) = NULL;
    return 0;
  }

  // for each edit distance
  int distance;
  for(distance = 1; distance <= max_distance; ++distance) {

    // for each possible diagonal
    int diagonal;
    for(diagonal = lower_bound; diagonal <= upper_bound; diagonal += 2) {

      // locate next edit
      ++num_edits;
      int edit_array = num_edits / (sequence_one_size + 1);
      int edit = num_edits % (sequence_one_size + 1);

      if(edit == 0)
        if((edit_pointers[edit_array] = (struct edit *)calloc(sequence_one_size + 1, sizeof(struct edit))) == NULL) {

          // clean allocates
          int i;
          for(i = 0; i < edit_array; ++i)
            free(edit_pointers[i]);

          // no script on error
          (*edit_script) = NULL;
          free(script);

          return -1;
        }
        
      //struct edit temp_edit;
      //edit_pointers[edit_array][edit] = temp_edit;


      // move down if no right distance or has farthest down the diagonal
      if(diagonal == (center - distance) || (diagonal != (center + distance) && (last_distance[diagonal + 1] >= last_distance[diagonal - 1]))) {
        // move down (set delete operation) and append edit
        row = last_distance[diagonal + 1] + 1;
        edit_pointers[edit_array][edit].operation = SESDELETE;
        edit_pointers[edit_array][edit].previous = script[diagonal + 1];
      } else {

        // move right (set insert operation) and append edit
        row = last_distance[diagonal - 1];
        edit_pointers[edit_array][edit].operation = SESINSERT;
        edit_pointers[edit_array][edit].previous = script[diagonal - 1];
      }

      // calculate column
      column = row + (diagonal - center);

      edit_pointers[edit_array][edit].offset_sequence_one = row;
      edit_pointers[edit_array][edit].offset_sequence_two = column;

      // update the script
      script[diagonal] = &edit_pointers[edit_array][edit];

      // slide down the diagonal
      while(row < sequence_one_size && column < sequence_two_size && compare(accessor(row, sequence_one, context), accessor(column, sequence_two, context), context) == 0) {

        ++row;
        ++column;
      }

      // update diagonal's last distance with row
      last_distance[diagonal] = row;

      // reached lower right (finished)
      if(row == sequence_one_size && column == sequence_two_size) {

        // make shortest edit script
        int edit_distance = make_edit_script(&edit_pointers[edit_array][edit], edit_script);

        // clean allocates
        int i;
        for(i = 0; i <= edit_array; ++i)
          free(edit_pointers[i]);

        free(script);

        return edit_distance;
      }

      // reached bottom do not go farther down next iteration (decrement will set to only check diagonal + 1)
      if(row == sequence_one_size)
        lower_bound = diagonal + 2;

      // reached right edge do not go farther down right iteration (increment will set to only check diagonal - 1)
      if(column == sequence_two_size)
        upper_bound = diagonal - 2;

    }

    // set diagonal bounds for next iteration
    --lower_bound;
    ++upper_bound;

  }

  // no edit script on error
  (*edit_script) = NULL;

  return -2;
}

