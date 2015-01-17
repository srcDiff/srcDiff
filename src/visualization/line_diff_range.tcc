/*
  line_diff_range.tcc

  Compute range of line diffs

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <uri_stream.hpp>

template<class T>
line_diff_range<T>::line_diff_range(const std::string & file_one, const std::string & file_two, const T * input)
  : file_one(file_one), file_two(file_two), ses(line_compare, line_accessor, NULL), input(input) {}

template<class T>
line_diff_range<T>::~line_diff_range() {}

template<class T>
const std::string & line_diff_range<T>::get_file_one() const {

  return file_one;

}

template<class T>
const std::string & line_diff_range<T>::get_file_two() const {

  return file_two;

}

template<class T>
unsigned int line_diff_range<T>::get_length_file_one() const {

  return lines_one.size();

}

template<class T>
unsigned int line_diff_range<T>::get_length_file_two() const {

  return lines_two.size();

}

template<class T>
edit * line_diff_range<T>::get_line_diff() {

  return ses.get_script();

}

template<class T>
std::vector<std::string> line_diff_range<T>::read_file(const T * input, const char * file) {

  std::vector<std::string> lines;

  if(file == 0 || file[0] == 0) return lines;

  typename T::input_context * context = input->open(file);

  uri_stream<T> stream(context);

  char * line;
  while((line = stream.readline())) {

    lines.push_back(line);

  }

  return lines;

}

template<class T>
std::string line_diff_range<T>::get_line_diff_range() {

  std::string diff;

  for(edit * edits = ses.get_script(); edits; edits = edits->next) {

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

  return diff;

}

template<class T>
void line_diff_range<T>::create_line_diff() {

  lines_one = read_file(input, file_one.c_str());
  lines_two = read_file(input, file_two.c_str());

  int distance = ses.compute(&lines_one, lines_one.size(), &lines_two, lines_two.size());

  if(distance < 0) {

    fprintf(stderr, "Error with files %s:%s", file_one.c_str(), file_two.c_str());

    exit(1);

  }

}

template<class T>
bool line_diff_range<T>::is_no_white_space_diff() {


  for(edit * edits = ses.get_script(); edits; edits = edits->next) {

    if(is_change(edits)) {

      int i, j;
      for(i = 0, j = 0; i < edits->length && j < edits->next->length;) {

        if(remove_white_space(lines_one.at(edits->offset_sequence_one + i)) == "" 
           && remove_white_space(lines_two.at(edits->next->offset_sequence_two + j)) == "") {

          ++i;
          ++j;
          continue;

        }

        if(remove_white_space(lines_one.at(edits->offset_sequence_one + i)) == "") {

          ++i;
          continue;

        }

        if(remove_white_space(lines_two.at(edits->next->offset_sequence_two + j)) == "") {

          ++j;
          continue;

        }

        std::string original_line = remove_white_space(lines_one.at(edits->offset_sequence_one + i));
        std::string modified_line = remove_white_space(lines_two.at(edits->next->offset_sequence_two + j));

        if(original_line != modified_line)
          return true;

        ++i;
        ++j;

      }

      if(i != edits->length || j != edits->next->length)
         return true;

      edits = edits->next;
      continue;

    }

    switch(edits->operation) {

    case SESINSERT:

      for(int i = 0; i < edits->length; ++i)
        if(remove_white_space(lines_two.at(edits->offset_sequence_two + i)) != "")
          return true;

      break;

    case SESDELETE:

      for(int i = 0; i < edits->length; ++i)
        if(remove_white_space(lines_one.at(edits->offset_sequence_one + i)) != "")
          return true;

      break;

    }


  }

  return false;

}
