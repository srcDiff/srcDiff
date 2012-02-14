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

class LineDiffRange {

private:

  std::string file_one;
  std::string file_two;

  edit * edit_script;

  std::vector<std::string> lines_one;
  std::vector<std::string> lines_two;

public:

  LineDiffRange(std::string file_one, std::string file_two);

  ~LineDiffRange();

  std::string get_line_diff_range();

  void get_line_diff();


  static std::vector<std::string> read_file(const char * file);

};

int line_compare(const void * line_one, const void * line_two, const void * context);

const void * line_accessor(int position, const void * lines, const void * context);


#endif
