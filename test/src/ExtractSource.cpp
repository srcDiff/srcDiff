/*
  ExtractSource.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include <libxml/parserInternals.h>
#include "SAX2ExtractSource.hpp"

// forward declarations
static xmlParserCtxtPtr createURLParserCtxt(const char * infile);
static void parseDocument(xmlParserCtxtPtr ctxt);

int main(int argc, char * argv[]) {

  char * srcML_file;
  int diff;
  if(argc < 2) {

    fprintf(stderr, "Usage: %s xmlfile operation\n", "ExtractSource");
    return 1;
  }

  if(argc < 3) {

    srcML_file = (char *)"/dev/stdin";
    diff = atoi(argv[1]);
  }
  else {

    srcML_file = argv[1];
    diff = atoi(argv[2]);
  }

  // create the ctxt
  xmlParserCtxtPtr ctxt = createURLParserCtxt(srcML_file);

  // setup sax handler
  xmlSAXHandler sax = factory();
  ctxt->sax = &sax;

  struct source_diff data = { diff,  new std::vector<int>()};

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
    fprintf(stderr, "%s: %s", "ElementInfo", ep->message);
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

    fprintf(stderr, "%s: %s in '%s'\n", "ElementInfo", partmsg, ep->file);
    exit(1);
  }

}
