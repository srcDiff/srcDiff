/*
  test_hortest_edit_script.c

  Unit tests for shortest_edit_script.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <shortest_edit_script.h>
#include <shortest_edit_script_private.h>

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
  
  {
    // Stack to Malloc
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous
    struct edit edit = { SESINSERT, 0, 0, 0, NULL, NULL };
    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(&edit)) != NULL);

    assert(copy->operation               == SESINSERT);
    assert(copy->offset_sequence_one     == 0);
    assert(copy->offset_sequence_two     == 0);
    assert(copy->length                  == 0);

    assert(copy->next                    == NULL);
    assert(copy->previous                == NULL);

    free(copy);

  }

  {
    // Stack with stack links to Malloc
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous
    struct edit next;
    struct edit previous;
    struct edit edit = { SESINSERT, 1, 1, 1, &next, &previous };
    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(&edit)) != NULL);

    assert(copy->operation               == SESINSERT);
    assert(copy->offset_sequence_one     == 1);
    assert(copy->offset_sequence_two     == 1);
    assert(copy->length                  == 1);

    assert(copy->next                    == &next);
    assert(copy->previous                == &previous);

    free(copy);

  }

  {
    // Stack with malloc links to Malloc
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous
    struct edit * next;
    struct edit * previous;

    if((next = (struct edit *)malloc(sizeof(struct edit))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    if((previous = (struct edit *)malloc(sizeof(struct edit))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    struct edit edit = { SESDELETE, 3, 12, 48, next, previous };
    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(&edit)) != NULL);

    assert(copy->operation               == SESDELETE);
    assert(copy->offset_sequence_one     == 3);
    assert(copy->offset_sequence_two     == 12);
    assert(copy->length                  == 48);

    assert(copy->next                    == next);
    assert(copy->previous                == previous);

    free(next);
    free(previous);
    free(copy);

  }

  {
    // Malloc with stack links to Malloc
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous
    struct edit next;
    struct edit previous;

    struct edit * edit;

    if((edit = (struct edit *)malloc(sizeof(struct edit))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    edit->operation               = SESINSERT;
    edit->offset_sequence_one     = 100;
    edit->offset_sequence_two     = 50;
    edit->length                  = 20;

    edit->next = &next;
    edit->previous = &previous;

    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(edit)) != NULL);

    assert(copy->operation               == SESINSERT);
    assert(copy->offset_sequence_one     == 100);
    assert(copy->offset_sequence_two     == 50);
    assert(copy->length                  == 20);

    assert(copy->next                    == &next);
    assert(copy->previous                == &previous);

    free(edit);
    free(copy);

  }

  {
    // Stack with malloc links to Malloc
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous
    struct edit * next;
    struct edit * previous;

    if((next = (struct edit *)malloc(sizeof(struct edit))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    if((previous = (struct edit *)malloc(sizeof(struct edit))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    struct edit * edit;

    if((edit = (struct edit *)malloc(sizeof(struct edit))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    edit->operation               = SESINSERT;
    edit->offset_sequence_one     = 200;
    edit->offset_sequence_two     = 100;
    edit->length                  = 50;

    edit->next = next;
    edit->previous = previous;

    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(edit)) != NULL);

    assert(copy->operation               == SESINSERT);
    assert(copy->offset_sequence_one     == 200);
    assert(copy->offset_sequence_two     == 100);
    assert(copy->length                  == 50);

    assert(copy->next                    == next);
    assert(copy->previous                == previous);

    free(next);
    free(previous);
    free(edit);
    free(copy);

  }

  test_case_number = 0;
  
  {
    // NULL
    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(NULL, &edit_script, NULL) == 0);

  } 

  {
    // NULL
    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(NULL, &edit_script, &last_edit) == 0);

    assert(edit_script == NULL);
    assert(last_edit   == NULL);

  }

  {
    // One Edit Stack
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit start_edit = { SESINSERT, 200, 101, 50, NULL, NULL };
    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    assert(edit_script->operation               == SESINSERT);
    assert(edit_script->offset_sequence_one     == 200);
    assert(edit_script->offset_sequence_two     == 100);
    assert(edit_script->length                  == 1);

    assert(edit_script->next                    == NULL);
    assert(edit_script->previous                == NULL);

    assert(edit_script == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // One Edit Malloc
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit * start_edit;

    if((start_edit = (struct edit *)malloc(sizeof(struct edit))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    start_edit->operation               = SESINSERT;
    start_edit->offset_sequence_one     = 200;
    start_edit->offset_sequence_two     = 101;
    start_edit->length                  = 50;

    start_edit->next = NULL;
    start_edit->previous = NULL;

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(start_edit, &edit_script, &last_edit) == 1);

    assert(edit_script->operation               == SESINSERT);
    assert(edit_script->offset_sequence_one     == 200);
    assert(edit_script->offset_sequence_two     == 100);
    assert(edit_script->length                  == 1);

    assert(edit_script->next                    == NULL);
    assert(edit_script->previous                == NULL);

    assert(edit_script == last_edit);
    
    free(start_edit);
    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESInsert Edit Condense one
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESINSERT, 0, 1, 1, NULL, NULL };
    struct edit start_edit =  { SESINSERT, 0, 1, 1, NULL, &first_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    assert(edit_script->operation               == SESINSERT);
    assert(edit_script->offset_sequence_one     == 0);
    assert(edit_script->offset_sequence_two     == 0);
    assert(edit_script->length                  == 2);

    assert(edit_script->next                    == NULL);
    assert(edit_script->previous                == NULL);

    assert(edit_script == last_edit);
    
    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESInsert Edit No Condense
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESINSERT, 0, 1, 1, NULL, NULL };
    struct edit start_edit =  { SESINSERT, 1, 1, 1, NULL, &first_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);
    
    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESDelete Edit Condense
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit start_edit =  { SESDELETE, 2, 0, 1, NULL, &first_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESDelete Edit No Condense
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit start_edit =  { SESDELETE, 1, 0, 1, NULL, &first_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);
    
    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESInsert/SESDelete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESINSERT, 0, 1, 1, NULL, NULL };
    struct edit start_edit =  { SESDELETE, 2, 0, 1, NULL, &first_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESInsert/SESDelete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESINSERT, 0, 1, 1, NULL, NULL };
    struct edit start_edit =  { SESDELETE, 1, 0, 1, NULL, &first_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESDelete/SESInsert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit start_edit =  { SESINSERT, 0, 1, 1, NULL, &first_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESDelete/SESInsert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit start_edit =  { SESINSERT, 1, 1, 1, NULL, &first_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESINSERT, 0, 1, 1, NULL, NULL };
    struct edit middle_edit = { SESINSERT, 0, 1, 1, NULL, &first_edit };
    struct edit start_edit =  { SESINSERT, 0, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 3);

    assert(edit->previous                == NULL);
    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESINSERT, 0, 1, 1, NULL, NULL };
    struct edit middle_edit = { SESINSERT, 0, 1, 1, NULL, &first_edit };
    struct edit start_edit =  { SESINSERT, 1, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESINSERT, 0, 1, 1, NULL, NULL };
    struct edit middle_edit = { SESINSERT, 1, 1, 1, NULL, &first_edit };
    struct edit start_edit =  { SESINSERT, 1, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESINSERT, 0, 1, 1, NULL, NULL };
    struct edit middle_edit = { SESINSERT, 1, 1, 1, NULL, &first_edit };
    struct edit start_edit =  { SESINSERT, 0, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 3);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit middle_edit = { SESDELETE, 2, 0, 1, NULL, &first_edit };
    struct edit start_edit =  { SESDELETE, 3, 0, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 3);

    assert(edit->previous                == NULL);
    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit middle_edit = { SESDELETE, 2, 0, 1, NULL, &first_edit };
    struct edit start_edit =  { SESDELETE, 2, 0, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit middle_edit = { SESDELETE, 1, 0, 1, NULL, &first_edit };
    struct edit start_edit =  { SESDELETE, 2, 0, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit middle_edit = { SESDELETE, 1, 0, 1, NULL, &first_edit };
    struct edit start_edit =  { SESDELETE, 1, 0, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 3);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert/SESDelete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESINSERT, 0, 1, 1, NULL, NULL };
    struct edit middle_edit = { SESDELETE, 1, 0, 1, NULL, &first_edit };
    struct edit start_edit =  { SESINSERT, 0, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 3);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete/SESInsert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit middle_edit = { SESINSERT, 0, 1, 1, NULL, &first_edit };
    struct edit start_edit =  { SESDELETE, 1, 0, 1, NULL, &middle_edit };

    struct edit * edit_script;
    struct edit * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 3);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete/SESInsert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { SESDELETE, 1, 0, 1, NULL, NULL };
    struct edit middle_edit = { SESINSERT, 0, 1, 1, NULL, &first_edit };
    struct edit start_edit =  { SESDELETE, 1, 0, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, NULL) == 3);

    struct edit * edit = edit_script;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == SESINSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    edit = edit->next;

    assert(edit->operation               == SESDELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }
  
  // Shortest edit script tests
  void * shortest_edit_script_functions[] = { shortest_edit_script, shortest_edit_script_linear_space, shortest_edit_script_hybrid, 0 };

  typedef int (*shortest_edit_script_function_pointer)(const void * sequence_one, int sequence_one_size, const void * sequence_two, int sequence_two_size,
    struct edit ** edit_script, 
    int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);

  typedef int (*shortest_edit_script_hybrid_function_pointer)(const void * sequence_one, int sequence_one_size, const void * sequence_two, int sequence_two_size,
    struct edit ** edit_script, 
    int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context,
    int threshold);

  #define test_shortest_edit_script(SHORTEST_EDIT_SCRIPT_FUNCTION, EDIT_DISTANCE) \
    if(SHORTEST_EDIT_SCRIPT_FUNCTION != shortest_edit_script_hybrid) { \
        \
        shortest_edit_script_function_pointer shortest_edit_script_function = SHORTEST_EDIT_SCRIPT_FUNCTION; \
        assert(shortest_edit_script_function((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == EDIT_DISTANCE); \
        \
    } else { \
        shortest_edit_script_hybrid_function_pointer shortest_edit_script_function = SHORTEST_EDIT_SCRIPT_FUNCTION; \
        assert(shortest_edit_script_function((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0, 2) == EDIT_DISTANCE); \
    }

  for(size_t function_pos = 0; shortest_edit_script_functions[function_pos]; ++function_pos) {

      test_case_number = 0;

      {
        // NULL test case
        int sequence_one_size = 0;
        const char ** test_sequence_one = NULL;

        int sequence_two_size = 0;
        const char ** test_sequence_two = NULL;

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 0)

        assert(edit_script == NULL);

        free_shortest_edit_script(edit_script);
      
      }

      {
        // empty test case
        int sequence_one_size = 0;
        const char * test_sequence_one[sequence_one_size];

        int sequence_two_size = 0;
        const char * test_sequence_two[sequence_two_size];

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 0)

        assert(edit_script == NULL);

        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        int sequence_one_size = 1;
        const char * test_sequence_one[] = { "a" };

        int sequence_two_size = 2;
        const char * test_sequence_two[] = { "a", "b" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 1);
        assert(edit->offset_sequence_two     == 1);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);

        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        int sequence_one_size = 2;
        const char * test_sequence_one[] = { "a", "b" };

        int sequence_two_size = 1;
        const char * test_sequence_two[] = { "a" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 1);
        assert(edit->offset_sequence_two     == 1);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 1;
        const char * test_sequence_one[] = { "b" };

        int sequence_two_size = 2;
        const char * test_sequence_two[] = { "a", "b" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 0);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);

        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        int sequence_one_size = 2;
        const char * test_sequence_one[] = { "a", "b" };

        int sequence_two_size = 1;
        const char * test_sequence_two[] = { "b" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 0);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 2;
        const char * test_sequence_one[] = { "a", "c" };

        int sequence_two_size = 3;
        const char * test_sequence_two[] = { "a", "b" , "c"};

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)
     
        struct edit * edit = edit_script;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 1);
        assert(edit->offset_sequence_two     == 1);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
     
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 3;
        const char * test_sequence_one[] = { "a", "b" , "c"};

        int sequence_two_size = 2;
        const char * test_sequence_two[] = { "a", "c" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 1);
        assert(edit->offset_sequence_two     == 1);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 4;
        const char * test_sequence_one[] = { "a", "b" , "c", "d"};

        int sequence_two_size = 3;
        const char * test_sequence_two[] = { "a", "b", "d" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 2);
        assert(edit->offset_sequence_two     == 2);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 3;
        const char * test_sequence_one[] = { "a", "b" , "d"};

        int sequence_two_size = 4;
        const char * test_sequence_two[] = { "a", "b", "c", "d" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 2);
        assert(edit->offset_sequence_two     == 2);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 4;
        const char * test_sequence_two[] = { "a", "b", "c", "e" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 3);
        assert(edit->offset_sequence_two     == 3);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 4;
        const char * test_sequence_one[] = { "a", "b" , "c", "e"};

        int sequence_two_size = 5;
        const char * test_sequence_two[] = { "a", "b", "c", "d", "e" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 3);
        assert(edit->offset_sequence_two     == 3);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 3;
        const char * test_sequence_two[] = { "a", "b", "e" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 2);
        assert(edit->offset_sequence_two     == 2);
        assert(edit->length                  == 2);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 3;
        const char * test_sequence_one[] = { "a", "b", "e" };

        int sequence_two_size = 5;
        const char * test_sequence_two[] =  { "a", "b" , "c", "d", "e" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 2);
        assert(edit->offset_sequence_two     == 2);
        assert(edit->length                  == 2);

        assert(edit->next                    == NULL);
        assert(edit->previous                == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        // second sequence NULL test case
        int sequence_one_size = 5;
        const char * test_sequence_one[] =  { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 0;
        const char ** test_sequence_two = NULL;

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 0);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 5);

        assert(edit->next                    == NULL);

        free_shortest_edit_script(edit_script);

      }

      {
        // edit distance = 1
        // first sequence NULL test case
        int sequence_one_size = 0;
        const char * test_sequence_one = NULL;

        int sequence_two_size = 5;
        const char * test_sequence_two[] =  { "a", "b" , "c", "d", "e" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 0);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 5);

        assert(edit->next                    == NULL);
      
        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        // second sequence empty test case
        int sequence_one_size = 5;
        const char * test_sequence_one[] =  { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 0;
        const char * test_sequence_two[sequence_two_size];

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 0);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 5);

        assert(edit->next                    == NULL);
      
        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        // first sequence empty test case
        int sequence_one_size = 0;
        const char * test_sequence_one[sequence_one_size];

        int sequence_two_size = 5;
        const char * test_sequence_two[] =  { "a", "b" , "c", "d", "e" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 0);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 5);

        assert(edit->next                    == NULL);
      
        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 2
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "f" };

        int sequence_two_size = 5;
        const char * test_sequence_two[] = { "a", "b", "c", "e", "f" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 2)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 3);
        assert(edit->offset_sequence_two     == 3);
        assert(edit->length                  == 1);

        assert(edit->previous                == NULL);

        edit = edit->next;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 4);
        assert(edit->offset_sequence_two     == 3);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 2
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "g" };

        int sequence_two_size = 5;
        const char * test_sequence_two[] = { "a", "b", "e", "f", "g" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 2)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 2);
        assert(edit->offset_sequence_two     == 2);
        assert(edit->length                  == 2);

        assert(edit->previous                == NULL);

        edit = edit->next;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 4);
        assert(edit->offset_sequence_two     == 2);
        assert(edit->length                  == 2);

        assert(edit->next                    == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 2
        // all different test case
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 5;
        const char * test_sequence_two[] =  { "f", "g" , "h", "i", "j" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 2)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 0);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 5);

        assert(edit->previous                == NULL);

        edit = edit->next;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 5);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 5);

        assert(edit->next                    == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 4
        int sequence_one_size = 14;
        const char * test_sequence_one[] = { "a", "b", "c", "d", "e", "d", "f", "g", "h", "d", "i", "i", "i", "j" };

        int sequence_two_size = 9;
        const char * test_sequence_two[] =  { "k", "l", "m", "b", "k", "n", "b", "o", "j" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 4)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 0);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 1);

        assert(edit->previous                == NULL);

        edit = edit->next;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 1);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 3);

        edit = edit->next;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 2);
        assert(edit->offset_sequence_two     == 4);
        assert(edit->length                  == 11);

        edit = edit->next;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 13);
        assert(edit->offset_sequence_two     == 4);
        assert(edit->length                  == 4);

        assert(edit->next                    == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 4
        int sequence_one_size = 22;
        const char * test_sequence_one[] =  { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v" };
        int sequence_two_size = 17;
        const char * test_sequence_two[] =  { "e", "f", "g", "h", "i", "j", "k", "l", "m", "w", "o", "p", "q", "r", "s", "t", "u" };

        struct edit * edit_script;

        fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 4)

        struct edit * edit = edit_script;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 0);
        assert(edit->offset_sequence_two     == 0);
        assert(edit->length                  == 4);

        assert(edit->previous                == NULL);

        edit = edit->next;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 13);
        assert(edit->offset_sequence_two     == 9);
        assert(edit->length                  == 1);

        edit = edit->next;

        assert(edit->operation               == SESINSERT);
        assert(edit->offset_sequence_one     == 14);
        assert(edit->offset_sequence_two     == 9);
        assert(edit->length                  == 1);

        edit = edit->next;

        assert(edit->operation               == SESDELETE);
        assert(edit->offset_sequence_one     == 21);
        assert(edit->offset_sequence_two     == 17);
        assert(edit->length                  == 1);

        assert(edit->next                    == NULL);
      
        free_shortest_edit_script(edit_script);

      }

  }

  return 0;

}