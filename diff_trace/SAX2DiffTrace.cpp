/*
  SAX2DiffTrace.cpp

  Implements SAX object and SAX methods for extracting an absolute XPath from
  srcML documents for differences between files

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

// includes
#include <cstring>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <iostream>

#include "SAX2DiffTrace.hpp"
#include "../src/shortest_edit_script.h"

// helper method
int find_attribute_index(int nb_attributes, const xmlChar** attributes, const char* attribute);
std::string & trim_string(std::string & source);

xmlSAXHandler SAX2DiffTrace::factory() {

  xmlSAXHandler sax = { 0 };

  sax.initialized    = XML_SAX2_MAGIC;

  sax.startDocument = &SAX2DiffTrace::startDocument;
  sax.endDocument = &SAX2DiffTrace::endDocument;

  sax.startElementNs = &SAX2DiffTrace::startElementNs;
  sax.endElementNs = &SAX2DiffTrace::endElementNs;

  sax.characters = &SAX2DiffTrace::characters;
  sax.comment = &SAX2DiffTrace::comments;

  return sax;
}

void SAX2DiffTrace::startDocument(void * ctx) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  SAX2DiffTrace & tracer = *(SAX2DiffTrace *)ctxt->_private;

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  tracer.output = false;

  diff startdiff = { 0 };
  startdiff.operation = COMMON;

  tracer.diff_stack.push_back(startdiff);

}

void SAX2DiffTrace::endDocument(void * ctx) {
  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  // fprintf(stderr, "%s\n\n", __FUNCTION__);

  fprintf(stdout, "\n");
}

void SAX2DiffTrace::startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                    int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                             const xmlChar** attributes) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  SAX2DiffTrace & tracer = *(SAX2DiffTrace *)ctxt->_private;

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    diff curdiff = { 0 };

    if(strcmp((const char *)localname, "common") == 0) {

      curdiff.operation = COMMON;

    } else if(strcmp((const char *)localname, "delete") == 0) {

      tracer.output = true;
      curdiff.operation = DELETE;

    } else if(strcmp((const char *)localname, "insert") == 0) {

      tracer.output = true;
      curdiff.operation = INSERT;

    }

    tracer.diff_stack.push_back(curdiff);

  } else {

    element curelement;
    curelement.name = (const char *)localname;
    curelement.prefix = (const char *)prefix;

    tracer.elements.push_back(curelement);
    ++tracer.diff_stack.back().level;

  }

  if(tracer.diff_stack.back().operation != COMMON && tracer.diff_stack.back().level == 1) {

    output_diff(tracer);

  }

}

void SAX2DiffTrace::endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  SAX2DiffTrace & tracer = *(SAX2DiffTrace *)ctxt->_private;

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    if(strcmp((const char *)localname, "common") == 0
       || strcmp((const char *)localname, "old") == 0
       || strcmp((const char *)localname, "new") == 0)
      tracer.diff_stack.pop_back();

  } else {

    tracer.elements.pop_back();
    --tracer.diff_stack.back().level;

  }

}

void SAX2DiffTrace::characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  SAX2DiffTrace & tracer = *(SAX2DiffTrace *)ctxt->_private;

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  int i;
  for(i = 0; i < len; ++i) {

    if(!isspace((char)ch[i]))
       break;

  }

    if(i == len && tracer.diff_stack.back().level == 0) {

      element curelement;
      curelement.name = "text()";
      curelement.prefix = "";

      tracer.elements.push_back(curelement);

      output_diff(tracer);

      tracer.elements.pop_back();

    }

}

void SAX2DiffTrace::comments(void* ctx, const xmlChar* ch) {

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

}

void output_diff(SAX2DiffTrace & tracer) {

  fprintf(stdout, "/");

  for(unsigned int i = 0; i < tracer.elements.size() - 1; ++i) {

    if(tracer.elements.at(i).prefix.c_str() != "")
      fprintf(stdout, "%s:%s/", tracer.elements.at(i).prefix.c_str(), tracer.elements.at(i).name.c_str());
    else
      fprintf(stdout, "%s/", tracer.elements.at(i).name.c_str());

  }

  if(tracer.elements.back().prefix.c_str() != "")
    fprintf(stdout, "%s:%s\n", tracer.elements.back().prefix.c_str(), tracer.elements.back().name.c_str());
  else
    fprintf(stdout, "%s\n", tracer.elements.back().name.c_str());

}

// index of attribute in attributes
int find_attribute_index(int nb_attributes, const xmlChar** attributes, const char* attribute) {

  for (int i = 0, index = 0; attributes[index]; ++i, index += 2)
    if (strcmp((const char*) attributes[index], attribute) == 0)
      return index;

  return -1;
}

// trim spaces from end of string
std::string & trim_string(std::string & source) {

  //fprintf(stderr, "%s\n", source.c_str());

  int pos;
  for(pos = source.size() - 2; pos >= 0 && source[pos] == ' ' && source[pos + 1] == ' '; --pos);
  return source.erase(pos + 2);
}
