/*
  diff_match_patch_srcML.cpp

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
#include <libxml/parserInternals.h>

#include "SAX2DiffTrace.hpp"

/*
  Main method of program
*/
int main(int argc, char **argv) {

  // Create context for first file
  xmlParserCtxtPtr ctxt = xmlCreateURLParserCtxt(argv[1], XML_PARSE_COMPACT);

  // Create SAX object and add to context
  SAX2DiffTrace tracer;

  ctxt->userData = &tracer;
  
  // set up the SAX parser and add to context
  xmlSAXHandler sax = SAX2DiffTrace::factory();
  ctxt->sax = &sax;
  
  // process the document
  xmlParseDocument(ctxt);
  
  // null the SAX parser so can be freed
  ctxt->sax = NULL;

  // free the context
  xmlFreeParserCtxt(ctxt);
  
  return 0;
}

