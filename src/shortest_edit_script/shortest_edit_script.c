/*
  shortest_edit_sequence.c

  Compute the shortest edit script between two sequences.

  Miller and Myers Shortest Edit Script Algorithm.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <shortest_edit_script.h>
#include <shortest_edit_script_private.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
  struct edit ** edit_script, struct edit ** last_edit,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {

  // max edit distance
  int max_distance = (sequence_one_end - sequence_one_start) + (sequence_two_end - sequence_two_start) + 1;

  int max_diagonals = 2 * max_distance + 1;

  // last row with edit distance for each diagonal
  struct point * last_distance = (struct point *)calloc(max_diagonals, sizeof(struct point));

  // hold all allocates
  struct edit ** edit_pointers = (struct edit **)calloc(max_diagonals, sizeof(struct edit *));

  int num_edits = -1;

  // internal script of edits
  //struct edit * script[max_diagonals];
  struct edit ** script = (struct edit **)calloc(max_diagonals, sizeof(struct edit *));
  if(script == NULL) {

    (*edit_script) = NULL;
    return -2;

  }

  // initialization, slide 0 along 0 diagonal and find 1st edit
  int row = sequence_one_start;
  int column = sequence_two_start;
  for(; row < sequence_one_end && column < sequence_two_end && compare(accessor(row, sequence_one, context), accessor(column, sequence_two, context), context) == 0; ++row, ++column)
    ;

  // center to start building differences
  int center = max_distance;

  // set 0 diagonal's row of distance and set beginning of script
  last_distance[center].x = row;
  last_distance[center].y = column;
  script[center] = NULL;

  // identical files
  if(row >= sequence_one_end && column >= sequence_two_end) {
    
    if(edit_script) (*edit_script) = NULL;;
    if(last_edit) (*last_edit) = NULL;;

    return 0;

  }

  // for each edit distance
  int distance;
  for(distance = 1; distance <= max_distance; ++distance) {

    // for each possible diagonal
    int diagonal;
    for(diagonal = -distance; diagonal <= distance; diagonal += 2) {
      // fprintf(stderr, "Distance %d Diagonal: %d\n", distance, diagonal);

      // locate next edit
      ++num_edits;
      int edit_array = num_edits / (max_distance + 1);
      int edit = num_edits % (max_distance + 1);

      if(edit == 0)
        if((edit_pointers[edit_array] = (struct edit *)calloc(max_distance + 1, sizeof(struct edit))) == NULL) {

          // clean allocates
          int i;
          for(i = 0; i < edit_array; ++i)
            free(edit_pointers[i]);

          // no script on error
          (*edit_script) = NULL;
          free(script);

          return -1;

        }

      int diagonal_pos = diagonal + center;

      // move down if no right distance or has farthest down the diagonal
      if(diagonal == -distance || (diagonal != distance && (last_distance[diagonal_pos - 1].x < last_distance[diagonal_pos + 1].x))) {

        // move down (set delete operation) and append edit
        row = last_distance[diagonal_pos + 1].x;
        column= last_distance[diagonal_pos + 1].y + 1;
        edit_pointers[edit_array][edit].operation = SESINSERT;
        edit_pointers[edit_array][edit].previous = script[diagonal_pos + 1];

      } else {

        // move right (set insert operation) and append edit
        row = last_distance[diagonal_pos - 1].x + 1;
        column = last_distance[diagonal_pos - 1].y;
        edit_pointers[edit_array][edit].operation = SESDELETE;
        edit_pointers[edit_array][edit].previous = script[diagonal_pos - 1];

      }

      // fprintf(stderr, "Point: (%d,%d)\n", row, column);

      edit_pointers[edit_array][edit].offset_sequence_one = row;
      edit_pointers[edit_array][edit].offset_sequence_two = column;

      // update the script
      script[diagonal_pos] = &edit_pointers[edit_array][edit];

      // slide down the diagonal
      while(row < sequence_one_end && column < sequence_two_end && compare(accessor(row, sequence_one, context), accessor(column, sequence_two, context), context) == 0) {

        ++row;
        ++column;

      }

      // update diagonal's last distance with row
      last_distance[diagonal_pos].x = row;
      last_distance[diagonal_pos].y = column;

      // reached lower right (finished)
      if(row == sequence_one_end && column == sequence_two_end) {

        // make shortest edit script
        int edit_distance = make_edit_script(&edit_pointers[edit_array][edit], edit_script, last_edit);

        free(last_distance);

        // clean allocates
        int i;
        for(i = 0; i <= edit_array; ++i)
          free(edit_pointers[i]);

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
  for(i = 0; i <= edit_array; ++i)
    free(edit_pointers[i]);

  free(edit_pointers);

  free(script);

  // no edit script on error
  (*edit_script) = NULL;
  (*last_edit) = NULL;

  return -2;
}

int shortest_edit_script(const void * sequence_one, int sequence_one_end, const void * sequence_two, int sequence_two_end,
  struct edit ** edit_script, 
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {

  return shortest_edit_script_inner(sequence_one, 0, sequence_one_end, sequence_two, 0, sequence_two_end, edit_script, 0, compare, accessor, context);

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

  }

}

/*
  Make a compact edit script from the found edits.

  Parameter last_edit            The last edit found
  Parameter edit_script          The shortest edit script

  Returns Then number of edits or an error code (-1 malloc) 
*/
int make_edit_script(struct edit * start_edit, struct edit ** edit_script, struct edit ** last_edit) {

  struct edit * current_edit = start_edit;

  if(edit_script) (*edit_script) = NULL;
  if(last_edit) (*last_edit) = NULL;

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
    if(current_edit->operation == SESINSERT)
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
        current_edit->next = NULL;
        free_shortest_edit_script(*edit_script);


        // no script on error
        (*edit_script) = NULL;

        return -1;

      }

      current_edit->next = next;

      // reattach with copied edit
      current_edit->next->previous = current_edit;

    } else {

      if(last_edit) (*last_edit) = current_edit;

    }

    // correct offset
    if(current_edit->operation == SESDELETE)
      --current_edit->offset_sequence_one;
    else
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

