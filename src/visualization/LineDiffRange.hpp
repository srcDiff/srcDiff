/*
  LineDiffRange.hpp

  Compute range of line diffs

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <shortest_edit_script.hpp>

#include <Options.hpp>

#include <string>
#include <vector>
#include <fstream>

#ifndef INCLUDED_LINEDIFFRANGE_HPP
#define INCLUDED_LINEDIFFRANGE_HPP

class LineDiffRange {

private:

  std::string file_one;
  std::string file_two;
  class shortest_edit_script ses;

  std::vector<std::string> lines_one;
  std::vector<std::string> lines_two;

  const char * url;

  OPTION_TYPE options;

public:

  LineDiffRange(std::string file_one, std::string file_two, const char * url, OPTION_TYPE options);

  ~LineDiffRange();

  std::string & get_file_one();
  std::string & get_file_two();
  unsigned int get_length_file_one();
  unsigned int get_length_file_two();

  edit * get_line_diff();

  bool is_no_white_space_diff();

  std::string get_line_diff_range();

  void create_line_diff();

  static std::vector<std::string> read_local_file(const char * file);

 #ifdef SVN
  static std::vector<std::string> read_svn_file(const char * file);
#endif

};

int line_compare(const void * line_one, const void * line_two, const void * context);

const void * line_accessor(int position, const void * lines, const void * context);


#endif
