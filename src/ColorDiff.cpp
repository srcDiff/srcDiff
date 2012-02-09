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
#include <libxml/parserInternals.h>
#include "LineDiffRange.hpp"
#include "shortest_edit_script.h"
#include "SAX2ColorDiff.hpp"

#include "srcDiffOutput.hpp"

#include "ColorDiff.hpp"

// forward declarations
static xmlParserCtxtPtr createURLParserCtxt(xmlBuffer * srcdiff);
static void parseDocument(xmlParserCtxtPtr ctxt);

ColorDiff::ColorDiff(xmlBuffer * srcdiff, std::string colordiff_file, std::string directory, std::string version, std::string css
                     , OPTION_TYPE & options) 
  : first(true), srcdiff(srcdiff), css_url(css), options(options) {

  if(colordiff_file != "-")
    outfile = new std::ofstream(colordiff_file.c_str());
  else
    outfile = &std::cout;
}

ColorDiff::~ColorDiff() {

  if(first) {

    output_start_document(*outfile, css_url);
    first = false;
  
  }

  output_end_document(*outfile);

  if(outfile != &std::cout) {

    ((std::ofstream *)outfile)->close();
    delete outfile;

  }
}

xmlBuffer * ColorDiff::getsrcDiffBuffer() {

  return srcdiff;

}

void ColorDiff::setsrcDiffBuffer(xmlBuffer * srcdiff_buffer) {

  srcdiff = srcdiff_buffer;

}

int ColorDiff::colorize(std::string file_one, std::string file_two) {


  unsigned int size_old = 0;
  unsigned int size_new = 0;

  std::string stream = get_line_diff_range(file_one, file_two, size_old, size_new);

  std::vector<bool> lines_old;
  std::vector<bool> lines_new;

  std::stringstream in(stream);

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

  if(isoption(options, OPTION_NESTED) && !isoption(options, OPTION_OUTPUTSAME) && lines_old.size() == 0 && lines_new.size() == 0)
    return 0;

  if(isoption(options, OPTION_NESTED) && !isoption(options, OPTION_OUTPUTPURE) && (size_old == 0 || size_new == 0))
    return 0;

  while(lines_old.size() <= size_old)
        lines_old.push_back(false);

  while(lines_new.size() <= size_new)
        lines_new.push_back(false);

  if(first) {

    output_start_document(*outfile, css_url);
    first = false;

  }

  // create the ctxt
  xmlParserCtxtPtr ctxt = createURLParserCtxt(srcdiff);

  // setup sax handler
  xmlSAXHandler sax = factory();
  ctxt->sax = &sax;

  std::vector<int> stack = std::vector<int>();
  stack.push_back(SESCOMMON);

  struct source_diff data = { 1, 1, &stack, lines_old, lines_new, file_one, file_two, *outfile };

  ctxt->_private = &data;

  parseDocument(ctxt);

  // local variable, do not want xmlFreeParserCtxt to free
  ctxt->sax = NULL;

  // all done with parsing
  xmlFreeParserCtxt(ctxt);

  return 0;
}

// create the ctxt
static xmlParserCtxtPtr createURLParserCtxt(xmlBuffer * srcdiff) {

  xmlParserCtxtPtr ctxt = xmlCreateMemoryParserCtxt((const char *)xmlBufferContent(srcdiff), srcdiff->use);
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
