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

struct point {

  int x;
  int y;

};

int compute_middle_snake(const void * sequence_one, int sequence_one_start, int sequence_one_size, const void * sequence_two, int sequence_two_start, int sequence_two_size, struct point points[2],
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {

  // compute delta
  int delta = (sequence_one_size - sequence_one_start) - (sequence_two_size - sequence_two_start);
  int is_even = delta % 2 == 0;

  // compute center
  int center = ceil(((sequence_one_size - sequence_one_start) + (sequence_two_size - sequence_two_start)) / 2);

  unsigned int paths_length = center * 2 + 1;
  // allocate arrays to record furthest reaching paths
  struct point * forward_paths = (struct point *)malloc(sizeof(struct point) * paths_length);
  size_t path_pos;
  struct point start_point = { sequence_one_start, sequence_two_start };
  for(path_pos = 0; path_pos < paths_length; ++path_pos)
    forward_paths[path_pos] = start_point;

  struct point * reverse_paths = (struct point *)malloc(sizeof(struct point) * paths_length);
  struct point end_point = { sequence_one_size, sequence_two_size };
  for(path_pos = 0; path_pos < paths_length; ++path_pos)
    reverse_paths[path_pos] = end_point;

  points[0] = start_point;
  points[1] = end_point;

  int distance;
  for(distance = 0; distance <= center; ++distance ) {

    int diagonal;
    for(diagonal = -distance; diagonal <= distance; diagonal += 2) {

      int diagonal_pos = diagonal + center;

      int column;
      if(diagonal == -distance || (diagonal != distance && forward_paths[diagonal_pos - 1].x < forward_paths[diagonal_pos + 1].x)) {

        column = forward_paths[diagonal_pos + 1].x;

      } else {

        column = forward_paths[diagonal_pos - 1].x + 1;

      }

      int row = column - diagonal;

      while(column < (sequence_one_size - 1) && row < (sequence_two_size - 1) && compare(accessor(column, sequence_one, context), accessor(row, sequence_two, context), context) == 0) {

        ++column;
        ++row;

      }

      forward_paths[diagonal_pos].x = column;
      forward_paths[diagonal_pos].y = row;

      // not sure if > or >= or if matters
      if(!is_even && diagonal >= (delta - (distance - 1)) && diagonal <= (delta + (distance - 1))
       && forward_paths[diagonal_pos].x > reverse_paths[diagonal_pos].x && forward_paths[diagonal_pos].y > reverse_paths[diagonal_pos].y) {

        points[0] = reverse_paths[diagonal_pos];
        points[1] = forward_paths[diagonal_pos];

        return 2 * distance - 1;

      }

    }

    for(diagonal = -distance; diagonal <= distance; diagonal += 2) {

      int diagonal_pos = diagonal + delta + center;

      int row;
      if(diagonal == distance || (diagonal != -distance && reverse_paths[diagonal_pos + 1].y > reverse_paths[diagonal_pos - 1].y)) {

        row = reverse_paths[diagonal_pos - 1].y - 1;

      } else {

        row = reverse_paths[diagonal_pos + 1].y;

      }

      int column = row + (diagonal + delta);

      while(column > 0 && row > 0 && compare(accessor(column, sequence_one, context), accessor(row, sequence_two, context), context) == 0) {

        --column;
        --row;

      }

      reverse_paths[diagonal_pos].x = column;
      reverse_paths[diagonal_pos].y = row;
      if(is_even && (diagonal + delta) >= -distance && (diagonal + delta) <= distance
       && reverse_paths[diagonal_pos].x < forward_paths[diagonal_pos].x && reverse_paths[diagonal_pos].y < forward_paths[diagonal_pos].y) {

        points[0] = reverse_paths[diagonal_pos];
        points[1] = forward_paths[diagonal_pos];

        return 2 * distance;

      }

    }

  }

  return -2;

}

/*
  Finds the shortest edit script between two sequences.
   
  Parameter sequence_one_size    The size of the first sequence
  Parameter sequence_one         The first sequence
  Parameter sequence_two_size    The size of the second sequence
  Parameter sequence_two         The second sequence
  Parameter edit_script          The shortest edit script

  Returns Then number of edits or an error code (-1 malloc, -2 otherwise) 
*/
int shortest_edit_script_linear_space(const void * sequence_one, int sequence_one_start, int sequence_one_size, const void * sequence_two, int sequence_two_start, int sequence_two_size,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {  
fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, sequence_one_start);
fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, sequence_one_size);
fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, sequence_two_start);
fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, sequence_two_size);
  int distance = -2;
  if((sequence_one_size - sequence_one_start) > 0 && (sequence_two_size - sequence_two_start) > 0) {

    struct point points[2];
    distance = compute_middle_snake(sequence_one, sequence_one_start, sequence_one_size, sequence_two, sequence_two_start, sequence_two_size, points, compare, accessor, context);
    fprintf(stderr, "Point (%d, %d)\n", points[0].x, points[0].y);
    fprintf(stderr, "Point (%d, %d)\n", points[1].x, points[1].y);

    if(distance > 1) {

      shortest_edit_script_linear_space(sequence_one, sequence_one_start, points[0].x + 1, sequence_two, sequence_two_start, points[0].y + 1, compare, accessor, context);
      size_t pos;
      for(pos = points[0].x + 1; pos < points[1].x; ++pos)
        fprintf(stderr, "%s\n", (const char *)accessor(pos, sequence_one, context));
      shortest_edit_script_linear_space(sequence_one, points[1].x, sequence_one_size, sequence_two, points[1].y, sequence_two_size, compare, accessor, context);

    } else if((sequence_two_size - sequence_two_start) > (sequence_one_size - sequence_one_start)) {

        size_t pos;
        for(pos = sequence_one_start; pos < sequence_one_size; ++pos)
          fprintf(stderr, "%s\n", (const char *)accessor(pos, sequence_one, context));

    } else {

        size_t pos;
        for(pos = sequence_two_start; pos < sequence_two_size; ++pos)
          fprintf(stderr, "%s\n", (const char *)accessor(pos, sequence_two, context));

    }

  }

  return -2;

}

int str_compare(const void * str_one, const void * str_two, const void * context) {

  return strcmp((const char *)str_one, (const char *)str_two);

}

const void * str_accessor(int index, const void * array, const void * context) {

  return (void *)((const char **)array)[index];

}

//#if 0
int main(int argc, char * argv[]) {

  const char * sequence_one[] = { "a", "b", "c", "e" };
  const char * sequence_two[] = { "a", "c", "e", "f" };
  //const char * sequence_one[] = { "a", "b", "c", "e" };
  //const char * sequence_two[] = { "b", "c", "d", "e" };

  shortest_edit_script_linear_space(sequence_one, 0, 4, sequence_two, 0, 4, str_compare, str_accessor, 0);

  return 0;

}

//#endif