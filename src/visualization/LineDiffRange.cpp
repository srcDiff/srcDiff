/*
  LineDiffRange.cpp

  Compute range of line diffs

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <LineDiffRange.hpp>

#include <shortest_edit_script.h>
#include <srcDiffUtility.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <URIStream.hpp>
#ifdef SVN
#include <svn_io.hpp>
#endif

LineDiffRange::LineDiffRange(std::string file_one, std::string file_two, const char * url, OPTION_TYPE options)
  : file_one(file_one), file_two(file_two), ses(line_compare, line_accessor, NULL), url(url), options(options) {}

LineDiffRange::~LineDiffRange() {

}

std::string & LineDiffRange::get_file_one() {

  return file_one;

}

std::string & LineDiffRange::get_file_two() {

  return file_two;

}
unsigned int LineDiffRange::get_length_file_one() {

  return lines_one.size();

}

unsigned int LineDiffRange::get_length_file_two() {

  return lines_two.size();

}

edit * LineDiffRange::get_line_diff() {

  return ses.get_script();

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

std::vector<std::string> LineDiffRange::read_local_file(const char * file) {

  std::vector<std::string> lines;

  URIStream stream(file);

  char * line;
  while((line = stream.readline())) {

    lines.push_back(line);

  }

  return lines;

}

#ifdef SVN
std::vector<std::string> LineDiffRange::read_svn_file(const char * file) {

  std::vector<std::string> lines;

  if(file == 0 || file[0] == 0) return lines;

  svn_context * context = (svn_context *)svnReadOpen(file);

  URIStream stream(context);

  char * line;
  while((line = stream.readline())) {

    lines.push_back(line);

  }

  return lines;

}
#endif

std::string LineDiffRange::get_line_diff_range() {

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

void LineDiffRange::create_line_diff() {

#ifdef SVN
  if(!isoption(options, OPTION_SVN)) {
#endif
    lines_one = read_local_file(file_one.c_str());
    lines_two = read_local_file(file_two.c_str());
#ifdef SVN
  } else {

    svn_ra_session_t * session;
    apr_pool_t * pool;
    svn_session_create(url, &session, &pool);
    lines_one = read_svn_file(file_one.c_str());
    lines_two = read_svn_file(file_two.c_str());
    svn_session_destroy(session, pool);

  }
#endif
  
  int distance = ses.compute(&lines_one, lines_one.size(), &lines_two, lines_two.size());

  if(distance < 0) {

    fprintf(stderr, "Error with files %s:%s", file_one.c_str(), file_two.c_str());

    exit(1);

  }

}

static std::string remove_white_space(std::string & source) {

  std::string dest;

  for(unsigned int i = 0; i < source.size(); ++i)
    if(!isspace(source[i]))
       dest += source[i];

  return dest;

}

bool LineDiffRange::is_no_white_space_diff() {


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

        std::string old_line = remove_white_space(lines_one.at(edits->offset_sequence_one + i));
        std::string new_line = remove_white_space(lines_two.at(edits->next->offset_sequence_two + j));

        if(old_line != new_line)
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
