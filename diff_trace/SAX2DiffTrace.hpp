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

// class definition
class SAX2DiffTrace {

 private:

  bool output;
  bool readconstruct;
  bool readsignature;
  bool waitoutput;
  std::string function;

  const char* filename;

  xmlSAXHandler * sax;

  std::vector<std::string> start_stack;
  std::vector<std::string> elements;
  std::vector<std::string> labels;
  std::vector<int> lastcharacterdepth;
  std::vector<std::map<std::string, int> > blocks;

 public:

  // constructor
  SAX2DiffTrace();

  // sax factory
  static xmlSAXHandler factory();

  // start document
  static void startDocument(void * ctx);

  // end document
  static void endDocument(void * ctx);

  // characters
  static void characters(void* ctx, const xmlChar* ch, int len);

  // startElement
  static void startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                    int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                    const xmlChar** attributes);


  // endElement
  static void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);

  // comments
  static void comments(void* ctx, const xmlChar* ch);


  // helper methods

  // output a difference formatted as an Xpath
  friend void output_diff(SAX2DiffTrace * pstate, int numelements);

};


#endif
