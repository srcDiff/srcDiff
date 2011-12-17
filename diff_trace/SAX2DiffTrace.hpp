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

struct diff {

  int operation;
  int level;

};

struct element {

  std::string name;
  std::string prefix;
  std::string uri;

  std::string signature_old;
  std::string signature_new;

  std::map<std::string, int> children_old;
  std::map<std::string, int> children_new;

};

// class definition
class SAX2DiffTrace {

 private:

  xmlSAXHandler * sax;

  bool output;
  bool collect;
  unsigned int collect_node_pos;

  std::vector<int> missed_diff_types;  
  std::vector<std::vector<element> > missed_diffs;  

  std::string filename_old;
  std::string filename_new;

  bool is_delete;
  bool is_insert;


  bool collect_text_delete;
  bool collect_text_insert;

  std::vector<element> elements;
  std::vector<diff> diff_stack;

 public:

  // constructor

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
  friend void output_diff(SAX2DiffTrace & tracer);

};


#endif
