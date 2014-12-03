/*
  SAX2ColorDiff.hpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#ifndef INCLUDED_SAX2BASHVIEW_HPP
#define INCLUDED_SAX2BASHVIEW_HPP

#include <vector>
#include <fstream>
#include <string>

#include <libxml/parser.h>

class bash_view {

private:

  std::vector<int> diff_stack;

  std::ostream & output;

public:

  bash_view(std::ostream & output) : output(output) {}

  static xmlSAXHandler factory();

  static void startDocument(void* ctx);

  static void endDocument(void* ctx);


  static void startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                      int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                      const xmlChar** attributes);

  static void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);

  static void characters(void* ctx, const xmlChar* ch, int len);

  static void comment(void* ctx, const xmlChar* ch);

};

#endif
