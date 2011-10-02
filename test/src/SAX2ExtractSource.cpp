/*
  SAX2ExtractSource.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include "SAX2ExtractSource.hpp"

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

  if(strcmp((const char *)localname, "escape") == 0) {

    int index;
    for(int i = 0, index = 0; i < nb_attributes; ++i, index += 5) {

    }

  }

}

void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if((data->op == DELETE && strcmp((const char *)localname, "new") == 0)
     || (data->op == INSERT && strcmp((const char *)localname, "old") == 0)) {

    data->in_diff = !data->in_diff;
  }
}

void characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(!data->in_diff) {

    for(int i = 0; i < len; ++i)
      fprintf(stdout, "%c", (char)ch[i]);
  }
}

void comments(void* ctx, const xmlChar* ch) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}
