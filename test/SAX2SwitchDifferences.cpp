/*
  SAX2SwitchDifferences.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include <string>
#include "SAX2SwitchDifferences.hpp"

#define LITERALPLUSSIZE(s) BAD_CAST s, sizeof(s) - 1

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

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_switch * data = (source_switch *)ctxt->_private;

  xmlTextWriterWriteRawLen(data->writer, LITERALPLUSSIZE("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"));
}

void endDocument(void* ctx) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}

void startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                    int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                    const xmlChar** attributes) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_switch * data = (source_switch *)ctxt->_private;

  std::string name = "";
  if(prefix) {

    name += (const char *)prefix;
    name += ":";
  }

  name += (const char *)localname;

  xmlTextWriterStartElement(data->writer, (const xmlChar *)name.c_str());

  static bool first = true;

  if(first) {

    first = false;

    int index;
    for(int i = 0, index = 0; i < nb_namespaces; ++i, index += 2) {

      std::string ns = "xmlns";
      if(namespaces[index]) {

        ns += ":";
        ns += (const char *)namespaces[index];
      }

      xmlTextWriterWriteAttribute(data->writer, (const xmlChar *)ns.c_str(), (const xmlChar *)namespaces[index+1]);
    }
  }

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

  xmlTextWriterEndElement(data->writer);
}

void characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_switch * data = (source_switch *)ctxt->_private;

  for (int i = 0; i < len; ++i) {

    // escape characters or print out character
    if (ch[i] == '&')
      xmlTextWriterWriteRawLen(data->writer, LITERALPLUSSIZE("&amp;"));
    else if (ch[i] == '<')
      xmlTextWriterWriteRawLen(data->writer, LITERALPLUSSIZE("&lt;"));
    else if (ch[i] == '>')
      xmlTextWriterWriteRawLen(data->writer, LITERALPLUSSIZE("&gt;"));
    else
      xmlTextWriterWriteRawLen(data->writer, &ch[i], 1);
  }

}

void comments(void* ctx, const xmlChar* ch) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}
