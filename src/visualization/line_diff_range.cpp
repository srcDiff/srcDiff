/*
  line_diff_range.cpp

  Compute range of line diffs

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <line_diff_range.hpp>

#include <string>
#include <vector>

int line_compare(const void * line_one, const void * line_two, const void * context) {

  std::string & line1 = *(std::string *)line_one;
  std::string & line2 = *(std::string *)line_two;

  return line1 != line2;
}

const void * line_accessor(int position, const void * lines, const void * context) {

  std::string & line = ((std::vector<std::string> *)lines)->at(position);

  return &line;
}

std::string remove_white_space(std::string & source) {

  std::string dest;

  for(unsigned int i = 0; i < source.size(); ++i)
    if(!isspace(source[i]))
       dest += source[i];

  return dest;

}
