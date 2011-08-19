/*
  diff_trace_simple.cpp

  Take two source code files and compute the difference using
  mba, then form the absolute XPath using SAX.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

// includes
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

#include <libxml/parserInternals.h>

#include "SAX2DiffTraceSimple.hpp"

/*
  Index into the structure containing the lines, and return the line at the 
  specified index
*/
const void * line_index(const void *s, int idx, void *context) {
  std::vector<char *> & lines = *(std::vector<char *> *)s;
  return lines[idx];
}

/*
  Compare two lines for equality
*/
int line_compare(const void * e1, const void * e2, void * context) {
  char * str1 = (char *)e1;
  char * str2 = (char *)e2;

  return strcmp(str1, str2);
}

/*
  Main method of program
*/
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

  // read first file into vector
  std::ifstream file1;
  file1.open(argv[1]);

  getline(file1, *buffer);
  while(!file1.eof()) {
    lines1.push_back((char *)buffer->c_str());
    buffer = new std::string();
    getline(file1, *buffer);
  }
  file1.close();

  // read second file into vector
  std::ifstream file2;
  file2.open(argv[2]);

  getline(file2, *buffer);
  while(!file2.eof()) {
    lines2.push_back((char *)buffer->c_str());
    buffer = new std::string();
    getline(file2, *buffer);
  }
  file2.close();

  // Compute the difference
  if ((d = diff(&lines1, 0, lines1.size(), &lines2, 0, lines2.size(), line_index, line_compare, NULL, 0, ses, &sn, NULL)) == -1) {
    //MMNO(errno);
    return 1;
    }

  // count the column differences between lines
  std::vector<struct diff_edit> columnoffsets;
  for (i = 0; i < sn; i++) {
    struct diff_edit *e = (diff_edit *)varray_get(ses, i);
    
    // check if a change
    if(((i + 1) < sn) && ((e->op == DIFF_INSERT) || (e->op == DIFF_DELETE))) {
      struct diff_edit *enext = (diff_edit *)varray_get(ses, (i + 1));
      if((enext->op == DIFF_INSERT) || (enext->op == DIFF_DELETE)) {
	
	// find offset between lines in each file for change start
	int column = 0;
	struct diff_edit diff;
	for(; column < strlen(lines1[e->off]) && lines1[e->off][column] == lines2[enext->off][column]; ++column);
	diff.off = column;

	// put one on for add and another for delete
	columnoffsets.push_back(diff);
	columnoffsets.push_back(diff);

        //fprintf(stderr, "Line1:%s Line2: %s Start:%d End1:%d End2:%d\n", lines1[e->off + e->len - 1], lines2[enext->off + enext->len - 1],column, columnone + 1, columntwo + 1);

	// skip next already processed
	++i;
	continue;
      }
      else {
        // single diff start after white space
        int column = 0;
        struct diff_edit diff;

        // remove beginning white space
        if(e->op == DIFF_INSERT) {

          for(; column < strlen(lines2[e->off]) && (lines2[e->off][column] == ' ' || lines2[e->off][column] == '\t'); ++column);

        } else if(e->op == DIFF_DELETE) {

          for(; column < strlen(lines1[e->off]) && (lines1[e->off][column] == ' ' || lines1[e->off][column] == '\t'); ++column);

        }

        diff.off = column;
        columnoffsets.push_back(diff);

      }
    } 
    else {
      // single diff start after white space
      int column = 0;
      struct diff_edit diff;

      // remove beginning white space
      if(e->op == DIFF_INSERT) {

        for(; column < strlen(lines2[e->off]) && (lines2[e->off][column] == ' ' || lines2[e->off][column] == '\t'); ++column);

      } else if(e->op == DIFF_DELETE) {

        for(; column < strlen(lines1[e->off]) && (lines1[e->off][column] == ' ' || lines1[e->off][column] == '\t'); ++column);

      }

      diff.off = column;
      columnoffsets.push_back(diff);
    }
  }
  
  // Create context for first file
  xmlParserCtxtPtr ctxt = xmlCreateURLParserCtxt(argv[3], XML_PARSE_COMPACT);
  
  // First file is the deletes
  //fprintf(stdout, "Deletes:\n");

  // Create SAX object and add to context
  SAX2DiffTraceSimple tracer(sn, ses, columnoffsets, DIFF_DELETE);
  ctxt->userData = &tracer;
  
  // set up the SAX parser and add to context
  xmlSAXHandler sax = SAX2DiffTraceSimple::factory();
  ctxt->sax = &sax;
  
  // process the document
  xmlParseDocument(ctxt);
  
  // null the SAX parser so can be freed
  ctxt->sax = NULL;

  // free the context
  xmlFreeParserCtxt(ctxt);
  
  // Create context for second file
  ctxt = xmlCreateURLParserCtxt(argv[4], XML_PARSE_COMPACT);

  // Second file is the inserts
  //fprintf(stdout, "\nInserts:\n");

  // Create SAX object and add to context
  tracer = SAX2DiffTraceSimple(sn, ses, columnoffsets, DIFF_INSERT);
  ctxt->userData = &tracer;
  
  // set up the SAX parser and add to context
  sax = SAX2DiffTraceSimple::factory();
  ctxt->sax = &sax;
  
  // process the document
  xmlParseDocument(ctxt);
  
  // null the SAX parser so can be freed
  ctxt->sax = NULL;
    
  // free the context
  xmlFreeParserCtxt(ctxt);
  
  return 0;
}

