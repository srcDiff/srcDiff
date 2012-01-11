/*
  shortest_edit_sequence.c

  Compute the shortest edit script between two sequences.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include "shortest_edit_script.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
  Make a compact edit script from the found edits.

  Parameter last_edit            The last edit found
  Parameter edit_script          The shortest edit script

  Returns -1 on fail, 0 otherwise
*/
int make_edit_script(struct edit * last_edit, struct edit ** edit_script);

/*
  Copy a node from the heap.

  Parameter edit          Edit to copy

  Returns The copied edit or NULL if failed
*/
struct edit * copy_edit(struct edit * edit);

/*
  Finds the shortest edit script between two sequences.
   
  Parameter sequence_one_size    The size of the first sequence
  Parameter sequence_one         The first sequence
  Parameter sequence_two_size    The size of the second sequence
  Parameter sequence_two         The second sequence
  Parameter edit_script          The shortest edit script

  Returns Then number of edits or an error code (-1 malloc, -2 otherwise) 
*/
int shortest_edit_script(int sequence_one_size, const void * sequence_one, int sequence_two_size, const void * sequence_two, int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), struct edit ** edit_script, void * context) {


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
  struct edit * script[max_distance + 1];

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
        if((edit_pointers[edit_array] = (struct edit *)malloc(sizeof(struct edit) * (sequence_one_size + 1))) == NULL) {
          memset(edit_pointers[edit_array], 0, sizeof(struct edit) * (sequence_one_size + 1));
          
          // clean allocates
          int i;
          for(i = 0; i < edit_array; ++i)
            free(edit_pointers[i]);

          // no script on error
          (*edit_script) = NULL;

          return -1;
        }
        
      //struct edit temp_edit;
      //edit_pointers[edit_array][edit] = temp_edit;


      // move down if no right distance or has farthest down the diagonal
      if(diagonal == (center - distance) || (diagonal != (center + distance) && (last_distance[diagonal + 1] >= last_distance[diagonal - 1]))) {
        // move down (set delete operation) and append edit
        row = last_distance[diagonal + 1] + 1;
        edit_pointers[edit_array][edit].operation = DELETE;
        edit_pointers[edit_array][edit].previous = script[diagonal + 1];
      } else {

        // move right (set insert operation) and append edit
        row = last_distance[diagonal - 1];
        edit_pointers[edit_array][edit].operation = INSERT;
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



/*
  Free the memory in a shortest edit script.

  Parameter edit_script The shortest edit script to free
*/
void free_shortest_edit_script(struct edit * edit_script) {

  // free memory
  while(edit_script != NULL) {

    // set next edit
    struct edit * temp_edit = edit_script;
    edit_script = edit_script->next;

    // free edit
    free(temp_edit);
    temp_edit = NULL;

  }

}

/*
  Make a compact edit script from the found edits.

  Parameter last_edit            The last edit found
  Parameter edit_script          The shortest edit script

  Returns Then number of edits or an error code (-1 malloc) 
*/
int make_edit_script(struct edit * last_edit, struct edit ** edit_script) {

  struct edit * current_edit = last_edit;

  // holds the length of the short edit script
  int distance = 0;

  // check not NULL
  if(current_edit == NULL)
    return distance;

  current_edit->next = NULL;

  // find first edit in shortest edit script
  while(current_edit->previous != NULL) {

    current_edit->previous->next = current_edit;
    current_edit = current_edit->previous;

  }

  // copy first edit
  if(((*edit_script) = copy_edit(current_edit)) == NULL)
    return -1;

  current_edit = (*edit_script);

  // condense edit script
  while(current_edit != NULL) {

    // one more compact edit
    ++distance;

    // find same continous edits
    current_edit->length = 1;

    // condense insert edit
    if(current_edit->operation == INSERT)
      while(current_edit->next != NULL
            && (current_edit->operation == current_edit->next->operation)
            && (current_edit->offset_sequence_one == current_edit->next->offset_sequence_one)) {

        // add adjacent edit
        current_edit->next = current_edit->next->next;

        // update length
        ++current_edit->length;

      }

    // condense delete
    else
      while(current_edit->next != NULL
            && (current_edit->operation == current_edit->next->operation)
            && ((current_edit->offset_sequence_one + current_edit->length) == current_edit->next->offset_sequence_one)) {

        // add adjacent edit
        current_edit->next = current_edit->next->next;

        // update length
        ++current_edit->length;

      }

    // copy next edit
    if(current_edit->next != NULL) {

      struct edit * next;
      if((next = copy_edit(current_edit->next)) == NULL) {

        // free allocated edit
        free_shortest_edit_script(current_edit);

        // no script on error
        (*edit_script) = NULL;

        return -1;

      }

      current_edit->next = next;

      // reattach with copied edit
      current_edit->next->previous = current_edit;

    }

    // correct offset
    --current_edit->offset_sequence_one;
    --current_edit->offset_sequence_two;

    current_edit = current_edit->next;

  }

  return distance;

}

/*
  Copy a node from the heap.

  Parameter edit          Edit to copy

  Returns The copied edit or NULL if failed
*/
struct edit * copy_edit(struct edit * edit) {

  struct edit * new_edit;
  if((new_edit = (struct edit *)malloc(sizeof(struct edit))) == NULL)
    return NULL;

  // copy contents
  new_edit->operation = edit->operation;
  new_edit->offset_sequence_one = edit->offset_sequence_one;
  new_edit->offset_sequence_two = edit->offset_sequence_two;
  new_edit->length = edit->length;
  new_edit->next = edit->next;
  new_edit->previous = edit->previous;

  return new_edit;
}
