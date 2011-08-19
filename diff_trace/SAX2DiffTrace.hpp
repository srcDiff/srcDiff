/*
  SAX2DiffTrace.hpp

  Holds SAX object and SAX methods for extracting an absolute XPath from
  srcML documents for differences between files

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

// include guard
#ifndef INCLUDED_SAX2DIFFTRACE_HPP
#define INCLUDED_SAX2DIFFTRACE_HPP

// includes
#include <vector>
#include <string>
#include <map>

#include <libxml/parser.h>
#include <libxml/xmlwriter.h>

#include <mba/diff.h>

// class definition
class SAX2DiffTrace {

 private:

  // members
  int DIFF_TYPE;

  int numedits;
  int editpos;
  int linecount;
  int columncount;
  bool output;
  bool readconstruct;
  bool readsignature;
  bool waitoutput;
  std::string function;

  struct varray * diff_edits;
  std::vector<struct diff_edit> column_diffs;
  const char* filename;
  xmlSAXHandler * sax;

  std::vector<std::string> start_stack;
  std::vector<std::string> elements;
  std::vector<std::string> labels;
  std::vector<int> lastcharacterdepth;
  std::vector<std::map<std::string, int> > blocks;

 public:

  // constructor
  SAX2DiffTrace(int nedits, struct varray *ses, std::vector<struct diff_edit> columnoffsets, int difftype);

  // sax factory
  static xmlSAXHandler factory();

  // start document
  static void startDocument(void * ctx);

  // end document
  static void endDocument(void * ctx);

  // characters
  static void characters(void * ctx, const xmlChar* ch, int len);

  // startElement
  static void startElement(void * ctx, const xmlChar * name, const xmlChar ** attrs);

  // endElement
  static void endElement(void * ctx, const xmlChar * name);

  // helper methods
  // get the next difference for processing 
  friend struct diff_edit * get_next_diff(SAX2DiffTrace * pstate);
  // output a difference formatted as an Xpath
  friend void output_diff(SAX2DiffTrace * pstate, int numelements);

};


#endif
