/*
  diff_match_patch_srcML.cpp

  Take two source code files and compute the difference using
  mba
*/


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <errno.h>

#include "shortest_edit_script.h"

const void * line_index(int idx, const void *s) {
  std::vector<const char *> & lines = *(std::vector<const char *> *)s;
  return lines[idx];
}

int line_compare(const void * e1, const void * e2) {
  const char * str1 = (const char *)e1;
  const char * str2 = (const char *)e2;

  return strcmp(str1, str2);
}

int main(int argc, char **argv) {
  // variables
  int status = 1;

  std::vector<const char *> lines1;
  std::vector<const char *> lines2;

  //int count = 512;
  std::string * buffer = new std::string();
  //char * buf = (char *)malloc(512);

  std::ifstream file1;
  file1.open(argv[1]);

  getline(file1, *buffer);
  while(!file1.eof()) {
    lines1.push_back((const char *)buffer->c_str());
    buffer = new std::string();
    getline(file1, *buffer);
  }
  file1.close();

  std::ifstream file2;
  file2.open(argv[2]);

  getline(file2, *buffer);
  while(!file2.eof()) {
    lines2.push_back((const char *)buffer->c_str());
    buffer = new std::string();
    getline(file2, *buffer);
  }
  file2.close();

  {

    struct edit * edit_script;

    int distance = shortest_edit_script(lines1.size(), (void *)&lines1, lines2.size(), (void *)&lines2, line_compare, line_index, &edit_script);

    int length = 0;
    struct edit * e = edit_script;
    for (; e; e = e->next) {
      length += e->length;
    }

    fprintf(stdout, "%d", length);

    status = 0;
    free_shortest_edit_script(edit_script);

  }

  return 0;
}

