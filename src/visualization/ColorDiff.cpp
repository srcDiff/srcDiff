/*
  colordiff.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <LineDiffRange.hpp>
#include <shortest_edit_script.h>

#include <ColorDiff.hpp>

ColorDiff::ColorDiff(const std::string & colordiff_file, const std::string & directory, const std::string & version, const OPTION_TYPE & options) 
  : first(true), options(options) {

  if(colordiff_file != "-")
    outfile = new std::ofstream(colordiff_file.c_str());
  else
    outfile = &std::cout;
}

ColorDiff::~ColorDiff() {

  if(first) {

    color_diff::output_start_document(*outfile);
    first = false;
  
  }

  color_diff::output_end_document(*outfile);

  if(outfile != &std::cout) {

    ((std::ofstream *)outfile)->close();
    delete outfile;

  }
}

// create the ctxt
xmlParserCtxtPtr createURLParserCtxt(const char * srcdiff) {

  xmlParserCtxtPtr ctxt = xmlCreateMemoryParserCtxt(srcdiff, strlen(srcdiff));
  //xmlCtxtUseOptionsInternal(ctxt, XML_PARSE_COMPACT, NULL);

  if (ctxt == NULL) {

    // report error
    xmlErrorPtr ep = xmlGetLastError();
    fprintf(stderr, "%s: %s", "ExtractsrcML", ep->message);
    exit(1);
  }

  return ctxt;
}

// parse the document
void parseDocument(xmlParserCtxtPtr ctxt) {

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
