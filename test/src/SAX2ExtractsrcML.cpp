/*
  SAX2ExtractSource.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include <vector>
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

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(strcmp((const char *)localname, "common") == 0)
    data->in_diff->push_back(COMMON);

  if(strcmp((const char *)localname, "old") == 0)
    data->in_diff->push_back(DELETE);

  if(strcmp((const char *)localname, "new") == 0)
    data->in_diff->push_back(INSERT);

  }


  if(strcmp((const char *)localname, "escape") == 0) {

    int index;
    for(int i = 0, index = 0; i < nb_attributes; ++i, index += 5) {

      if(strcmp((const char *)attributes[index], "char") == 0) {

        int end = attributes[index + 4] - attributes[index + 3];
        char * value = strndup((const char *)attributes[index + 3], end);

        fprintf(stdout, "%c", (char)strtol(value, NULL, 16));

      }

    }

  }

}

void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(strcmp((const char *)localname, "common") == 0
     || strcmp((const char *)localname, "old") == 0
     || strcmp((const char *)localname, "new") == 0)
    data->in_diff->pop_back();

}

void characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(data->in_diff->back() == data->op || data->in_diff->back() == COMMON) {

    for(int i = 0; i < len; ++i)
      fprintf(stdout, "%c", (char)ch[i]);
  }
}

void comments(void* ctx, const xmlChar* ch) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}

void output_start_node() {

  std::string node = "<";

  

  if(prefix) {

    fprintf(stderr, "%s:\t%s:%s\t%s\n", __FUNCTION__, prefix, localname, URI);
  } else {

    fprintf(stderr, "%s:\t%s\t%s\n", __FUNCTION__, localname, URI);
  }

  if(nb_namespaces) {
    fprintf(stderr, "\t\tNamespaces:\n");

    int index;
    for(int i = 0, index = 0; i < nb_namespaces; ++i, index += 2)
      if(namespaces[i])
        fprintf(stderr, "\t\t\t%d. %s:%s\n", (i + 1) / 2 + 1, namespaces[index], namespaces[index + 1]);
      else
        fprintf(stderr, "\t\t\t%d. %s\n", (i + 1) / 2 + 1, namespaces[index + 1]);

  }

  if(nb_attributes) {
    fprintf(stderr, "\t\tAttributes:\n");

    int index;
    for(int i = 0, index = 0; i < nb_attributes; ++i, index += 5) {

      int end = attributes[index + 4] - attributes[index + 3];
      char * value = strndup((const char *)attributes[index + 3], end);

      if(attributes[index + 1])
        fprintf(stderr, "\t\t\t%d. %s:%s=%s", i + 1, attributes[index + 1], attributes[index], value);
      else
        fprintf(stderr, "\t\t\t%d. %s=%s", i + 1, attributes[index], value);

      if(attributes[index + 2])
        fprintf(stderr, "\t%s\n", attributes[index + 2]);
      else
        fprintf(stderr, "\n");

    }

  }

}
