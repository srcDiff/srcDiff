// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script_common.c
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <shortest_edit_script.h>
#include <shortest_edit_script_private.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
  Free the memory in a shortest edit script.

  Parameter edit_script The shortest edit script to free
*/
void free_shortest_edit_script(struct edit_t * edit_script) {

  // free memory
  while(edit_script != NULL) {

    // set next edit
    struct edit_t * temp_edit = edit_script;
    edit_script = edit_script->next;

    // free edit
    free(temp_edit);

  }

}

/*
  Make a compact edit script from the found edits.

  Parameter last_edit            The last edit found
  Parameter edit_script          The shortest edit script

  Returns Then number of edits or an error code (-1 malloc) 
*/
int make_edit_script(struct edit_t * start_edit, struct edit_t ** edit_script, struct edit_t ** last_edit) {

  struct edit_t * current_edit = start_edit;

  if(edit_script) (*edit_script) = NULL;
  if(last_edit)   (*last_edit)   = NULL;

  // holds the length_one of the short edit script
  int distance = 0;

  // check not NULL
  if(current_edit == NULL)
    return distance;

  current_edit->next = NULL;

  // find first edit in shortest edit script
  while(current_edit->previous != NULL) {

    current_edit->previous->next = current_edit;
    current_edit = current_edit->previous;

  }

  // copy first edit
  if(((*edit_script) = copy_edit(current_edit)) == NULL)
    return -1;

  current_edit = (*edit_script);

  // condense edit script
  while(current_edit != NULL) {

    // one more compact edit
    ++distance;

    // find same continous edits

    // condense insert edit
    if(current_edit->operation == SES_INSERT) {

      current_edit->length_two = 1;
      while(current_edit->next != NULL
            && (current_edit->operation == current_edit->next->operation)
            && (current_edit->offset_one == current_edit->next->offset_one)) {

        // add adjacent edit
        current_edit->next = current_edit->next->next;

        // update length_one
        ++current_edit->length_two;

      }
    }

    // condense delete
    else
  
      current_edit->length_one = 1;
      while(current_edit->next != NULL
            && (current_edit->operation == current_edit->next->operation)
            && ((current_edit->offset_one + current_edit->length_one) == current_edit->next->offset_one)) {

        // add adjacent edit
        current_edit->next = current_edit->next->next;

        // update length_one
        ++current_edit->length_one;

      }

    // copy next edit
    if(current_edit->next != NULL) {

      struct edit_t * next;
      if((next = copy_edit(current_edit->next)) == NULL) {

        // free allocated edit
        current_edit->next = NULL;
        free_shortest_edit_script(*edit_script);


        // no script on error
        (*edit_script) = NULL;

        return -1;

      }

      current_edit->next = next;

      // reattach with copied edit
      current_edit->next->previous = current_edit;

    } else {

      if(last_edit) (*last_edit) = current_edit;

    }

    // correct offset
    if(current_edit->operation == SES_DELETE) {
      --current_edit->offset_one;
    }
    else {
      --current_edit->offset_two;
    }

    current_edit = current_edit->next;

  }

  return distance;

}

int merge_sequential_edits(struct edit_t ** edit_script) {

  int edit_distance = 0;
  struct edit_t * current_edit = *edit_script;

  // condense edit script
  while(current_edit != NULL) {

    ++edit_distance;

    // condense insert edit
    if(current_edit->operation == SES_INSERT) {

      while(current_edit->next != NULL
            && (current_edit->operation == current_edit->next->operation)
            && (current_edit->offset_one == current_edit->next->offset_one)) {

        // update length
        current_edit->length_two += current_edit->next->length_two;

        // save edit for deletion
        struct edit_t * save_edit = current_edit->next;

        // add adjacent edit
        current_edit->next = current_edit->next->next;
        if(current_edit->next) current_edit->next->previous = current_edit;

        // delete edit
        free(save_edit);

      }

    // condense delete
    } else {

      while(current_edit->next != NULL
            && (current_edit->operation == current_edit->next->operation)
            && ((current_edit->offset_one + current_edit->length_one) == current_edit->next->offset_one)) {

        // update length
        current_edit->length_one += current_edit->next->length_one;

        // save edit for deletion
        struct edit_t * save_edit = current_edit->next;

        // add adjacent edit
        current_edit->next = current_edit->next->next;
        if(current_edit->next) current_edit->next->previous = current_edit;

        // delete edit
        free(save_edit);

      }

    }

    current_edit = current_edit->next;

  }

  return edit_distance;

}

/*
  Copy a node from the heap.

  Parameter edit          Edit to copy

  Returns The copied edit or NULL if failed
*/
struct edit_t * copy_edit(struct edit_t * edit) {

  struct edit_t * new_edit;
  if((new_edit = (struct edit_t *)malloc(sizeof(struct edit_t))) == NULL)
    return NULL;

  // copy contents
  new_edit->operation = edit->operation;

  new_edit->offset_one = edit->offset_one;
  new_edit->length_one = edit->length_one;

  new_edit->offset_two = edit->offset_two;
  new_edit->length_two = edit->length_two;

  new_edit->next = edit->next;
  new_edit->previous = edit->previous;

  return new_edit;
}

int is_change(const struct edit_t * edit_script) {

  return edit_script->operation == SES_DELETE && edit_script->next != NULL && edit_script->next->operation == SES_INSERT
    && (edit_script->offset_one + edit_script->length_one) == edit_script->next->offset_one;

}
