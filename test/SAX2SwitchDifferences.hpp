/*
  SAX2SwitchDifferences.hpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#ifndef INCLUDED_SAX2SWITCHDIFFERENCES_HPP
#define INCLUDED_SAX2SWITCHDIFFERENCES_HPP

#include <vector>
#include <libxml/parser.h>
#include <libxml/xmlwriter.h>

struct source_switch {

  bool in_old_diff;
  bool exited_old;
  std::vector<xmlNode *> old_diff_nodes; 
  xmlTextWriterPtr writer;
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