#if 0
int str_compare(const void * str_one, const void * str_two, const void * context) {

  return strcmp((const char *)str_one, (const char *)str_two);

}

const void * str_accessor(int index, const void * array, const void * context) {

  return (void *)((const char **)array)[index];

}

int main(int argc, char * argv[]) {

  const char * sequence_one[] = { "a", "b", "c", "e" };
  const char * sequence_two[] = { "a", "c", "e", "f" };
  //const char * sequence_one[] = { "a", "b", "c", "e" };
  //const char * sequence_two[] = { "b", "c", "d", "e" };
  //const char * sequence_one[] = { "a", "b", "c", "d" };
  //const char * sequence_two[] = { "a", "b", "e", "f" };
  //const char * sequence_one[] = { "a", "b", "c", "a", "b", "b", "a" };
  //const char * sequence_two[] = { "c", "b", "a", "b", "a", "c" };
  //const char * sequence_one[] = { "a", "b", "b", "a", "c", "b", "a" };
  //const char * sequence_two[] = { "c", "a", "b", "a", "b", "c" };
  //const char * sequence_one[] = { "a", "b", "c", "d", "f", "g", "h", "j", "q", "z" };
  //const char * sequence_two[] = { "a", "b", "c", "d", "e", "f", "g", "i", "j", "k", "r", "x", "y", "z" };

  struct edit * edit_script;

  shortest_edit_script(sequence_one, 4, sequence_two, 4, &edit_script, str_compare, str_accessor, 0);
  //shortest_edit_script(sequence_one, 7, sequence_two, 6, &edit_script, str_compare, str_accessor, 0);
  //shortest_edit_script(sequence_one, 10, sequence_two, 14, &edit_script, str_compare, str_accessor, 0);

  for(struct edit * current_edit = edit_script; current_edit; current_edit = current_edit->next) {

fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, current_edit->offset_sequence_one);
fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, current_edit->offset_sequence_two);
    const char ** sequence = current_edit->operation == SESDELETE ? sequence_one : sequence_two;
    for(int i = 0; i < current_edit->length; ++i) {

      fprintf(stderr, "%s: ",current_edit->operation == SESDELETE ? "DELETE" : "INSERT");
      fprintf(stderr, "%s\n", sequence[current_edit->operation == SESDELETE ? current_edit->offset_sequence_one + i : current_edit->offset_sequence_two + i]);

    }

  }

  return 0;

}
#endif

