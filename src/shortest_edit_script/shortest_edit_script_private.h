/*
  shortest_edit_sequence.c

  Compute the shortest edit script between two sequences.

  Miller and Myers Shortest Edit Script Algorithm.

  Michael John Decker
  mdecker6@kent.edu
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
int merge_sequential_edits(struct edit ** edit_script);

int compute_middle_snake(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end, struct point points[2],
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);

int shortest_edit_script_linear_space_inner(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end,
  struct edit ** edit_script, struct edit ** last_edit,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);

/*
  Make a compact edit script from the found edits.

  Parameter last_edit            The last edit found
  Parameter edit_script          The shortest edit script

  Returns -1 on fail, 0 otherwise
*/
int make_edit_script(struct edit * start_edit, struct edit ** edit_script, struct edit ** last_edit);

/*
  Copy a node from the heap.

  Parameter edit          Edit to copy

  Returns The copied edit or NULL if failed
*/
struct edit * copy_edit(struct edit * edit);

int shortest_edit_script_inner(const void * sequence_one, int sequence_one_start, int sequence_one_end, const void * sequence_two, int sequence_two_start, int sequence_two_end,
  struct edit ** edit_script, struct edit ** last_edit,
  int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);
