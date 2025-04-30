// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script_private.h
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

struct point {

  int x;
  int y;

};

/*
  Make a compact edit script from the found edits.

  Parameter edit_script          The shortest edit script

  Returns -1 on fail, 0 otherwise
*/
int merge_sequential_edits(struct edit_t ** edit_script);

int compute_middle_snake(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end, struct point points[2],
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);

int shortest_edit_script_linear_space_inner(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end,
  struct edit_t ** edit_script, struct edit_t ** last_edit,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);

int shortest_edit_script_inner(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end,
  struct edit_t ** edit_script, struct edit_t ** last_edit,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);
