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

// helper method
int find_attribute_index(int nb_attributes, const xmlChar** attributes, const char* attribute);
std::string & trim_string(std::string & source);

SAX2DiffTrace() {
}

static xmlSAXHandler factory() {

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

static void startDocument(void * ctx) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
  fprintf(stdout, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
}

  static void endDocument(void * ctx);


  // fprintf(stderr, "%s\n\n", __FUNCTION__);

  fprintf(stdout, "\n");
}

  static void startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
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

static void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    if(strcmp((const char *)localname, "common") == 0
       || strcmp((const char *)localname, "old") == 0
       || strcmp((const char *)localname, "new") == 0)
      data->in_diff->pop_back();

  }

}

static void characters(void* ctx, const xmlChar* ch, int len) {

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

static void comments(void* ctx, const xmlChar* ch) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}

void output_diff(SAX2DiffTrace * pstate, int numelements) {

  // output stack formatted to be an XPath
  // if(pstate->elements.size() > 1)
  //if(pstate->elements[pstate->elements.size() - 1] == "parameter_list" && pstate->column_diffs[pstate->editpos].off == 0)
      if(pstate->DIFF_TYPE == DIFF_INSERT)
        fprintf(stdout,"Insert:\t");
      else
        fprintf(stdout,"Delete:\t");
      /*else if(pstate->DIFF_TYPE == DIFF_INSERT)
      fprintf(stdout,"Change:\t");
    else
      fprintf(stdout,"Change:\t");
  else 
      fprintf(stdout,"OutsideMethodChange\t");    
      */
    
      //fprintf(stderr, "NumElements: %d\n", numelements);
      int blockindex = -1;
      for(int i = 0; i < numelements; ++i) {
        if(strcmp(pstate->elements[i].c_str(), "unit") == 0 || strcmp(pstate->elements[i].c_str(), "block") == 0)
          ++blockindex;
        //fprintf(stderr, "Block Index: %d Block Size:%d\n", blockindex, pstate->blocks.size());
        if (pstate->labels[i][0] != 0) {
          if(pstate->elements[i] == "unit") {
            fprintf(stdout, "/src:%s[@filename='%s']", pstate->elements[i].c_str(), pstate->labels[i].c_str());
          }
          else if(pstate->elements[i] == "function" || pstate->elements[i] == "function_decl") {

            /*
              || pstate->elements[i] == "struct" || pstate->elements[i] == "struct_decl"
              || pstate->elements[i] == "class" || pstate->elements[i] == "class_decl") {
            */

            fprintf(stdout, "/src:%s[src:signature('%s')]", pstate->elements[i].c_str(), pstate->labels[i].c_str());
            //        break;
          }
          else if(pstate->elements[i] == "class" || pstate->elements[i] == "class_decl"
                  || pstate->elements[i] == "struct" || pstate->elements[i] == "struct_decl")

            fprintf(stdout, "/src:%s[src:construct('%s')]", pstate->elements[i].c_str(), pstate->labels[i].c_str());
        }

        else if(pstate->blocks[blockindex][pstate->elements[i]] > 0)
          fprintf(stdout, "/src:%s[%d]", pstate->elements[i].c_str(), pstate->blocks[blockindex][pstate->elements[i]]);
        else
          fprintf(stdout, "/src:%s", pstate->elements[i].c_str());
      }

  fprintf(stdout, "\n");

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
