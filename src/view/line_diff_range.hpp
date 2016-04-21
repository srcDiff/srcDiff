/*
  line_diff_range.hpp

  Compute range of line diffs

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#ifndef INCLUDED_LINE_DIFF_RANGE_HPP
#define INCLUDED_LINE_DIFF_RANGE_HPP

#include <shortest_edit_script.hpp>

#include <string>
#include <vector>

template<class T>
class line_diff_range {

private:

  const std::string & file_one;
  const std::string & file_two;
  class shortest_edit_script ses;

  std::vector<std::string> lines_one;
  std::vector<std::string> lines_two;

  const T * input;

public:

  line_diff_range(const std::string & file_one, const std::string & file_two, const T * input);

  ~line_diff_range();

  const std::string & get_file_one() const;
  const std::string & get_file_two() const;
  unsigned int get_length_file_one() const;
  unsigned int get_length_file_two() const;

  edit * get_line_diff();

  bool is_no_white_space_diff();

  std::string get_line_diff_range();

  void create_line_diff();

  static void read_file(const T * input, const char * file, std::vector<std::string> & lines);

};

int line_compare(const void * line_one, const void * line_two, const void * context);

const void * line_accessor(int position, const void * lines, const void * context);

std::string remove_white_space(std::string & source);

#include <line_diff_range.tcc>

#endif
