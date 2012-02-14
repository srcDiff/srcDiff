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
#include <sstream>
#include <iostream>

LineDiffRange::LineDiffRange(std::string file_one, std::string file_two)
  : file_one(file_one), file_two(file_two), edit_script(NULL) {}

LineDiffRange::~LineDiffRange() {

  if(edit_script != NULL)
    free_shortest_edit_script(edit_script);

}

int line_compare(const void * line_one, const void * line_two, const void * context) {

  std::string & line1 = *(std::string *)line_one;
  std::string & line2 = *(std::string *)line_two;

  return line1 != line2;
}

const void * line_accessor(int position, const void * lines, const void * context) {

  std::string & line = ((std::vector<std::string> *)lines)->at(position);

  return &line;
}

std::vector<std::string> LineDiffRange::read_file(const char * file) {

  std::vector<std::string> lines;

  std::ifstream stream(file);
  if(!stream || stream.eof())
    return lines;

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

std::string LineDiffRange::get_line_diff_range() {

  std::string diff;

  for(edit * edits = edit_script; edits; edits = edits->next) {

    std::stringstream stream;
    
    if(edits->operation == SESDELETE) {

      stream <<  "d";
      stream << (edits->offset_sequence_one + 1);
      stream << "-";
      stream << (edits->offset_sequence_one + edits->length);

    } else if(edits->operation == SESINSERT) {

      stream << "i";
      stream << (edits->offset_sequence_two + 1);
      stream << "-"; 
      stream << (edits->offset_sequence_two + edits->length);

    }

    std::string temp;
    stream >> temp;

    diff += temp + "\n";
    
  }

  free_shortest_edit_script(edit_script);

  return diff;

}

void LineDiffRange::get_line_diff() {

  lines_one = read_file(file_one.c_str());
  lines_two = read_file(file_two.c_str());

  int distance = shortest_edit_script(lines_one.size(), &lines_one, lines_two.size(), &lines_two, line_compare, line_accessor, &edit_script, NULL);

  if(distance < 0) {

    fprintf(stderr, "Error with files %s:%s", file_one.c_str(), file_two.c_str());

    exit(1);

  }

}
