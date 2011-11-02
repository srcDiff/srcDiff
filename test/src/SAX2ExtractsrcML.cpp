/*
  SAX2ExtractSource.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "SAX2ExtractSource.hpp"

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
  fprintf(stdout, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");
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

  if(strcmp((const char *)localname, "common") == 0)
    data->in_diff->push_back(COMMON);
  else if(strcmp((const char *)localname, "old") == 0)
      data->in_diff->push_back(DELETE);
  else if(strcmp((const char *)localname, "new") == 0)
    data->in_diff->push_back(INSERT);
  else
    output_start_node(ctx, localname, prefix, URI, nb_namespaces, namespaces, nb_attributes, nb_defaulted, attributes);

}

void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(strcmp((const char *)localname, "common") == 0
     || strcmp((const char *)localname, "old") == 0
     || strcmp((const char *)localname, "new") == 0)
    data->in_diff->pop_back();
  else
    output_end_node(ctx, localname, prefix, URI);
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

void output_start_node(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                       int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                       const xmlChar** attributes) {

  std::string node = "<";

  if(prefix) {

    node += (const char *)prefix;
    node += ":";
  }

  node += (const char *)localname;

  if(strcmp((const char *)localname, "unit") == 0 && nb_namespaces) {

    int index;
    for(int i = 0, index = 0; i < nb_namespaces; ++i, index += 2) {

      node += (const char *)" xmlns";

      if(namespaces[i]) {

        node += ":";
        node += (const char *)namespaces[index];
      }

      node += (const char *)"=\"";
      node += (const char *)namespaces[index + 1];
      node += (const char *)"\"";
    }

  }
  fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  if(nb_attributes) {

    int index;
    for(int i = 0, index = 0; i < nb_attributes; ++i, index += 5) {

      node += (const char *)" ";

      int end = attributes[index + 4] - attributes[index + 3];
      char * value = strndup((const char *)attributes[index + 3], end);

      if(attributes[index + 1]) {

        node += (const char *)attributes[index + 1];
        node += (const char *)":";
      }

      node += (const char *)attributes[index];

      node += (const char *)"=\"";
  
      node += (const char *)value;

      node += (const char *)"\"";

      /*
      if(attributes[index + 2])
        fprintf(stderr, "\t%s\n", attributes[index + 2]);
      else
        fprintf(stderr, "\n");
      */

    }

  }

      node += (const char *)">";

      fprintf(stdout, "%s", node.c_str());

}

void output_end_node(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *UR) {

  std::string node = "</";

  if(prefix) {

    node += (const char *)prefix;
    node += ":";
  }

  node += (const char *)localname;

  node += (const char *)">";

  fprintf(stdout, "%s", node.c_str());

}
