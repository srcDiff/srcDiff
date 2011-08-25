/*
  SAX2SwitchDifferences.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include "SAX2SwitchDifferences.hpp"

xmlSAXHandler factory() {

  xmlSAXHandler sax = { 0 };

  sax.initialized    = XML_SAX2_MAGIC;

  sax.startDocument = &startDocument;
  sax.endDocument = &endDocument;

  sax.startElementNs = &startElementNs;
  sax.endElementNs = &endElementNs;

  sax.characters = &characters;
  sax.comment = &comment;

  return sax;
}

void startDocument(void* ctx) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}

void endDocument(void* ctx) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}

void startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
		     int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
		     const xmlChar** attributes) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_switch * data = (source_switch *)ctxt->_private;

  xmlTextWriterStartElement(data->writer, localname);

  int index;
  for(int i = 0, index = 0; i < nb_attributes; ++i, index += 5) {

    int end = attributes[index + 4] - attributes[index + 3];
    const char * value = strndup((const char *)attributes[index + 3], end);

    xmlTextWriterWriteAttribute(data->writer, attributes[index], (const xmlChar *)value);
  }
}

void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_switch * data = (source_switch *)ctxt->_private;

}

void characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_switch * data = (source_switch *)ctxt->_private;

  /*
  if(!data->in_diff) {

    for(int i = 0; i < len; ++i)
      fprintf(stdout, "%c", (char)ch[i]);
      }
  */

}

void comments(void* ctx, const xmlChar* ch) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}
