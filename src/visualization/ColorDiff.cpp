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
#include <LineDiffRange.hpp>
#include <shortest_edit_script.h>
#include <SAX2ColorDiff.hpp>

#include <ColorDiff.hpp>

// forward declarations
static xmlParserCtxtPtr createURLParserCtxt(const char * srcdiff);
static void parseDocument(xmlParserCtxtPtr ctxt);

ColorDiff::ColorDiff(std::string colordiff_file, std::string directory, std::string version, std::string css, OPTION_TYPE & options) 
  : first(true), css_url(css), options(options) {

  if(colordiff_file != "-")
    outfile = new std::ofstream(colordiff_file.c_str());
  else
    outfile = &std::cout;
}

ColorDiff::~ColorDiff() {

  if(first) {

    color_diff::output_start_document(*outfile, css_url);
    first = false;
  
  }

  color_diff::output_end_document(*outfile);

  if(outfile != &std::cout) {

    ((std::ofstream *)outfile)->close();
    delete outfile;

  }
}

int ColorDiff::colorize(const char * srcdiff, LineDiffRange & line_diff_range) {

  unsigned int size_old = line_diff_range.get_length_file_one();
  unsigned int size_new = line_diff_range.get_length_file_two();

  std::string stream = line_diff_range.get_line_diff_range();

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

  if(isoption(options, OPTION_ARCHIVE) && !isoption(options, OPTION_OUTPUTSAME) && lines_old.size() == 0 && lines_new.size() == 0)
    return 0;

  if(isoption(options, OPTION_ARCHIVE) && !isoption(options, OPTION_OUTPUTPURE) && (size_old == 0 || size_new == 0))
    return 0;

  while(lines_old.size() <= size_old)
        lines_old.push_back(false);

  while(lines_new.size() <= size_new)
        lines_new.push_back(false);

  if(first) {

    color_diff::output_start_document(*outfile, css_url);
    first = false;

  }

  // create the ctxt
  xmlParserCtxtPtr ctxt = createURLParserCtxt(srcdiff);

  // setup sax handler
  xmlSAXHandler sax = color_diff::factory();
  ctxt->sax = &sax;

  std::vector<int> stack = std::vector<int>();
  stack.push_back(SESCOMMON);

  struct color_diff::source_diff data = { 1, 1, &stack, lines_old, lines_new, line_diff_range.get_file_one(), line_diff_range.get_file_two()
                              , *outfile, options, "", false, false, false };

  ctxt->_private = &data;

  parseDocument(ctxt);

  // local variable, do not want xmlFreeParserCtxt to free
  ctxt->sax = NULL;

  // all done with parsing
  xmlFreeParserCtxt(ctxt);

  return 0;
}

// create the ctxt
static xmlParserCtxtPtr createURLParserCtxt(const char * srcdiff) {

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
