// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script_hybrid.c
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
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
  struct edit_t ** edit_script, struct edit_t ** last_edit,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context,
  int threshold) {  

  if(edit_script) (*edit_script) = 0;
  if(last_edit) (*last_edit) = 0;

  int edit_distance = 0;
  if((sequence_one_end - sequence_one_start) > 0 && (sequence_two_end - sequence_two_start) > 0) {

    struct point points[2];
    edit_distance = compute_middle_snake(sequence_one, sequence_one_start, sequence_one_end, sequence_two, sequence_two_start, sequence_two_end, points, compare, accessor, context);
    // what does -2 mean here
    if(edit_distance == -2) { /*fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, "Possible Error");*/ exit(-2); } 

    if(edit_distance > 1) {

      struct edit_t * previous_edits = 0;
      struct edit_t * previous_last_edit = 0;
      if(edit_distance < threshold) {
        shortest_edit_script_inner(sequence_one, sequence_one_start, points[0].x, sequence_two, sequence_two_start, points[0].y, &previous_edits, &previous_last_edit, compare, accessor, context);
      }
      else {
        shortest_edit_script_hybrid_inner(sequence_one, sequence_one_start, points[0].x, sequence_two, sequence_two_start, points[0].y, &previous_edits, &previous_last_edit, compare, accessor, context, threshold);
      }

      if(edit_script) (*edit_script) = previous_edits;

      struct edit_t * new_edits = 0;
      struct edit_t * next_last_edit = 0;
      if(edit_distance < threshold) {
        shortest_edit_script_inner(sequence_one, points[1].x, sequence_one_end, sequence_two, points[1].y, sequence_two_end, &new_edits, &next_last_edit, compare, accessor, context);
      }
      else {
        shortest_edit_script_hybrid_inner(sequence_one, points[1].x, sequence_one_end, sequence_two, points[1].y, sequence_two_end, &new_edits, &next_last_edit, compare, accessor, context, threshold);
      }

      previous_last_edit->next = new_edits;
      new_edits->previous = previous_last_edit;

      if(last_edit) (*last_edit) = next_last_edit;

    } else if((sequence_two_end - sequence_two_start) < (sequence_one_end - sequence_one_start)) {

      struct edit_t * new_edit = (struct edit_t *)malloc(sizeof(struct edit_t));
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

      struct edit_t * new_edit = (struct edit_t *)malloc(sizeof(struct edit_t));
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

    struct edit_t * new_edit = (struct edit_t *)malloc(sizeof(struct edit_t));
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
  struct edit_t ** edit_script,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context,
  int threshold) { 

  int sequence_one_start = 0, sequence_two_start = 0;
  while(sequence_one_start < sequence_one_end && sequence_two_start < sequence_two_end 
    && compare(accessor(sequence_one_start, sequence_one, context), accessor(sequence_two_start, sequence_two, context), context) == 0) {

    ++sequence_one_start;
    ++sequence_two_start;

  }

  int max_distance = ceil(((sequence_one_end - sequence_one_start) + (sequence_two_end - sequence_two_start)) / 2.0) * 2;

  if(max_distance < threshold) {
    shortest_edit_script_inner(sequence_one, sequence_one_start, sequence_one_end, sequence_two, sequence_two_start, sequence_two_end, edit_script, 0,
      compare, accessor, context);
  }
  else {
    shortest_edit_script_hybrid_inner(sequence_one, sequence_one_start, sequence_one_end, sequence_two, sequence_two_start, sequence_two_end, edit_script, 0,
      compare, accessor, context, threshold);
  }

  int edit_distance = merge_sequential_edits(edit_script);

  return edit_distance;

}
