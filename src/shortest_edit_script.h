/*
  shortest_edit_sequence.h

  Interface to compute the shortest edit script between two sequences.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#ifndef INCLUDED_SHORTEST_EDIT_SCRIPT_H
#define INCLUDED_SHORTEST_EDIT_SCRIPT_H

// edit constants
#define COMMON -1
#define INSERT 0
#define DELETE 1

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

  Returns Then number of edits or an error code (-1 malloc, -2 otherwise) 
*/
int shortest_edit_script(int sequence_one_size, const void * sequence_one, int sequence_two_size, const void * sequence_two, int compare(const void *, const void *), const void * accessor(int index, const void *), struct edit ** edit_script);

/*
  Free the memory in a shortest edit script.

  Parameter edit_script The shortest edit script to free
*/
void free_shortest_edit_script(struct edit * edit_script);

#endif
