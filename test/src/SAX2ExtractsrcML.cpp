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

bool isempty = false; 
bool outputend = false;

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

  if(outputend)
    fprintf(stdout, ">");

  isempty = false;
  outputend = false;

  if(strcmp((const char *)localname, "common") == 0)
    data->in_diff->push_back(COMMON);
  else if(strcmp((const char *)localname, "old") == 0)
      data->in_diff->push_back(DELETE);
  else if(strcmp((const char *)localname, "new") == 0)
    data->in_diff->push_back(INSERT);
  else if(data->in_diff->back() == data->op || data->in_diff->back() == COMMON) {
    isempty = true;
    outputend = true;
    if(strcmp((const char *)localname, "unit") == 0)
      fprintf(stdout, "<unit xmlns=\"http://www.sdml.info/srcML/src\" xmlns:cpp=\"http://www.sdml.info/srcML/cpp\" xmlns:lit=\"http://www.sdml.info/srcML/literal\" xmlns:op=\"http://www.sdml.info/srcML/operator\" xmlns:type=\"http://www.sdml.info/srcML/modifier\" language=\"C\"");
    else
      output_start_node(ctx, localname, prefix, URI, nb_namespaces, namespaces, nb_attributes, nb_defaulted, attributes);
  }

}

void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(outputend && isempty)
    fprintf(stdout, "/>");
  else if(outputend)
    fprintf(stdout, ">");

  if(strcmp((const char *)localname, "common") == 0
     || strcmp((const char *)localname, "old") == 0
     || strcmp((const char *)localname, "new") == 0)
    data->in_diff->pop_back();
  else if(!isempty && (data->in_diff->back() == data->op || data->in_diff->back() == COMMON))
      output_end_node(ctx, localname, prefix, URI);

  isempty = false;
  outputend = false;

}

void characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(outputend)
    fprintf(stdout, ">");

  isempty = false;
  outputend = false;

  if(data->in_diff->back() == data->op || data->in_diff->back() == COMMON) {

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
