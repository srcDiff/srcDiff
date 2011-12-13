/*
  colordiff.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <libxml/parserInternals.h>
#include "SAX2ColorDiff.hpp"
#include "shortest_edit_script.h"

// forward declarations
static xmlParserCtxtPtr createURLParserCtxt(const char * infile);
static void parseDocument(xmlParserCtxtPtr ctxt);

int main(int argc, char * argv[]) {

  char * srcdiff_file;
  if(argc < 2) {

    srcdiff_file = (char *)"-";
  }
  else {

    srcdiff_file = argv[1];
  }

  std::vector<bool> lines_old;
  std::vector<bool> lines_new;

  // make 1 offset
  lines_old.push_back(false);
  lines_new.push_back(false);

  std::fstream in(argv[2]);

  char operation;
  while(in >> operation) {

    unsigned int start;
    in >> start;

    char temp;
    in >> temp;

    unsigned int end;
    in >> end;

    if(operation == 'd') {

      while(lines_old.size() < start)
        lines_old.push_back(false);

      while(lines_old.size() <= end)
        lines_old.push_back(true);

    } else {

      while(lines_new.size() < start)
        lines_new.push_back(false);

      while(lines_new.size() <= end)
        lines_new.push_back(true);

    }

  }

  //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, lines_old.size());
  //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, lines_new.size());

  // create the ctxt
  xmlParserCtxtPtr ctxt = createURLParserCtxt(srcdiff_file);

  // setup sax handler
  xmlSAXHandler sax = factory();
  ctxt->sax = &sax;

  std::vector<int> * stack = new std::vector<int>();
  stack->push_back(COMMON);

  struct source_diff data = { 1, 1, stack, lines_old, lines_new };

  ctxt->_private = &data;

  // process the document
  parseDocument(ctxt);

  // local variable, do not want xmlFreeParserCtxt to free
  ctxt->sax = NULL;

  // all done with parsing
  xmlFreeParserCtxt(ctxt);

  return 0;
}

// create the ctxt
static xmlParserCtxtPtr createURLParserCtxt(const char * infile) {

  xmlParserCtxtPtr ctxt = xmlCreateURLParserCtxt(infile, XML_PARSE_COMPACT);
  if (ctxt == NULL) {

    // report error
    xmlErrorPtr ep = xmlGetLastError();
    fprintf(stderr, "%s: %s", "ExtractsrcML", ep->message);
    exit(1);
  }

  return ctxt;
}

// parse the document
static void parseDocument(xmlParserCtxtPtr ctxt) {

  // process the document
  int status;
  if ((status = xmlParseDocument(ctxt)) == -1) {

    xmlErrorPtr ep = xmlCtxtGetLastError(ctxt);

    // report error
    char* partmsg = strdup(ep->message);
    partmsg[strlen(partmsg) - 1] = '\0';

    fprintf(stderr, "%s: %s in '%s'\n", "ExtractsrcML", partmsg, ep->file);
    exit(1);
  }

}
