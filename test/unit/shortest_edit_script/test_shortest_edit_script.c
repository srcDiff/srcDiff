// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_shortest_edit_script.c
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
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
    // operation, offset_one, offset_two, length, next, previous
    struct edit_t edit = { SES_INSERT, 0, 0, 0, 0, NULL, NULL };
    struct edit_t * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(&edit)) != NULL);

    assert(copy->operation      == SES_INSERT);
    assert(copy->offset_one     == 0);
    assert(copy->offset_two     == 0);
    assert(copy->length         == 0);

    assert(copy->next           == NULL);
    assert(copy->previous       == NULL);

    free(copy);
  }

  {
    // Stack with stack links to Malloc
    // operation, offset_one, offset_two, length, next, previous
    struct edit_t next;
    struct edit_t previous;
    struct edit_t edit = { SES_INSERT, 1, 0, 1, 1, &next, &previous };
    struct edit_t * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(&edit)) != NULL);

    assert(copy->operation      == SES_INSERT);
    assert(copy->offset_one     == 1);
    assert(copy->length_one     == 0);
    assert(copy->offset_two     == 1);
    assert(copy->length_two     == 1);

    assert(copy->next           == &next);
    assert(copy->previous       == &previous);

    free(copy);

  }

  {
    // Stack with malloc links to Malloc
    // operation, offset_one, offset_two, length, next, previous
    struct edit_t * next;
    struct edit_t * previous;

    if((next = (struct edit_t *)malloc(sizeof(struct edit_t))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    if((previous = (struct edit_t *)malloc(sizeof(struct edit_t))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    struct edit_t edit = { SES_DELETE, 3, 12, 48, 0, next, previous };
    struct edit_t * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(&edit)) != NULL);

    assert(copy->operation      == SES_DELETE);
    assert(copy->offset_one     == 3);
    assert(copy->length_one     == 48);
    assert(copy->offset_two     == 12);
    assert(copy->length_two     == 0);

    assert(copy->next           == next);
    assert(copy->previous       == previous);

    free(next);
    free(previous);
    free(copy);

  }

  {
    // Malloc with stack links to Malloc
    // operation, offset_one, offset_two, length, next, previous
    struct edit_t next;
    struct edit_t previous;

    struct edit_t * edit;

    if((edit = (struct edit_t *)malloc(sizeof(struct edit_t))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    edit->operation      = SES_INSERT;
    edit->offset_one     = 100;
    edit->length_one     = 0;
    edit->offset_two     = 50;
    edit->length_two     = 20;

    edit->next = &next;
    edit->previous = &previous;

    struct edit_t * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(edit)) != NULL);

    assert(copy->operation      == SES_INSERT);
    assert(copy->offset_one     == 100);
    assert(copy->length_one     == 0);
    assert(copy->offset_two     == 50);
    assert(copy->length_two     == 20);

    assert(copy->next           == &next);
    assert(copy->previous       == &previous);

    free(edit);
    free(copy);

  }

  {
    // Stack with malloc links to Malloc
    // operation, offset_one, offset_two, length, next, previous
    struct edit_t * next;
    struct edit_t * previous;

    if((next = (struct edit_t *)malloc(sizeof(struct edit_t))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    if((previous = (struct edit_t *)malloc(sizeof(struct edit_t))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    struct edit_t * edit;

    if((edit = (struct edit_t *)malloc(sizeof(struct edit_t))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    edit->operation      = SES_INSERT;
    edit->offset_one     = 200;
    edit->length_one     = 0;
    edit->offset_two     = 100;
    edit->length_two     = 50;

    edit->next = next;
    edit->previous = previous;

    struct edit_t * copy;

    fprintf(stderr, "copy_edit test: %d\n", ++test_case_number);

    assert((copy = copy_edit(edit)) != NULL);

    assert(copy->operation      == SES_INSERT);
    assert(copy->offset_one     == 200);
    assert(copy->length_one     == 0);
    assert(copy->offset_two     == 100);
    assert(copy->length_two     == 50);

    assert(copy->next           == next);
    assert(copy->previous       == previous);

    free(next);
    free(previous);
    free(edit);
    free(copy);

  }

  test_case_number = 0;
  
  {
    // NULL
    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(NULL, &edit_script, NULL) == 0);

  } 

  {
    // NULL
    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(NULL, &edit_script, &last_edit) == 0);

    assert(edit_script == NULL);
    assert(last_edit   == NULL);

  }

  {
    // One Edit Stack
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t start_edit = { SES_INSERT, 200, 0, 101, 50, NULL, NULL };
    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    assert(edit_script->operation      == SES_INSERT);
    assert(edit_script->offset_one     == 200);
    assert(edit_script->length_one     == 0);
    assert(edit_script->offset_two     == 100);
    assert(edit_script->length_two     == 1);

    assert(edit_script->next           == NULL);
    assert(edit_script->previous       == NULL);

    assert(edit_script == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // One Edit Malloc
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t * start_edit;

    if((start_edit = (struct edit_t *)malloc(sizeof(struct edit_t))) == NULL) {

      fprintf(stderr, "Malloc Error");
      return -1;
    }

    start_edit->operation      = SES_INSERT;
    start_edit->offset_one     = 200;
    start_edit->length_one     = 0;
    start_edit->offset_two     = 101;
    start_edit->length_two     = 50;

    start_edit->next = NULL;
    start_edit->previous = NULL;

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(start_edit, &edit_script, &last_edit) == 1);

    assert(edit_script->operation      == SES_INSERT);
    assert(edit_script->offset_one     == 200);
    assert(edit_script->length_one     == 0);
    assert(edit_script->offset_two     == 100);
    assert(edit_script->length_two     == 1);

    assert(edit_script->next           == NULL);
    assert(edit_script->previous       == NULL);

    assert(edit_script == last_edit);
    
    free(start_edit);
    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESInsert Edit Condense one
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit = { SES_INSERT, 0, 0, 1, 1, NULL, NULL };
    struct edit_t start_edit = { SES_INSERT, 0, 0, 1, 1, NULL, &first_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    assert(edit_script->operation      == SES_INSERT);
    assert(edit_script->offset_one     == 0);
    assert(edit_script->length_one     == 0);
    assert(edit_script->offset_two     == 0);
    assert(edit_script->length_two     == 2);

    assert(edit_script->next           == NULL);
    assert(edit_script->previous       == NULL);

    assert(edit_script == last_edit);
    
    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESInsert Edit No Condense
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit = { SES_INSERT, 0, 0, 1, 1, NULL, NULL };
    struct edit_t start_edit = { SES_INSERT, 1, 0, 1, 1, NULL, &first_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 1);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->next           == NULL);

    assert(edit == last_edit);
    
    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESDelete Edit Condense
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t start_edit = { SES_DELETE, 2, 0, 1, 0, NULL, &first_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 2);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->next           == NULL);
    assert(edit->previous       == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESDelete Edit No Condense
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t start_edit = { SES_DELETE, 1, 0, 1, 0, NULL, &first_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->next           == NULL);

    assert(edit == last_edit);
    
    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESInsert/SESDelete Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit = { SES_INSERT, 0, 0, 1, 1, NULL, NULL };
    struct edit_t start_edit = { SES_DELETE, 2, 0, 1, 0, NULL, &first_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 1);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESInsert/SESDelete Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit = { SES_INSERT, 0, 0, 1, 1, NULL, NULL };
    struct edit_t start_edit = { SES_DELETE, 1, 0, 1, 0, NULL, &first_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESDelete/SESInsert Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t start_edit = { SES_INSERT, 0, 0, 1, 1, NULL, &first_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Two SESDelete/SESInsert Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t start_edit = { SES_INSERT, 1, 0, 1, 1, NULL, &first_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 1);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_INSERT, 0, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 0, 0, 1, 1, NULL, &first_edit };
    struct edit_t start_edit  = { SES_INSERT, 0, 0, 1, 1, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 3);

    assert(edit->previous       == NULL);
    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_INSERT, 0, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 0, 0, 1, 1, NULL, &first_edit };
    struct edit_t start_edit  = { SES_INSERT, 1, 0, 1, 1, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_INSERT);
    assert(edit->length_one     == 0);
    assert(edit->offset_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 2);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 1);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_INSERT, 0, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 1, 0, 1, 1, NULL, &first_edit };
    struct edit_t start_edit  = { SES_INSERT, 1, 0, 1, 1, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 1);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 2);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_INSERT, 0, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 1, 0, 1, 1, NULL, &first_edit };
    struct edit_t start_edit  = { SES_INSERT, 0, 0, 1, 1, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 3);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 1);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t middle_edit = { SES_DELETE, 2, 0, 1, 0, NULL, &first_edit };
    struct edit_t start_edit  = { SES_DELETE, 3, 0, 1, 0, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 1);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 3);
    assert(edit->offset_two     == 0);
    assert(edit->length_one     == 0);

    assert(edit->previous       == NULL);
    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t middle_edit = { SES_DELETE, 2, 0, 1, 0, NULL, &first_edit };
    struct edit_t start_edit  = { SES_DELETE, 2, 0, 1, 0, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 2);
    assert(edit->offset_two     == 0);
    assert(edit->length_one     == 0);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 1);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t middle_edit = { SES_DELETE, 1, 0, 1, 0, NULL, &first_edit };
    struct edit_t start_edit  = { SES_DELETE, 2, 0, 1, 0, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 2);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 2);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t middle_edit = { SES_DELETE, 1, 0, 1, 0, NULL, &first_edit };
    struct edit_t start_edit  = { SES_DELETE, 1, 0, 1, 0, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 3);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESInsert/SESDelete Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_INSERT, 0, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_DELETE, 1, 0, 1, 0, NULL, &first_edit };
    struct edit_t start_edit  = { SES_INSERT, 0, 0, 1, 1, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 3);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete/SESInsert Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 0, 0, 1, 1, NULL, &first_edit };
    struct edit_t start_edit  = { SES_DELETE, 1, 0, 1, 0, NULL, &middle_edit };

    struct edit_t * edit_script;
    struct edit_t * last_edit;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, &last_edit) == 3);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->next           == NULL);

    assert(edit == last_edit);

    free_shortest_edit_script(edit_script);

  }

  {
    // Three SESDelete/SESInsert Edit
    // operation, offset_one, offset_two, length, next, previous

    struct edit_t first_edit  = { SES_DELETE, 1, 0, 1, 0, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 0, 0, 1, 1, NULL, &first_edit };
    struct edit_t start_edit  = { SES_DELETE, 1, 0, 1, 0, NULL, &middle_edit };

    struct edit_t * edit_script;

    fprintf(stderr, "make_edit_script test: %d\n", ++test_case_number);

    assert(make_edit_script(&start_edit, &edit_script, NULL) == 3);

    struct edit_t * edit = edit_script;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->previous       == NULL);

    edit = edit->next;

    assert(edit->operation      == SES_INSERT);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 0);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 1);

    edit = edit->next;

    assert(edit->operation      == SES_DELETE);
    assert(edit->offset_one     == 0);
    assert(edit->length_one     == 1);
    assert(edit->offset_two     == 0);
    assert(edit->length_two     == 0);

    assert(edit->next           == NULL);

    free_shortest_edit_script(edit_script);

  }
  
  // Shortest edit script tests
  void * shortest_edit_script_functions[] = { shortest_edit_script, shortest_edit_script_linear_space, shortest_edit_script_hybrid, 0 };

  typedef int (*shortest_edit_script_function_pointer)(const void * sequence_one, int sequence_one_size, const void * sequence_two, int sequence_two_size,
    struct edit_t ** edit_script, 
    int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context);

  typedef int (*shortest_edit_script_hybrid_function_pointer)(const void * sequence_one, int sequence_one_size, const void * sequence_two, int sequence_two_size,
    struct edit_t ** edit_script, 
    int compare(const void *, const void *, const void *), const void * accessor(int index, const void *, const void *), const void * context,
    int threshold);

  #define test_shortest_edit_script(SHORTEST_EDIT_SCRIPT_FUNCTION, EDIT_DISTANCE, TEST_DESCRIPTION)                                                                                                     \
    const char * shortest_edit_script_function_name = SHORTEST_EDIT_SCRIPT_FUNCTION == shortest_edit_script ? "shortest_edit_script" :                                                             \
    SHORTEST_EDIT_SCRIPT_FUNCTION == shortest_edit_script_linear_space ? "shortest_edit_script_linear_space" : "shortest_edit_script_hybrid";                                                      \
                                                                                                                                                                                                   \
    fprintf(stderr, "Function: %s\tDescription: %s\n", shortest_edit_script_function_name, TEST_DESCRIPTION);                                                                                                      \
                                                                                                                                                                                                   \
    if(SHORTEST_EDIT_SCRIPT_FUNCTION != shortest_edit_script_hybrid) {                                                                                                                             \
                                                                                                                                                                                                   \
        shortest_edit_script_function_pointer shortest_edit_script_function = SHORTEST_EDIT_SCRIPT_FUNCTION;                                                                                       \
        assert(shortest_edit_script_function((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0) == EDIT_DISTANCE);    \
                                                                                                                                                                                                   \
    } else {                                                                                                                                                                                       \
                                                                                                                                                                                                   \
        shortest_edit_script_hybrid_function_pointer shortest_edit_script_function = SHORTEST_EDIT_SCRIPT_FUNCTION;                                                                                \
        assert(shortest_edit_script_function((void *)test_sequence_one, sequence_one_size, (void *)test_sequence_two, sequence_two_size, &edit_script, compare, accessor, 0, 2) == EDIT_DISTANCE); \
    } 

    size_t function_pos;
    for(function_pos = 0; shortest_edit_script_functions[function_pos]; ++function_pos) {

      {
        // NULL test case
        int sequence_one_size = 0;
        const char ** test_sequence_one = NULL;

        int sequence_two_size = 0;
        const char ** test_sequence_two = NULL;

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 0, "Test 1")

        assert(edit_script == NULL);

        free_shortest_edit_script(edit_script);
      
      }

      {
        // empty test case
        int sequence_one_size = 0;
        const char * test_sequence_one[sequence_one_size];

        int sequence_two_size = 0;
        const char * test_sequence_two[sequence_two_size];

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 0, "Test 2")

        assert(edit_script == NULL);

        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        int sequence_one_size = 1;
        const char * test_sequence_one[] = { "a" };

        int sequence_two_size = 2;
        const char * test_sequence_two[] = { "a", "b" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 3")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 1);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 1);
        assert(edit->length_two     == 1);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);

        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        int sequence_one_size = 2;
        const char * test_sequence_one[] = { "a", "b" };

        int sequence_two_size = 1;
        const char * test_sequence_two[] = { "a" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 4")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 1);
        assert(edit->length_one     == 1);
        assert(edit->offset_two     == 1);
        assert(edit->length_two     == 0);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 1;
        const char * test_sequence_one[] = { "b" };

        int sequence_two_size = 2;
        const char * test_sequence_two[] = { "a", "b" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 5")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 0);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 1);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);

        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        int sequence_one_size = 2;
        const char * test_sequence_one[] = { "a", "b" };

        int sequence_two_size = 1;
        const char * test_sequence_two[] = { "b" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 6")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 0);
        assert(edit->length_one     == 1);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 0);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 2;
        const char * test_sequence_one[] = { "a", "c" };

        int sequence_two_size = 3;
        const char * test_sequence_two[] = { "a", "b" , "c"};

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 7")
     
        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 1);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 1);
        assert(edit->length_two     == 1);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
     
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 3;
        const char * test_sequence_one[] = { "a", "b" , "c"};

        int sequence_two_size = 2;
        const char * test_sequence_two[] = { "a", "c" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 8")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 1);
        assert(edit->length_one     == 1);
        assert(edit->offset_two     == 1);
        assert(edit->length_two     == 0);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 4;
        const char * test_sequence_one[] = { "a", "b" , "c", "d"};

        int sequence_two_size = 3;
        const char * test_sequence_two[] = { "a", "b", "d" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 9")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 2);
        assert(edit->length_one     == 1);
        assert(edit->offset_two     == 2);
        assert(edit->length_two     == 0);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 3;
        const char * test_sequence_one[] = { "a", "b" , "d"};

        int sequence_two_size = 4;
        const char * test_sequence_two[] = { "a", "b", "c", "d" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 10")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 2);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 2);
        assert(edit->length_two     == 1);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 4;
        const char * test_sequence_two[] = { "a", "b", "c", "e" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 11")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 3);
        assert(edit->length_one     == 1);
        assert(edit->offset_two     == 3);
        assert(edit->length_two     == 0);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 4;
        const char * test_sequence_one[] = { "a", "b" , "c", "e"};

        int sequence_two_size = 5;
        const char * test_sequence_two[] = { "a", "b", "c", "d", "e" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 12")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 3);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 3);
        assert(edit->length_two     == 1);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 3;
        const char * test_sequence_two[] = { "a", "b", "e" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 13")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 2);
        assert(edit->length_one     == 2);
        assert(edit->offset_two     == 2);
        assert(edit->length_two     == 0);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        int sequence_one_size = 3;
        const char * test_sequence_one[] = { "a", "b", "e" };

        int sequence_two_size = 5;
        const char * test_sequence_two[] =  { "a", "b" , "c", "d", "e" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 14")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 2);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 2);
        assert(edit->length_two     == 2);

        assert(edit->next           == NULL);
        assert(edit->previous       == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 1
        // second sequence NULL test case
        int sequence_one_size = 5;
        const char * test_sequence_one[] =  { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 0;
        const char ** test_sequence_two = NULL;

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 15")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 0);
        assert(edit->length_one     == 5);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 0);

        assert(edit->next           == NULL);

        free_shortest_edit_script(edit_script);

      }

      {
        // edit distance = 1
        // first sequence NULL test case
        int sequence_one_size = 0;
        const char * test_sequence_one = NULL;

        int sequence_two_size = 5;
        const char * test_sequence_two[] =  { "a", "b" , "c", "d", "e" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 16")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 0);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 5);

        assert(edit->next           == NULL);
      
        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        // second sequence empty test case
        int sequence_one_size = 5;
        const char * test_sequence_one[] =  { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 0;
        const char * test_sequence_two[sequence_two_size];

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 17")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 0);
        assert(edit->length_one     == 5);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 0);

        assert(edit->next           == NULL);
      
        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 1
        // first sequence empty test case
        int sequence_one_size = 0;
        const char * test_sequence_one[sequence_one_size];

        int sequence_two_size = 5;
        const char * test_sequence_two[] =  { "a", "b" , "c", "d", "e" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 1, "Test 18")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 0);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 5);

        assert(edit->next           == NULL);
      
        free_shortest_edit_script(edit_script);
      
      }

      {
        // edit distance = 2
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "f" };

        int sequence_two_size = 5;
        const char * test_sequence_two[] = { "a", "b", "c", "e", "f" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 2, "Test 19")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 3);
        assert(edit->length_one     == 1);
        assert(edit->offset_two     == 3);
        assert(edit->length_two     == 0);

        assert(edit->previous                == NULL);

        edit = edit->next;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 4);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 3);
        assert(edit->length_two     == 1);

        assert(edit->next           == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 2
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "g" };

        int sequence_two_size = 5;
        const char * test_sequence_two[] = { "a", "b", "e", "f", "g" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 2, "Test 20")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 2);
        assert(edit->length_one     == 2);
        assert(edit->offset_two     == 2);
        assert(edit->length_two     == 0);

        assert(edit->previous       == NULL);

        edit = edit->next;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 4);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 2);
        assert(edit->length_two     == 2);

        assert(edit->next           == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 2
        // all different test case
        int sequence_one_size = 5;
        const char * test_sequence_one[] = { "a", "b" , "c", "d", "e" };

        int sequence_two_size = 5;
        const char * test_sequence_two[] =  { "f", "g" , "h", "i", "j" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 2, "Test 21")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 0);
        assert(edit->length_one     == 5);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 0);

        assert(edit->previous       == NULL);

        edit = edit->next;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 5);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 5);

        assert(edit->next           == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 4
        int sequence_one_size = 14;
        const char * test_sequence_one[] = { "a", "b", "c", "d", "e", "d", "f", "g", "h", "d", "i", "i", "i", "j" };

        int sequence_two_size = 9;
        const char * test_sequence_two[] =  { "k", "l", "m", "b", "k", "n", "b", "o", "j" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 4, "Test 22")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 0);
        assert(edit->length_one     == 1);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 0);

        assert(edit->previous       == NULL);

        edit = edit->next;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 1);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 3);

        edit = edit->next;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 2);
        assert(edit->length_one     == 11);
        assert(edit->offset_two     == 4);
        assert(edit->length_two     == 0);

        edit = edit->next;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 13);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 4);
        assert(edit->length_two     == 4);

        assert(edit->next           == NULL);
      
        free_shortest_edit_script(edit_script);
      }

      {
        // edit distance = 4
        int sequence_one_size = 22;
        const char * test_sequence_one[] =  { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v" };
        int sequence_two_size = 17;
        const char * test_sequence_two[] =  { "e", "f", "g", "h", "i", "j", "k", "l", "m", "w", "o", "p", "q", "r", "s", "t", "u" };

        struct edit_t * edit_script;

        test_shortest_edit_script(shortest_edit_script_functions[function_pos], 4, "Test 23")

        struct edit_t * edit = edit_script;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 0);
        assert(edit->length_one     == 4);
        assert(edit->offset_two     == 0);
        assert(edit->length_two     == 0);

        assert(edit->previous       == NULL);

        edit = edit->next;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 13);
        assert(edit->length_one     == 1);
        assert(edit->offset_two     == 9);
        assert(edit->length_two     == 0);

        edit = edit->next;

        assert(edit->operation      == SES_INSERT);
        assert(edit->offset_one     == 14);
        assert(edit->length_one     == 0);
        assert(edit->offset_two     == 9);
        assert(edit->length_two     == 1);

        edit = edit->next;

        assert(edit->operation      == SES_DELETE);
        assert(edit->offset_one     == 21);
        assert(edit->length_one     == 1);
        assert(edit->offset_two     == 17);
        assert(edit->length_two     == 0);

        assert(edit->next           == NULL);
      
        free_shortest_edit_script(edit_script);

      }

  }

  #undef test_shortest_edit_script

  return 0;

}
