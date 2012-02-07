/*
  Shortest_edit_script_t.c

  Unit tests for shortest_edit_script.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include "shortest_edit_script.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int line_compare(const void * line_one, const void * line_two, const void * context) {

  std::string & line1 = *(std::string *)line_one;
  std::string & line2 = *(std::string *)line_two;

  return line1 != line2;
}

const void * line_accessor(int position, const void * lines, const void * context) {

  std::string & line = ((std::vector *)lines)->at(position);

  return line;
}

void get_line_diffs(const char * file_one, const char * file_two) {

  std::string lines1;
  std::string lines2;

  int distance = shortest_edit_script();


}
