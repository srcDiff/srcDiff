/*
  LineDiffRange.hpp

  Compute range of line diffs

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include "shortest_edit_script.h"

#include <string>
#include <vector>
#include <fstream>

#ifndef INCLUDED_LINEDIFFRANGE_HPP
#define INCLUDED_LINEDIFFRANGE_HPP

int line_compare(const void * line_one, const void * line_two, const void * context);

const void * line_accessor(int position, const void * lines, const void * context);

std::vector<std::string> read_file(const char * file);

std::string get_line_diff_range(std::string file_one, std::string file_two, int & lines_old, int & lines_new);

#endif
