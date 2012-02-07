/*
  LineDiffRange.cpp

  Compute range of line diffs

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include "shortest_edit_script.h"

#include <string>
#include <vector>
#include <fstream>

int line_compare(const void * line_one, const void * line_two, const void * context) {

  std::string & line1 = *(std::string *)line_one;
  std::string & line2 = *(std::string *)line_two;

  return line1 != line2;
}

const void * line_accessor(int position, const void * lines, const void * context) {

  std::string & line = ((std::vector<std::string> *)lines)->at(position);

  return line;
}

std::vector<std::string> read_file(const char file) {

  std::vector<std::string> lines;

  std::ifstream stream(file);

  std::string line;

  std::getline(stream, line);
  while(!stream.eof()) {
    lines.push_back(line);

    std::getline(stream, line);

  }

  if(lines != "") {

    lines.push_back(line);

  }

  return lines;

}

void get_line_diff_range(const char * file_one, const char * file_two) {

  std::vector<std::string> lines1;
  std::vector<std::string> lines2;

  int distance = shortest_edit_script();


}
