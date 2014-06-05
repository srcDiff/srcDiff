/*
  shortest_edit_sequence.c

  Compute the shortest edit script between two sequences.

  Miller and Myers Shortest Edit Script Algorithm.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

//#include <shortest_edit_script.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <shortest_edit_script.h>

/*
  Make a compact edit script from the found edits.

  Parameter edit_script          The shortest edit script

  Returns -1 on fail, 0 otherwise
*/
int merge_sequential_edits(struct edit ** edit_script);

struct point {

  int x;
  int y;

};

struct point compute_next_forward_path_snake(const void * sequence_one, int sequence_one_end, const void * sequence_two, int sequence_two_end,
  struct point * forward_paths, int distance, int diagonal, int diagonal_pos,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {

  int column, row;
  if(diagonal == -distance || (diagonal != distance && forward_paths[diagonal_pos - 1].x <= forward_paths[diagonal_pos + 1].x)) {

    column = forward_paths[diagonal_pos + 1].x;
    if(distance != 0) 
      row = forward_paths[diagonal_pos + 1].y + 1;
    else
      row = forward_paths[diagonal_pos + 1].y;

  } else {

    column = forward_paths[diagonal_pos - 1].x + 1;
    row = forward_paths[diagonal_pos - 1].y;
  }

  int save_column = column;
  int save_row = row;

  struct point start_snake = { column, row };

  while(column < sequence_one_end && row < sequence_two_end && compare(accessor(column, sequence_one, context), accessor(row, sequence_two, context), context) == 0) {

    ++column;
    ++row;

  }

  forward_paths[diagonal_pos].x = column;
  forward_paths[diagonal_pos].y = row;

  return start_snake;

}

struct point compute_next_reverse_path_snake(const void * sequence_one, int sequence_one_start, const void * sequence_two, int sequence_two_start,
  struct point * reverse_paths, int distance, int diagonal, int diagonal_pos,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {

      int row, column;
      if(diagonal == distance || (diagonal != -distance && reverse_paths[diagonal_pos + 1].y > reverse_paths[diagonal_pos - 1].y)) {

          row = reverse_paths[diagonal_pos - 1].y;
          column = reverse_paths[diagonal_pos - 1].x;

      } else {

          row = reverse_paths[diagonal_pos + 1].y;
          column = reverse_paths[diagonal_pos + 1].x;

      }

      int save_column = column;
      int save_row = row;

      while(column > sequence_one_start && row > sequence_two_start && compare(accessor(column - 1, sequence_one, context), accessor(row - 1, sequence_two, context), context) == 0) {

        --column;
        --row;

      }


      if(distance > 0) {

        if(diagonal == distance || (diagonal != -distance && reverse_paths[diagonal_pos + 1].y > reverse_paths[diagonal_pos - 1].y)) {

          reverse_paths[diagonal_pos].x = column;
          reverse_paths[diagonal_pos].y = row -= 1;

        } else {

          reverse_paths[diagonal_pos].x = column -= 1;
          reverse_paths[diagonal_pos].y = row;

        }

      } else {

        reverse_paths[diagonal_pos].x = column;
        reverse_paths[diagonal_pos].y = row;

      }

      struct point end_snake = { column, row };

      return end_snake;

}

int compute_middle_snake(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end, struct point points[2],
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {

  // compute delta
  int delta = (sequence_one_end - sequence_one_start) - (sequence_two_end - sequence_two_start);
  int is_even = delta % 2 == 0;

  // compute center
  int center = ceil(((sequence_one_end - sequence_one_start) + (sequence_two_end - sequence_two_start)) / 2) + 1;

  unsigned int paths_length = center * 2 + 1;

  // allocate arrays to record furthest reaching paths
  struct point * forward_paths = (struct point *)malloc(sizeof(struct point) * paths_length);
  size_t path_pos;
  struct point start_point = { sequence_one_start, sequence_two_start };
  for(path_pos = 0; path_pos < paths_length; ++path_pos)
    forward_paths[path_pos] = start_point;

  struct point * reverse_paths = (struct point *)malloc(sizeof(struct point) * paths_length);
  struct point end_point = { sequence_one_end, sequence_two_end };
  for(path_pos = 0; path_pos < paths_length; ++path_pos)
    reverse_paths[path_pos] = end_point;

  points[0] = start_point;
  points[1] = end_point;

  int distance;
  for(distance = 0; distance <= center; ++distance ) {

    int diagonal;
    for(diagonal = distance; diagonal >= -distance; diagonal -= 2) {

      int diagonal_pos = diagonal + center;

      struct point start_snake = compute_next_forward_path_snake(sequence_one, sequence_one_end, sequence_two, sequence_two_end,
        forward_paths, distance, diagonal, diagonal_pos, compare, accessor, context);

      // not sure if > or >= or if matters
      if(!is_even && diagonal >= (delta - (distance - 1)) && diagonal <= (delta + (distance - 1))
       && (forward_paths[diagonal_pos].x - forward_paths[diagonal_pos].y) == (reverse_paths[diagonal_pos].x - reverse_paths[diagonal_pos].y)
        && forward_paths[diagonal_pos].x >= reverse_paths[diagonal_pos].x) {

        struct point end_snake = { forward_paths[diagonal_pos].x, forward_paths[diagonal_pos].y };
        points[0] = start_snake;
        points[1] = end_snake;

        free(forward_paths);
        free(reverse_paths);

        return 2 * distance - 1;

      }

    }

    for(diagonal = distance; diagonal >= -distance; diagonal -= 2) {

      int diagonal_pos = diagonal + delta + center;

      struct point end_snake = compute_next_reverse_path_snake(sequence_one, sequence_one_start, sequence_two, sequence_two_start,
        reverse_paths, distance, diagonal, diagonal_pos, compare, accessor, context);

      if(is_even && (diagonal + delta) >= -distance && (diagonal + delta) <= distance
       && (forward_paths[diagonal_pos].x - forward_paths[diagonal_pos].y) == (reverse_paths[diagonal_pos].x - reverse_paths[diagonal_pos].y)
        && forward_paths[diagonal_pos].x >= reverse_paths[diagonal_pos].x) {

        int column =  end_snake.x;
        int row =  end_snake.y;
        while(column > sequence_one_start && row > sequence_two_start && compare(accessor(column - 1, sequence_one, context), accessor(row - 1, sequence_two, context), context) == 0) {

          --column;
          --row;

        }

        struct point start_snake = { column, row };
        points[0] = start_snake;
        points[1] = end_snake;

        free(forward_paths);
        free(reverse_paths);

        return 2 * distance;

      }



    }

  }

  return -2;

}

/*
  Finds the shortest edit script between two sequences.
   
  Parameter sequence_one_end    The size of the first sequence
  Parameter sequence_one         The first sequence
  Parameter sequence_two_end    The size of the second sequence
  Parameter sequence_two         The second sequence
  Parameter edit_script          The shortest edit script

  Returns Then number of edits or an error code (-1 malloc, -2 otherwise) 
*/

int shortest_edit_script_linear_space_inner(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end,
  struct edit ** edit_script, struct edit ** last_edit,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {  

  //fprintf(stderr, "Point: (%d,%d)->(%d,%d)\n", sequence_one_start, sequence_two_start, sequence_one_end, sequence_two_end);

  if(edit_script) (*edit_script) = 0;
  if(last_edit) (*last_edit) = 0;

  int distance = 0;
  if((sequence_one_end - sequence_one_start) > 0 && (sequence_two_end - sequence_two_start) > 0) {

    struct point points[2];
    distance = compute_middle_snake(sequence_one, sequence_one_start, sequence_one_end, sequence_two, sequence_two_start, sequence_two_end, points, compare, accessor, context);

    //fprintf(stderr, "Point: (%d,%d)->(%d,%d)\n", points[0].x, points[0].y, points[1].x, points[1].y);
    //fprintf(stderr, "Distance: %d\n", distance);

    if(distance == -2) { fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, "Possible Error"); exit(-2); } 

    if(distance > 1) {

      struct edit * previous_edits = 0;
      struct edit * previous_last_edit = 0;
      shortest_edit_script_linear_space_inner(sequence_one, sequence_one_start, points[0].x, sequence_two, sequence_two_start, points[0].y, &previous_edits, &previous_last_edit, compare, accessor, context);

      if(edit_script) (*edit_script) = previous_edits;

      // int pos;
      // for(pos = points[0].x; pos < points[1].x; ++pos)
      //   fprintf(stderr, "%s\n", (const char *)accessor(pos, sequence_one, context));

      struct edit * new_edits = 0;
      struct edit * next_last_edit = 0;
      shortest_edit_script_linear_space_inner(sequence_one, points[1].x, sequence_one_end, sequence_two, points[1].y, sequence_two_end, &new_edits, &next_last_edit, compare, accessor, context);

      previous_last_edit->next = new_edits;
      new_edits->previous = previous_last_edit;

      if(last_edit) (*last_edit) = next_last_edit;

    } else if((sequence_two_end - sequence_two_start) < (sequence_one_end - sequence_one_start)) {

      // fprintf(stderr, "Range Old: %d->%d\n", sequence_one_start, sequence_one_end);
      // fprintf(stderr, "Range New: %d->%d\n", sequence_two_start, sequence_two_end);
      // fprintf(stderr, "Point: (%d,%d)->(%d,%d)\n", points[0].x, points[0].y, points[1].x, points[1].y);
      struct edit * new_edit = (struct edit *)malloc(sizeof(struct edit));
      new_edit->operation = SESDELETE;
      new_edit->previous = 0;
      new_edit->next = 0;

      if((sequence_two_end - sequence_two_start) <= 0) {

        new_edit->offset_sequence_one = points[0].x;
        new_edit->offset_sequence_two = points[0].y;
        new_edit->length = 1;

      } else if(points[0].x > sequence_one_start && points[0].y >= sequence_two_start 
        && compare(accessor(points[0].x - 1, sequence_one, context), accessor(points[0].y, sequence_two, context), context) != 0) {

        new_edit->offset_sequence_one = points[0].x - 1;
        new_edit->offset_sequence_two = points[0].y;
        new_edit->length = 1;

      } else {

        new_edit->offset_sequence_one = points[1].x + 1;
        new_edit->offset_sequence_two = points[1].y;
        new_edit->length = 1;

      }

      if(edit_script) (*edit_script) = new_edit;
      if(last_edit) (*last_edit) = new_edit;

    } else {

      // fprintf(stderr, "Range Old: %d->%d\n", sequence_one_start, sequence_one_end);
      // fprintf(stderr, "Range New: %d->%d\n", sequence_two_start, sequence_two_end);
      // fprintf(stderr, "Point: (%d,%d)->(%d,%d)\n", points[0].x, points[0].y, points[1].x, points[1].y);
      struct edit * new_edit = (struct edit *)malloc(sizeof(struct edit));
      new_edit->operation = SESINSERT;
      new_edit->previous = 0;
      new_edit->next = 0;

      if((sequence_one_end - sequence_one_start) <= 0) {

        new_edit->offset_sequence_one = points[0].x;
        new_edit->offset_sequence_two = points[0].y;
        new_edit->length = 1;

      } else if(points[0].x >= sequence_one_start && points[0].y > sequence_two_start 
        && compare(accessor(points[0].x, sequence_one, context), accessor(points[0].y - 1, sequence_two, context), context) != 0) {

        new_edit->offset_sequence_one = points[0].x;
        new_edit->offset_sequence_two = points[0].y - 1;
        new_edit->length = 1;

      } else {

        new_edit->offset_sequence_one = points[1].x;
        new_edit->offset_sequence_two = points[1].y + 1;
        new_edit->length = 1;

      }

      if(edit_script) (*edit_script) = new_edit;
      if(last_edit) (*last_edit) = new_edit;

    }

  } else if((sequence_one_end - sequence_one_start) > 0 || (sequence_two_end - sequence_two_start) > 0) {

    struct edit * new_edit = (struct edit *)malloc(sizeof(struct edit));
    new_edit->previous = 0;
    new_edit->next = 0;

    if((sequence_one_end - sequence_one_start) > 0) {

      new_edit->operation = SESDELETE;
      new_edit->offset_sequence_one = sequence_one_start;
      new_edit->offset_sequence_two = sequence_two_start;
      new_edit->length = sequence_one_end - sequence_one_start;


    } else if((sequence_two_end - sequence_two_start) > 0) {

      new_edit->operation = SESINSERT;
      new_edit->offset_sequence_one = sequence_one_start;
      new_edit->offset_sequence_two = sequence_two_start;
      new_edit->length = sequence_two_end - sequence_two_start;

    }

    if(edit_script) (*edit_script) = new_edit;
    if(last_edit) (*last_edit) = new_edit;

  }

  return distance;

}

int shortest_edit_script_linear_space(const void * sequence_one, int sequence_one_end, const void * sequence_two, int sequence_two_end,
  struct edit ** edit_script,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) { 

  int edit_distance = shortest_edit_script_linear_space_inner(sequence_one, 0, sequence_one_end, sequence_two, 0, sequence_two_end, edit_script, 0,
    compare, accessor, context);

  merge_sequential_edits(edit_script);

  return edit_distance;

}

int merge_sequential_edits(struct edit ** edit_script) {

  int edit_distance = 0;
  struct edit * current_edit = *edit_script;

  // condense edit script
  while(current_edit != NULL) {

    ++edit_distance;

    // condense insert edit
    if(current_edit->operation == SESINSERT) {

      while(current_edit->next != NULL
            && (current_edit->operation == current_edit->next->operation)
            && (current_edit->offset_sequence_one == current_edit->next->offset_sequence_one)) {

        // update length
        current_edit->length += current_edit->next->length;

        // save edit for deletion
        struct edit * save_edit = current_edit->next;

        // add adjacent edit
        current_edit->next = current_edit->next->next;
        if(current_edit->next) current_edit->next->previous = current_edit;

        // delete edit
        free(save_edit);

      }

    // condense delete
    } else {

      while(current_edit->next != NULL
            && (current_edit->operation == current_edit->next->operation)
            && ((current_edit->offset_sequence_one + current_edit->length) == current_edit->next->offset_sequence_one)) {

        // update length
        current_edit->length += current_edit->next->length;

        // save edit for deletion
        struct edit * save_edit = current_edit->next;

        // add adjacent edit
        current_edit->next = current_edit->next->next;
        if(current_edit->next) current_edit->next->previous = current_edit;

        // delete edit
        free(save_edit);

      }

    }

    current_edit = current_edit->next;

  }

  return edit_distance;

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

  shortest_edit_script_linear_space(sequence_one, 4, sequence_two, 4, &edit_script, str_compare, str_accessor, 0);
  //shortest_edit_script_linear_space(sequence_one, 7, sequence_two, 6, &edit_script, str_compare, str_accessor, 0);
  //shortest_edit_script_linear_space(sequence_one, 10, sequence_two, 14, &edit_script, str_compare, str_accessor, 0);

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
