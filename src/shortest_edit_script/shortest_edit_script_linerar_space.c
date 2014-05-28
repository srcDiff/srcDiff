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
#include <math.h>

struct point {

  int x;
  int y;

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
int shortest_edit_script_linear_space(int sequence_one_size, const void * sequence_one, int sequence_two_size, const void * sequence_two,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), struct edit ** edit_script, const void * context) {  return -2; }

int compute_middle_snake(int sequence_one_size, const void * sequence_one, int sequence_two_size, const void * sequence_two, struct point * points,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context) {

  // compute delta
  int delta = sequence_one_size - sequence_two_size;
  int is_even = delta % 2;

  // compute center
  int center = ceil((sequence_one_size + sequence_two_size) / 2);

  unsigned int paths_length = center * 2 + 1;
  // allocate arrays to record furthest reaching paths
  struct point * forward_paths = (struct point *)malloc(sizeof(struct point) * paths_length)
  memset(forward_paths, 0, paths_length);

  struct point * reverse_paths = (struct point *)malloc(sizeof(struct point) * paths_length);
  size_t path_pos;
  for(path_pos = 0; path_pos < paths_length; ++path_pos)
    reverse_paths[path_pos] = { sequence_one_size - 1, sequence_two_size - 1 };

  int distance;
  for(distance = 0; distance <= center; ++distance ) {

    int diagonal;
    for(diagonal = -distance; diagonal <= distance; diagonal += 2) {

      int diagonal_pos = diagonal + center;
      int column = forward_paths[diagonal_pos - 1].x;
      if(diagonal == -distance || (diagonal == D && forward_paths[diagonal_pos - 1].x < forward_paths[diagonal_pos + 1].x))
        column = forward_paths[diagonal_pos + 1].x;
      row = column - diagonal;

      while(column < sequence_one_size && row < sequence_two_size && compare(accessor(column, sequence_one, context), accessor(row, sequence_two, context)) == 0) {

        ++column;
        ++row;

      }

      if(!is_even && forward_paths[diagonal_pos].x >= reverse_paths[diagonal_pos].y && forward_paths[diagonal_pos].y >= reverse_paths[diagonal_pos].y)
        return { reverse_paths[diagonal_pos], forward_paths[diagonal_pos] };

    }


  }

  return -2;
}

