/*
  SAX2ExtractSource.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "SAX2ColorDiff.hpp"
#include "shortest_edit_script.h"

const char * const common_color = "[[1m\1^[[0m|g";
const char * const delete_color = "[[1;31m\1^[[0m";
const char * const insert_color = "[[1;34m\1^[[0m";

void output_start_node(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                       int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                       const xmlChar** attributes);

void output_end_node(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *UR);

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
  fprintf(stdout, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
}

void endDocument(void* ctx) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);

  fprintf(stdout, "\n");
}

void startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
		     int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
		     const xmlChar** attributes) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    if(strcmp((const char *)localname, "common") == 0)
      data->in_diff->push_back(COMMON);
    else if(strcmp((const char *)localname, "delete") == 0)
      data->in_diff->push_back(DELETE);
    else if(strcmp((const char *)localname, "insert") == 0)
      data->in_diff->push_back(INSERT);

  }

}

void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    if(strcmp((const char *)localname, "common") == 0
       || strcmp((const char *)localname, "old") == 0
       || strcmp((const char *)localname, "new") == 0)
      data->in_diff->pop_back();

  }

}

void characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(data->in_diff->back() == COMMON)
    fprintf(stdout, "%s", common_color);
  else if(data->in_diff->back() == DELETE)
    fprintf(stdout, "%s", delete_color);
  else
    fprintf(stdout, "%s", insert_color);

    for (int i = 0; i < len; ++i) {

      if ((char)ch[i] == '&')
        fprintf(stdout,"&amp;");
      else if ((char)ch[i] == '<')
        fprintf(stdout, "&lt;");
      else if ((char)ch[i] == '>')
        fprintf(stdout, "&gt;");
      else
        fprintf(stdout, "%c", (char)ch[i]);

    }

}

void comments(void* ctx, const xmlChar* ch) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}

