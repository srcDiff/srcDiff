/*
  SAX2ColorDiff.hpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#ifndef INCLUDED_SAX2COLORDIFF_HPP
#define INCLUDED_SAX2COLORDIFF_HPP

#include <libxml/parser.h>

struct source_diff {

  unsigned int line_old;
  unsigned int line_new;
  std::vector<int> * in_diff;

  std::vector<bool> & lines_old;
  std::vector<bool> & lines_new;
};

xmlSAXHandler factory();

void startDocument(void* ctx);

void endDocument(void* ctx);


void startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                    int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                    const xmlChar** attributes);

void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);

void characters(void* ctx, const xmlChar* ch, int len);

void comments(void* ctx, const xmlChar* ch);

#endif
