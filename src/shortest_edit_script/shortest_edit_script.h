/*
  shortest_edit_sequence.h

  Interface to compute the shortest edit script between two sequences.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#ifndef INCLUDED_SHORTEST_EDIT_SCRIPT_H
#define INCLUDED_SHORTEST_EDIT_SCRIPT_H

#ifdef __cplusplus
extern "C" {
#endif

// edit constants
#define SESCOMMON 0
#define SESDELETE -1
#define SESINSERT 1
#define SESCHANGE 2

// single edit script
struct edit {

  int operation;
  int offset_sequence_one;
  int offset_sequence_two;
  int length;

  // links
  struct edit * next;
  struct edit * previous;

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
  struct edit ** edit_script, 
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);
int shortest_edit_script_linear_space(const void * sequence_one, int sequence_one_end, const void * sequence_two, int sequence_two_end,
  struct edit ** edit_script,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);
int shortest_edit_script_hybrid(const void * sequence_one, int sequence_one_end, const void * sequence_two, int sequence_two_end,
  struct edit ** edit_script,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context,
  int threshold);

/*
  Free the memory in a shortest edit script.

  Parameter edit_script The shortest edit script to free
*/
void free_shortest_edit_script(struct edit * edit_script);


/*
  Check if next edit is a change.
*/
int is_change(struct edit * edit_script);

#ifdef __cplusplus
}
#endif

#endif
