/*
  test_shortest_edit_script_linear_space.c

  Unit tests for shortest_edit_script_linear_space.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include "shortest_edit_script.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int compare(const void * element_one, const void * element_two, const void * context) {

  const char * e1 = (const char *)element_one;
  const char * e2 = (const char *)element_two;

  int match = strcmp(e1, e2);
  return match;
}

const void * accessor(int position, const void * sequence, const void * context) {

  return (((const char **)sequence)[position]);
}

int main(int argc, char * argv[]) {

  int test_case_number = 0;
  
  // Shortest edit script tests
  
  {
    // NULL test case
    int sequence_one_size = 0;
    const char ** test_sequence_one = NULL;

    int sequence_two_size = 0;
    const char ** test_sequence_two = NULL;

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 0);

    assert(edit_script == NULL);

    free_shortest_edit_script_linear_space(edit_script);
  
  }

  {
    // empty test case
    int sequence_one_size = 0;
    const char * test_sequence_one[sequence_one_size];

    int sequence_two_size = 0;
    const char * test_sequence_two[sequence_two_size];

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 0);

    assert(edit_script == NULL);

    free_shortest_edit_script_linear_space(edit_script);
  
  }

  {
    // edit distance = 1
    int sequence_one_size = 1;
    const char * test_sequence_one[] = { "a" };

    int sequence_two_size = 2;
    const char * test_sequence_two[] = { "a", "b" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 1);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);

    free_shortest_edit_script_linear_space(edit_script);
  
  }

  {
    // edit distance = 1
    int sequence_one_size = 2;
    const char * test_sequence_one[] = { "a", "b" };

    int sequence_two_size = 1;
    const char * test_sequence_two[] = { "a" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    int sequence_one_size = 1;
    const char * test_sequence_one[] = { "b" };

    int sequence_two_size = 2;
    const char * test_sequence_two[] = { "a", "b" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == -1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);

    free_shortest_edit_script_linear_space(edit_script);
  
  }

  {
    // edit distance = 1
    int sequence_one_size = 2;
    const char * test_sequence_one[] = { "a", "b" };

    int sequence_two_size = 1;
    const char * test_sequence_two[] = { "b" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == -1);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    int sequence_one_size = 2;
    const char * test_sequence_one[] = { "a", "c" };

    int sequence_two_size = 3;
    const char * test_sequence_two[] = { "a", "b" , "c"};

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);
 
    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 1);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
 
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    int sequence_one_size = 3;
    const char * test_sequence_one[] = { "a", "b" , "c"};

    int sequence_two_size = 2;
    const char * test_sequence_two[] = { "a", "c" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    int sequence_one_size = 4;
    const char * test_sequence_one[] = { "a", "b" , "c", "d"};

    int sequence_two_size = 3;
    const char * test_sequence_two[] = { "a", "b", "d" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 2);
    assert(edit->offset_sequence_two     == 1);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    int sequence_one_size = 3;
    const char * test_sequence_one[] = { "a", "b" , "d"};

    int sequence_two_size = 4;
    const char * test_sequence_two[] = { "a", "b", "c", "d" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 2);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    int sequence_one_size = 5;
    const char * test_sequence_one[] = { "a", "b" , "c", "d", "e" };

    int sequence_two_size = 4;
    const char * test_sequence_two[] = { "a", "b", "c", "e" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 3);
    assert(edit->offset_sequence_two     == 2);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    int sequence_one_size = 4;
    const char * test_sequence_one[] = { "a", "b" , "c", "e"};

    int sequence_two_size = 5;
    const char * test_sequence_two[] = { "a", "b", "c", "d", "e" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 2);
    assert(edit->offset_sequence_two     == 3);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    int sequence_one_size = 5;
    const char * test_sequence_one[] = { "a", "b" , "c", "d", "e" };

    int sequence_two_size = 3;
    const char * test_sequence_two[] = { "a", "b", "e" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 2);
    assert(edit->offset_sequence_two     == 1);
    assert(edit->length                  == 2);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    int sequence_one_size = 3;
    const char * test_sequence_one[] = { "a", "b", "e" };

    int sequence_two_size = 5;
    const char * test_sequence_two[] =  { "a", "b" , "c", "d", "e" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 2);
    assert(edit->length                  == 2);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 1
    // second sequence NULL test case
    int sequence_one_size = 5;
    const char * test_sequence_one[] =  { "a", "b" , "c", "d", "e" };

    int sequence_two_size = 0;
    const char ** test_sequence_two = NULL;

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == -1);
    assert(edit->length                  == 5);

    assert(edit->next                    == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  
  }

  {
    // edit distance = 1
    // first sequence NULL test case
    int sequence_one_size = 0;
    const char * test_sequence_one = NULL;

    int sequence_two_size = 5;
    const char * test_sequence_two[] =  { "a", "b" , "c", "d", "e" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == -1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 5);

    assert(edit->next                    == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  
  }

  {
    // edit distance = 1
    // second sequence empty test case
    int sequence_one_size = 5;
    const char * test_sequence_one[] =  { "a", "b" , "c", "d", "e" };

    int sequence_two_size = 0;
    const char * test_sequence_two[sequence_two_size];

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == -1);
    assert(edit->length                  == 5);

    assert(edit->next                    == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  
  }

  {
    // edit distance = 1
    // first sequence empty test case
    int sequence_one_size = 0;
    const char * test_sequence_one[sequence_one_size];

    int sequence_two_size = 5;
    const char * test_sequence_two[] =  { "a", "b" , "c", "d", "e" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == -1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 5);

    assert(edit->next                    == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  
  }

  {
    // edit distance = 2
    int sequence_one_size = 5;
    const char * test_sequence_one[] = { "a", "b" , "c", "d", "f" };

    int sequence_two_size = 5;
    const char * test_sequence_two[] = { "a", "b", "c", "e", "f" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 3);
    assert(edit->offset_sequence_two     == 2);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 3);
    assert(edit->offset_sequence_two     == 3);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 2
    int sequence_one_size = 5;
    const char * test_sequence_one[] = { "a", "b" , "c", "d", "g" };

    int sequence_two_size = 5;
    const char * test_sequence_two[] = { "a", "b", "e", "f", "g" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 2);
    assert(edit->offset_sequence_two     == 1);
    assert(edit->length                  == 2);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 3);
    assert(edit->offset_sequence_two     == 2);
    assert(edit->length                  == 2);

    assert(edit->next                    == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  {
    // edit distance = 2
    // all different test case
    int sequence_one_size = 5;
    const char * test_sequence_one[] = { "a", "b" , "c", "d", "e" };

    int sequence_two_size = 5;
    const char * test_sequence_two[] =  { "f", "g" , "h", "i", "j" };

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script_linear_space test: %d\n", ++test_case_number);

    assert(shortest_edit_script_linear_space((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == -1);
    assert(edit->length                  == 5);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 4);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 5);

    assert(edit->next                    == NULL);
  
    free_shortest_edit_script_linear_space(edit_script);
  }

  return 0;

}