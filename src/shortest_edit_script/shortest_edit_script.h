// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script.h
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SHORTEST_EDIT_SCRIPT_H
#define INCLUDED_SHORTEST_EDIT_SCRIPT_H

#ifdef __cplusplus
extern "C" {
#endif

// edit constants
enum edit_operations { SES_COMMON, SES_DELETE, SES_INSERT, /** possibly temp for longer refactoring */SES_CHANGE };

// single edit script
struct edit_t {

  int operation;
  unsigned long offset_sequence_one;
  unsigned long offset_sequence_two;
  unsigned long length;

  // links
  struct edit_t * next;
  struct edit_t * previous;

  // temporary for use for longer refactoring
  unsigned long length_two;
};

/*
  Finds the shortest edit script between two sequences.

  Parameter sequence_one_size    The size of the first sequence
  Parameter sequence_one         The first sequence
  Parameter sequence_two_size    The size of the second sequence
  Parameter sequence_two         The second sequence
  Parameter edit_script          The shortest edit script

  Returns The number of edits or an error code (-1 malloc, -2 otherwise) 
*/
int shortest_edit_script(const void * sequence_one, int sequence_one_size, const void * sequence_two, int sequence_two_size,
  struct edit_t ** edit_script, 
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);
int shortest_edit_script_linear_space(const void * sequence_one, int sequence_one_end, const void * sequence_two, int sequence_two_end,
  struct edit_t ** edit_script,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);
int shortest_edit_script_hybrid(const void * sequence_one, int sequence_one_end, const void * sequence_two, int sequence_two_end,
  struct edit_t ** edit_script,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context,
  int threshold);

/*
  Free the memory in a shortest edit script.

  Parameter edit_script The shortest edit script to free
*/
void free_shortest_edit_script(struct edit_t * edit_script);


/*
  Check if next edit is a change.
*/
int is_change(const struct edit_t * edit_script);

/*
  Shallow copy of an edit

  Parameter edit          Edit to copy

  Returns The copied edit or NULL if failed
*/
struct edit_t * copy_edit(struct edit_t * edit);

#ifdef __cplusplus
}
#endif

#endif
