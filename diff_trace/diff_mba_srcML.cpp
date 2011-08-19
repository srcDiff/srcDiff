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

#include <mba/diff.h>
#include <mba/msgno.h>

const void * line_index(const void *s, int idx, void *context) {
  std::vector<char *> & lines = *(std::vector<char *> *)s;
  return lines[idx];
}

int line_compare(const void * e1, const void * e2, void * context) {
  char * str1 = (char *)e1;
  char * str2 = (char *)e2;

  return strcmp(str1, str2);
}

int main(int argc, char **argv) {
  // variables
  int status = 1;

  std::vector<char *> lines1;
  std::vector<char *> lines2;

  int n, m, d;
  int sn, i;
  struct varray *ses = varray_new(sizeof(struct diff_edit), NULL);

  int count = 512;
  std::string * buffer = new std::string();
  char * buf = (char *)malloc(512);

  std::ifstream file1;
  file1.open(argv[1]);

  getline(file1, *buffer);
  while(!file1.eof()) {
    lines1.push_back((char *)buffer->c_str());
    buffer = new std::string();
    getline(file1, *buffer);
  }
  file1.close();

  std::ifstream file2;
  file2.open(argv[2]);

  getline(file2, *buffer);
  while(!file2.eof()) {
    lines2.push_back((char *)buffer->c_str());
    buffer = new std::string();
    getline(file2, *buffer);
  }
  file2.close();

  {
    if ((d = diff(&lines1, 0, lines1.size(), &lines2, 0, lines2.size(), line_index, line_compare, NULL, 0, ses, &sn, NULL)) == -1) {
      //MMNO(errno);
      return 1;
    }

    for (i = 0; i < sn; i++) {
      struct diff_edit *e = (diff_edit *)varray_get(ses, i);

      if(((i + 1) < sn) && ((e->op == DIFF_INSERT) || (e->op == DIFF_DELETE))) {
	  struct diff_edit *enext = (diff_edit *)varray_get(ses, (i + 1));
	  if((enext->op == DIFF_INSERT) || (enext->op == DIFF_DELETE)) {
	    if(e->op == DIFF_INSERT) {
	      puts("#ifdef SRCDIFF");
	      for(int j = 0; j < e->len; ++j)
		puts(lines2[e->off + j]);
	      puts("#else // SRCDIFF");
	      for(int j = 0; j < enext->len; ++j)
		puts(lines1[enext->off + j]);
	      puts("#endif // SRCDIFF");
	    }
	    else {
	      puts("#ifndef SRCDIFF");
	      for(int j = 0; j < e->len; ++j)
		puts(lines1[e->off + j]);
	      puts("#else // SRCDIFF");
	      for(int j = 0; j < enext->len; ++j)
		puts(lines2[enext->off + j]);
	      puts("#endif // SRCDIFF");
	    }

	    ++i;
	    continue;
	  }
      }

      switch (e->op) {
      case DIFF_MATCH:
	//for(int j = 0; j < e->len; ++j)
        //puts(lines1[e->off + j]);
	break;
      case DIFF_INSERT:
	puts("#ifdef SRCDIFF");
	for(int j = 0; j < e->len; ++j)
	  puts(lines2[e->off + j]);
	puts("#endif // SRCDIFF");
	break;
      case DIFF_DELETE:
	puts("#ifndef SRCDIFF");
	for(int j = 0; j < e->len; ++j)
	  puts(lines1[e->off + j]);
	puts("#endif // ! SRCDIFF");
	break;
        }
    }

    status = 0;
  }

  return 0;
}

