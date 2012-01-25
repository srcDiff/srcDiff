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
  int text_num;
  bool output_text;

};

struct element {

  std::string name;
  std::string prefix;
  std::string uri;

  std::vector<std::string> signature_path_old;
  std::vector<std::string> signature_path_new;

  std::vector<std::string> signature_name_old;
  std::vector<std::string> signature_name_new;

  std::map<std::string, int> children;
  std::map<std::string, int> children_old;
  std::map<std::string, int> children_new;

};

// class definition
class SAX2DiffTrace {

 private:

  long & options;

  xmlSAXHandler * sax;

  bool output;
  bool wait;
  bool collect;
  unsigned int offset_pos;
  unsigned int collect_node_pos;

  bool collect_name;
  bool wait_name;

  std::vector<int> missed_diff_types;  
  std::vector<std::vector<element> > missed_diffs;  

  std::vector<std::vector<int> > signature_path_pos_old;  
  std::vector<std::vector<int> > signature_path_offsets_old;  
  std::vector<std::vector<std::string> > signature_path_old;  

  std::vector<std::vector<int> > signature_path_pos_new;  
  std::vector<std::vector<int> > signature_path_offsets_new;  
  std::vector<std::vector<std::string> > signature_path_new;  

  std::string filename_old;
  std::string filename_new;

  bool is_delete;
  bool is_insert;

  bool collect_text;
  bool collect_text_delete;
  bool collect_text_insert;

  std::vector<element> elements;
  std::vector<diff> diff_stack;

 public:

  // constructor
  SAX2DiffTrace(long & options);

  // sax factory
  static xmlSAXHandler factory();

  // start document
  static void startDocument(void * ctx);

  // end document
  static void endDocument(void * ctx);

  static bool is_wait(const char * name, const char * prefix);

  static bool is_type(const char * name, const char * prefix);

  static bool is_collect(SAX2DiffTrace & tracer, const char * name, const char * prefix);

  static bool is_end_wait(SAX2DiffTrace & tracer, const char * name, const char * prefix, const char * context);
 
  static bool is_end_collect(const char * name, const char * prefix, const char * context);

  static void form_paths(SAX2DiffTrace & tracer);

  static void end_collect(SAX2DiffTrace & tracer);

  static void update_offsets(SAX2DiffTrace & tracer, int offset, int operation);

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
