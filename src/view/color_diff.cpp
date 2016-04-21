/*
  color_diff.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <shortest_edit_script.h>

#include <color_diff.hpp>

color_diff::color_diff(const std::string & color_diff_file, const std::string & directory, const std::string & version, const OPTION_TYPE & options) 
  : first(true), options(options) {

  if(color_diff_file != "-")
    outfile = new std::ofstream(color_diff_file.c_str());
  else
    outfile = &std::cout;
}

color_diff::~color_diff() {

  if(first) {

    sax2_color_diff::output_start_document(*outfile);
    first = false;
  
  }

  sax2_color_diff::output_end_document(*outfile);

  if(outfile != &std::cout) {

    ((std::ofstream *)outfile)->close();
    delete outfile;

  }
}

// create the ctxt
xmlParserCtxtPtr create_url_parser_ctxt(const char * srcdiff) {

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
void parse_document(xmlParserCtxtPtr ctxt) {

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
