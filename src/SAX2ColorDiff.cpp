/*
  SAX2ColorDiff.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "SAX2ColorDiff.hpp"
#include "shortest_edit_script.h"

const char * const normal_color = "normal";

const char * const common_color = "common";
const char * const delete_color = "delete";
const char * const insert_color = "insert";

const char * const diff_color_common = "diff_common";
const char * const diff_color_change = "diff_change";
const char * const diff_color_delete = "diff_delete";
const char * const diff_color_insert = "diff_insert";

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

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  // fprintf(stderr, "%s\n\n", __FUNCTION__);

  std::string span_class;

  if(data->in_diff->back() == COMMON)
    span_class = common_color;
  else if(data->in_diff->back() == DELETE)
    span_class = delete_color;
  else
    span_class = insert_color;

  span_class += "-";

  std::string span_out = span_class;

  if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)
     && data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)){

      span_out = span_class + diff_color_change;

  } else if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)) {

      span_out = span_class + diff_color_change;

  } else if(data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)) {

      span_out = span_class + diff_color_change;

  } else {

      span_out = span_class + diff_color_common;

  }

  fprintf(stdout, "<html>");
  fprintf(stdout, "<head>");
  fprintf(stdout, "</head>");
  fprintf(stdout, "<body>");
  fprintf(stdout, "<pre>");
  fprintf(stdout, "<span class=\"%s\">%d-%d\t", span_out.c_str(), data->line_old, data->line_new);

}

void endDocument(void* ctx) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);

  fprintf(stdout, "%s\n", normal_color);
  fprintf(stdout, "</pre>");
  fprintf(stdout, "</body>");
  fprintf(stdout, "</html>");
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
       || strcmp((const char *)localname, "delete") == 0
       || strcmp((const char *)localname, "insert") == 0)
      data->in_diff->pop_back();

  }

}

void characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  std::string span_class;

  if(data->in_diff->back() == COMMON)
    span_class = common_color;
  else if(data->in_diff->back() == DELETE)
    span_class = delete_color;
  else
    span_class = insert_color;

  span_class += "-";

  std::string span_out = span_class;

  if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)
     && data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)){

      span_out = span_class + diff_color_change;

  } else if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)) {

      span_out = span_class + diff_color_change;

  } else if(data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)) {

      span_out = span_class + diff_color_change;

  } else {

      span_out = span_class + diff_color_common;

  }

  fprintf(stdout, "</span><span class=\"%s\">", span_out.c_str());

  for (int i = 0; i < len; ++i) {

    if ((char)ch[i] == '&')
      fprintf(stdout,"&amp;");
    else if ((char)ch[i] == '<')
      fprintf(stdout, "&lt;");
    else if ((char)ch[i] == '>')
      fprintf(stdout, "&gt;");
    else if((char)ch[i] != '\n')
      fprintf(stdout, "%c", (char)ch[i]);

    if((char)ch[i] == '\n') {

      if(data->in_diff->back() == COMMON) {

        ++data->line_old;
        ++data->line_new;

      } else if(data->in_diff->back() == DELETE) {

        ++data->line_old;

      } else {
        ++data->line_new;

      }

      std::string span_out = span_class;

  if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)
     && data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)){

      span_out = span_class + diff_color_change;

  } else if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)) {

      span_out = span_class + diff_color_change;

  } else if(data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)) {

      span_out = span_class + diff_color_change;

  } else {

      span_out = span_class + diff_color_common;

  }

  // clear color before output line
  fprintf(stdout, "</span><span class=\"%s\">", normal_color);
  fprintf(stdout, "%c", (char)'\n');
  fprintf(stdout, "</span><span class=\"%s\">%d-%d\t", span_out.c_str(), data->line_old, data->line_new);
    
    }

  if(data->in_diff->back() == COMMON)
    span_class = common_color;
  else if(data->in_diff->back() == DELETE)
    span_class = delete_color;
  else
    span_class = insert_color;

  span_class += "-";

  }

}

void comments(void* ctx, const xmlChar* ch) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}

