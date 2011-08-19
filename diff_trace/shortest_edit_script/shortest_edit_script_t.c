/*
  Shortest_edit_script_t.c

  Unit tests for shortest_edit_script.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include "shortest_edit_script.h"
#include "shortest_edit_script.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int compare(const void * element_one, const void * element_two) {

  const char * e1 = (const char *)element_one;
  const char * e2 = (const char *)element_two;

  int match = strcmp(e1, e2);
  return match;
}

const void * accessor(int position, const void * sequence) {

  return (((const char **)sequence)[position]);
}

int main(int argc, char * argv[]) {

  int test_case_number = 0;
  
  {
    // Stack to Malloc
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous
    struct edit edit = { INSERT, 0, 0, 0, NULL, NULL };
    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(&edit)) != NULL);

    assert(copy->operation               == INSERT);
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
    struct edit edit = { INSERT, 1, 1, 1, &next, &previous };
    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(&edit)) != NULL);

    assert(copy->operation               == INSERT);
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

    struct edit edit = { DELETE, 3, 12, 48, next, previous };
    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(&edit)) != NULL);

    assert(copy->operation               == DELETE);
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

    edit->operation               = INSERT;
    edit->offset_sequence_one     = 100;
    edit->offset_sequence_two     = 50;
    edit->length                  = 20;

    edit->next = &next;
    edit->previous = &previous;

    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(edit)) != NULL);

    assert(copy->operation               == INSERT);
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

    edit->operation               = INSERT;
    edit->offset_sequence_one     = 200;
    edit->offset_sequence_two     = 100;
    edit->length                  = 50;

    edit->next = next;
    edit->previous = previous;

    struct edit * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(edit)) != NULL);

    assert(copy->operation               == INSERT);
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

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(NULL, &edit_script) == 0);

  }

  {
    // One Edit Stack
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit last_edit = { INSERT, 201, 101, 50, NULL, NULL };
    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 1);

    assert(edit_script->operation               == INSERT);
    assert(edit_script->offset_sequence_one     == 200);
    assert(edit_script->offset_sequence_two     == 100);
    assert(edit_script->length                  == 1);

    assert(edit_script->next                    == NULL);
    assert(edit_script->previous                == NULL);
    

    free_shortest_edit_script(edit_script);

  }

  {
    // One Edit Malloc
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit * last_edit;

    if((last_edit = (struct edit *)malloc(sizeof(struct edit))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    last_edit->operation               = INSERT;
    last_edit->offset_sequence_one     = 201;
    last_edit->offset_sequence_two     = 101;
    last_edit->length                  = 50;

    last_edit->next = NULL;
    last_edit->previous = NULL;

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(last_edit, &edit_script) == 1);

    assert(edit_script->operation               == INSERT);
    assert(edit_script->offset_sequence_one     == 200);
    assert(edit_script->offset_sequence_two     == 100);
    assert(edit_script->length                  == 1);

    assert(edit_script->next                    == NULL);
    assert(edit_script->previous                == NULL);
    
    free(last_edit);
    free_shortest_edit_script(edit_script);

  }

  {
    // Two Insert Edit Condense one
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { INSERT, 1, 1, 1, NULL, NULL };
    struct edit last_edit =  { INSERT, 1, 1, 1, NULL, &first_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 1);

    assert(edit_script->operation               == INSERT);
    assert(edit_script->offset_sequence_one     == 0);
    assert(edit_script->offset_sequence_two     == 0);
    assert(edit_script->length                  == 2);

    assert(edit_script->next                    == NULL);
    assert(edit_script->previous                == NULL);
    
    free_shortest_edit_script(edit_script);

  }

  {
    // Two Insert Edit No Condense
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { INSERT, 1, 1, 1, NULL, NULL };
    struct edit last_edit =  { INSERT, 2, 1, 1, NULL, &first_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    
    free_shortest_edit_script(edit_script);

  }

  {
    // Two Delete Edit Condense
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { DELETE, 1, 1, 1, NULL, NULL };
    struct edit last_edit =  { DELETE, 2, 1, 1, NULL, &first_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->next                    == NULL);
    assert(edit->previous                == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two Delete Edit No Condense
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { DELETE, 1, 1, 1, NULL, NULL };
    struct edit last_edit =  { DELETE, 1, 1, 1, NULL, &first_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);
    
    free_shortest_edit_script(edit_script);

  }

  {
    // Two Insert/Delete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { INSERT, 1, 1, 1, NULL, NULL };
    struct edit last_edit =  { DELETE, 2, 1, 1, NULL, &first_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two Insert/Delete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { INSERT, 1, 1, 1, NULL, NULL };
    struct edit last_edit =  { DELETE, 1, 1, 1, NULL, &first_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two Delete/Insert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { DELETE, 1, 1, 1, NULL, NULL };
    struct edit last_edit =  { INSERT, 1, 1, 1, NULL, &first_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two Delete/Insert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { DELETE, 1, 1, 1, NULL, NULL };
    struct edit last_edit =  { INSERT, 2, 1, 1, NULL, &first_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Insert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { INSERT, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { INSERT, 1, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { INSERT, 1, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 3);

    assert(edit->previous                == NULL);
    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Insert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { INSERT, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { INSERT, 1, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { INSERT, 2, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Insert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { INSERT, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { INSERT, 2, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { INSERT, 2, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Insert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { INSERT, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { INSERT, 2, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { INSERT, 1, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 3);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Delete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { DELETE, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { DELETE, 2, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { DELETE, 3, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 3);

    assert(edit->previous                == NULL);
    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Delete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { DELETE, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { DELETE, 2, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { DELETE, 2, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Delete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { DELETE, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { DELETE, 1, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { DELETE, 2, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 2);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Delete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { DELETE, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { DELETE, 1, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { DELETE, 1, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 3);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    edit = edit->next;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Insert/Delete Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { INSERT, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { DELETE, 1, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { INSERT, 1, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 3);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three Delete/Insert Edit
    // operation, offset_sequence_one, offset_sequence_two, length, next, previous

    struct edit first_edit = { DELETE, 1, 1, 1, NULL, NULL };
    struct edit middle_edit = { INSERT, 1, 1, 1, NULL, &first_edit };
    struct edit last_edit =  { DELETE, 1, 1, 1, NULL, &middle_edit };

    struct edit * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&last_edit, &edit_script) == 3);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    edit = edit->next;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 1);

    assert(edit->next                    == NULL);

    free_shortest_edit_script(edit_script);

  }
  
  // Shortest edit script tests
  test_case_number = 0;

  {
    // NULL test case
    int sequence_one_size = 0;
    const char ** test_sequence_one = NULL;

    int sequence_two_size = 0;
    const char ** test_sequence_two = NULL;

    struct edit * edit_script;

    fprintf(stderr, "shortest_edit_script test: %d\n", ++test_case_number);

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 0);

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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 0);

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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == -1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == -1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);
 
    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 0);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 1);
    assert(edit->offset_sequence_two     == 0);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 2);
    assert(edit->offset_sequence_two     == 1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 3);
    assert(edit->offset_sequence_two     == 2);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 2);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 2);
    assert(edit->offset_sequence_two     == 1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == -1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == -1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == -1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 1);

    struct edit * edit = edit_script;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == -1);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 3);
    assert(edit->offset_sequence_two     == 2);
    assert(edit->length                  == 1);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 3);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 2);
    assert(edit->offset_sequence_two     == 1);
    assert(edit->length                  == 2);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 3);
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

    assert(shortest_edit_script(sequence_one_size, (void *)test_sequence_one, sequence_two_size, (void *)test_sequence_two, compare, accessor, &edit_script) == 2);

    struct edit * edit = edit_script;

    assert(edit->operation               == DELETE);
    assert(edit->offset_sequence_one     == 0);
    assert(edit->offset_sequence_two     == -1);
    assert(edit->length                  == 5);

    assert(edit->previous                == NULL);

    edit = edit->next;

    assert(edit->operation               == INSERT);
    assert(edit->offset_sequence_one     == 4);
    assert(edit->offset_sequence_two     == 0);
    assert(edit->length                  == 5);

    assert(edit->next                    == NULL);
  
    free_shortest_edit_script(edit_script);
  }

  return 0;
}
