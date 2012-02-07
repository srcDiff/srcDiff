/*
  LineDiffRange.cpp

  Compute range of line diffs

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include "LineDiffRange.hpp"

#include "shortest_edit_script.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

int line_compare(const void * line_one, const void * line_two, const void * context) {

  std::string & line1 = *(std::string *)line_one;
  std::string & line2 = *(std::string *)line_two;

  return line1 != line2;
}

const void * line_accessor(int position, const void * lines, const void * context) {

  std::string & line = ((std::vector<std::string> *)lines)->at(position);

  return &line;
}

std::vector<std::string> read_file(const char * file) {

  std::vector<std::string> lines;

  std::ifstream stream(file);

  std::string line;

  std::getline(stream, line);
  while(!stream.eof()) {

    lines.push_back(line);
    std::getline(stream, line);

  }

  if(line != "") {

    lines.push_back(line);

  }

  return lines;

}

void get_line_diff_range(const char * file_one, const char * file_two) {

  std::vector<std::string> lines1 = read_file(file_one);
  std::vector<std::string> lines2 = read_file(file_two);

  edit * edit_script;

  int distance = shortest_edit_script(lines1.size(), &lines1, lines2.size(), &lines2, line_compare, line_accessor, &edit_script, NULL);

  std::string diff;

  for(edit * edits = edit_script; edits, edits = edits->next) {
    
    if(edits->operation == DELETE) {

      diff += "d" + (edits->offset_sequence_one + 1) + "-" + (edits->offset_sequence_one + edits->length) + "\n";

      
    } else if(edits->operation == INSERT) {

      diff += "i" + (edits->offest_sequence_two + 1) + "-" + (edits->offset_sequence_two + edits->length) + "\n";

    }
    
  }

}


int main(int argc, char * argv[]) {

  get_line_diff_range(argv[1], argv[2]);

  return 0;

}
