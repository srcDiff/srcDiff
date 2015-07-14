/*
  shortest_edit_sequence.c

  Compute the shortest edit script between two sequences.

  Miller and Myers Shortest Edit Script Algorithm.

  Michael John Decker
  mdecker6@kent.edu
*/

//#include <shortest_edit_script.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <shortest_edit_script.h>
#include <shortest_edit_script_private.h>

/*
  Finds the shortest edit script between two sequences.
   
  Parameter sequence_one_end    The size of the first sequence
  Parameter sequence_one         The first sequence
  Parameter sequence_two_end    The size of the second sequence
  Parameter sequence_two         The second sequence
  Parameter edit_script          The shortest edit script

  Returns Then number of edits or an error code (-1 malloc, -2 otherwise) 
*/

int shortest_edit_script_hybrid_inner(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end,
  struct edit ** edit_script, struct edit ** last_edit,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context,
  int threshold) {  

  //fprintf(stderr, "Point: (%d,%d)->(%d,%d)\n", sequence_one_start, sequence_two_start, sequence_one_end, sequence_two_end);

  if(edit_script) (*edit_script) = 0;
  if(last_edit) (*last_edit) = 0;

  int edit_distance = 0;
  if((sequence_one_end - sequence_one_start) > 0 && (sequence_two_end - sequence_two_start) > 0) {

    struct point points[2];
    edit_distance = compute_middle_snake(sequence_one, sequence_one_start, sequence_one_end, sequence_two, sequence_two_start, sequence_two_end, points, compare, accessor, context);

    //fprintf(stderr, "Point: (%d,%d)->(%d,%d)\n", points[0].x, points[0].y, points[1].x, points[1].y);
    //fprintf(stderr, "Distance: %d\n", edit_distance);

    if(edit_distance == -2) { fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, "Possible Error"); exit(-2); } 

    if(edit_distance > 1) {

      struct edit * previous_edits = 0;
      struct edit * previous_last_edit = 0;
      if(edit_distance < threshold)
        shortest_edit_script_inner(sequence_one, sequence_one_start, points[0].x, sequence_two, sequence_two_start, points[0].y, &previous_edits, &previous_last_edit, compare, accessor, context);
      else
        shortest_edit_script_hybrid_inner(sequence_one, sequence_one_start, points[0].x, sequence_two, sequence_two_start, points[0].y, &previous_edits, &previous_last_edit, compare, accessor, context, threshold);

      if(edit_script) (*edit_script) = previous_edits;

      // int pos;
      // for(pos = points[0].x; pos < points[1].x; ++pos)
      //   fprintf(stderr, "%s\n", (const char *)accessor(pos, sequence_one, context));

      struct edit * new_edits = 0;
      struct edit * next_last_edit = 0;
      if(edit_distance < threshold)
        shortest_edit_script_inner(sequence_one, points[1].x, sequence_one_end, sequence_two, points[1].y, sequence_two_end, &new_edits, &next_last_edit, compare, accessor, context);
      else
        shortest_edit_script_hybrid_inner(sequence_one, points[1].x, sequence_one_end, sequence_two, points[1].y, sequence_two_end, &new_edits, &next_last_edit, compare, accessor, context, threshold);

      previous_last_edit->next = new_edits;
      new_edits->previous = previous_last_edit;

      if(last_edit) (*last_edit) = next_last_edit;

    } else if((sequence_two_end - sequence_two_start) < (sequence_one_end - sequence_one_start)) {

      // fprintf(stderr, "Range Old: %d->%d\n", sequence_one_start, sequence_one_end);
      // fprintf(stderr, "Range New: %d->%d\n", sequence_two_start, sequence_two_end);
      // fprintf(stderr, "Point: (%d,%d)->(%d,%d)\n", points[0].x, points[0].y, points[1].x, points[1].y);
      struct edit * new_edit = (struct edit *)malloc(sizeof(struct edit));
      new_edit->operation = SES_DELETE;
      new_edit->previous = 0;
      new_edit->next = 0;

      if((sequence_two_end - sequence_two_start) <= 0) {

        new_edit->offset_sequence_one = points[0].x;
        new_edit->offset_sequence_two = points[0].y;
        new_edit->length = 1;

      } else if(points[0].x >= sequence_one_end
        || (points[0].x > sequence_one_start && points[0].y >= sequence_two_start 
        && compare(accessor(points[0].x - 1, sequence_one, context), accessor(points[0].y, sequence_two, context), context) != 0)) {

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
      new_edit->operation = SES_INSERT;
      new_edit->previous = 0;
      new_edit->next = 0;

      if((sequence_one_end - sequence_one_start) <= 0) {

        new_edit->offset_sequence_one = points[0].x;
        new_edit->offset_sequence_two = points[0].y;
        new_edit->length = 1;

      } else if(points[0].y >= sequence_two_end
       || (points[0].x >= sequence_one_start && points[0].y > sequence_two_start 
        && compare(accessor(points[0].x, sequence_one, context), accessor(points[0].y - 1, sequence_two, context), context) != 0)) {

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

      new_edit->operation = SES_DELETE;
      new_edit->offset_sequence_one = sequence_one_start;
      new_edit->offset_sequence_two = sequence_two_start;
      new_edit->length = sequence_one_end - sequence_one_start;


    } else if((sequence_two_end - sequence_two_start) > 0) {

      new_edit->operation = SES_INSERT;
      new_edit->offset_sequence_one = sequence_one_start;
      new_edit->offset_sequence_two = sequence_two_start;
      new_edit->length = sequence_two_end - sequence_two_start;

    }

    if(edit_script) (*edit_script) = new_edit;
    if(last_edit) (*last_edit) = new_edit;

  }

  return edit_distance;

}

int shortest_edit_script_hybrid(const void * sequence_one, int sequence_one_end, const void * sequence_two, int sequence_two_end,
  struct edit ** edit_script,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context,
  int threshold) { 

  int sequence_one_start = 0, sequence_two_start = 0;
  while(sequence_one_start < sequence_one_end && sequence_two_start < sequence_two_end 
    && compare(accessor(sequence_one_start, sequence_one, context), accessor(sequence_two_start, sequence_two, context), context) == 0) {

    ++sequence_one_start;
    ++sequence_two_start;

  }

  int max_distance = ceil(((sequence_one_end - sequence_one_start) + (sequence_two_end - sequence_two_start)) / 2.0) * 2;

  if(max_distance < threshold)
    shortest_edit_script_inner(sequence_one, sequence_one_start, sequence_one_end, sequence_two, sequence_two_start, sequence_two_end, edit_script, 0,
      compare, accessor, context);
  else
    shortest_edit_script_hybrid_inner(sequence_one, sequence_one_start, sequence_one_end, sequence_two, sequence_two_start, sequence_two_end, edit_script, 0,
      compare, accessor, context, threshold);

  int edit_distance = merge_sequential_edits(edit_script);

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
    const char ** sequence = current_edit->operation == SES_DELETE ? sequence_one : sequence_two;
    for(int i = 0; i < current_edit->length; ++i) {

      fprintf(stderr, "%s: ",current_edit->operation == SES_DELETE ? "DELETE" : "INSERT");
      fprintf(stderr, "%s\n", sequence[current_edit->operation == SES_DELETE ? current_edit->offset_sequence_one + i : current_edit->offset_sequence_two + i]);

    }

  }

  return 0;

}
#endif
