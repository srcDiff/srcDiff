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

  std::string span_class = "class=\"";

  if(data->in_diff->back() == SESCOMMON)
    span_class += common_color;
  else if(data->in_diff->back() == SESDELETE)
    span_class += delete_color;
  else
    span_class += insert_color;

  span_class += " ";

  std::string span_out = span_class;

  if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)
     && data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)){

    span_out = span_class + diff_color_change;

  } else if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)) {

    span_out = span_class + diff_color_delete;

  } else if(data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)) {

    span_out = span_class + diff_color_insert;

  } else {

    span_out = span_class + diff_color_common;

  }

  span_out += "\"";


  colordiff_file << "<html>\n";
  colordiff_file << "<head>\n";
  colordiff_file << "<style type=\"text/css\">\n";
  colordiff_file << ".normal\n";
  colordiff_file << "{\n";
  colordiff_file << "\n";
  colordiff_file << "color:black;\n";
  colordiff_file << "background-color:white;\n";
  colordiff_file << "font-weight:normal;\n";
  colordiff_file << "\n";
  colordiff_file << "}\n";
  colordiff_file << "\n";
  colordiff_file << ".common\n";
  colordiff_file << "{\n";
  colordiff_file << "\n";
  colordiff_file << "color:black;\n";
  colordiff_file << "\n";
  colordiff_file << "}\n";
  colordiff_file << "\n";
  colordiff_file << ".delete\n";
  colordiff_file << "{\n";
  colordiff_file << "\n";
  colordiff_file << "color:red;\n";
  colordiff_file << "font-weight:bold;\n";
  //colordiff_file << "text-decoration:line-through;\n";
  colordiff_file << "\n";
  colordiff_file << "}\n";
  colordiff_file << "\n";
  colordiff_file << ".insert\n";
  colordiff_file << "{\n";
  colordiff_file << "\n";
  colordiff_file << "color:blue;\n";
  colordiff_file << "font-weight:bold;\n";
  //colordiff_file << "text-decoration:underline;\n";
  colordiff_file << "\n";
  colordiff_file << "}\n";
  colordiff_file << "\n";
  colordiff_file << ".diff_common\n";
  colordiff_file << "{\n";
  colordiff_file << "\n";
  colordiff_file << "background-color:white;\n";
  colordiff_file << "\n";
  colordiff_file << "}\n";
  colordiff_file << "\n";
  colordiff_file << ".diff_change\n";
  colordiff_file << "{\n";
  colordiff_file << "\n";
  colordiff_file << "background-color:yellow;\n";
  colordiff_file << "font-weight:bold;\n";
  colordiff_file << "\n";
  colordiff_file << "}\n";
  colordiff_file << "\n";
  colordiff_file << ".diff_delete\n";
  colordiff_file << "{\n";
  colordiff_file << "\n";
  colordiff_file << "background-color:#FA8258;\n";
  colordiff_file << "font-weight:bold;\n";
  colordiff_file << "\n";
  colordiff_file << "}\n";
  colordiff_file << "\n";
  colordiff_file << ".diff_insert\n";
  colordiff_file << "{\n";
  colordiff_file << "\n";
  colordiff_file << "background-color:#CED8F6;\n";
  colordiff_file << "font-weight:bold;\n";
  colordiff_file << "\n";
  colordiff_file << "}\n";
  colordiff_file << "</style>\n";

  //colordiff_file << "<link href=\"http://www.sdml.info/projects/srcml/ex/colordiff.css\" rel=\"stylesheet\" type=\"text/css\"></link>";
  colordiff_file << "</head>\n";
  colordiff_file << "<body>\n";
  colordiff_file << "<pre>\n";
  colordiff_file << data->line_old << "-" << data->line_new;
  colordiff_file << "<span " << span.out.c_str() << ">\t";

}

void endDocument(void* ctx) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);

  colordiff_file << "</span><span class=\"%s\"/>", normal_color;
  colordiff_file << "</pre>\n";
  colordiff_file << "</body>\n";
  colordiff_file << "</html>\n";
}

void startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                    int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                    const xmlChar** attributes) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    if(strcmp((const char *)localname, "common") == 0)
      data->in_diff->push_back(SESCOMMON);
    else if(strcmp((const char *)localname, "delete") == 0)
      data->in_diff->push_back(SESDELETE);
    else if(strcmp((const char *)localname, "insert") == 0)
      data->in_diff->push_back(SESINSERT);

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

  std::string span_class = "class=\"";

  if(data->in_diff->back() == SESCOMMON)
    span_class += common_color;
  else if(data->in_diff->back() == SESDELETE)
    span_class += delete_color;
  else
    span_class += insert_color;

  span_class += " ";

  std::string span_out = span_class;

  if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)
     && data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)){

    span_out = span_class + diff_color_change;

  } else if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)) {

    span_out = span_class + diff_color_delete;

  } else if(data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)) {

    span_out = span_class + diff_color_insert;

  } else {

    span_out = span_class + diff_color_common;

  }

  span_out += "\"";

  colordiff_file << "</span><span " << span_out.c_str()) << ">";

  for (int i = 0; i < len; ++i) {

    if ((char)ch[i] == '&')
      colordiff_file << "&amp;";
    else if ((char)ch[i] == '<')
      colordiff_file << "&lt;";
    else if ((char)ch[i] == '>')
      colordiff_file << "&gt;";
    else if((char)ch[i] != '\n')
      colordiff_file << (char)ch[i]);

    if((char)ch[i] == '\n') {

      if(data->in_diff->back() == SESCOMMON) {

        ++data->line_old;
        ++data->line_new;

      } else if(data->in_diff->back() == SESDELETE) {

        ++data->line_old;

      } else {
        ++data->line_new;

      }


      std::string span_out = span_class;

      if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)
         && data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)){

        span_out = span_class + diff_color_change;

      } else if(data->line_old < data->lines_old.size() && data->lines_old.at(data->line_old)) {

        span_out = span_class + diff_color_delete;

      } else if(data->line_new < data->lines_new.size() && data->lines_new.at(data->line_new)) {

        span_out = span_class + diff_color_insert;

      } else {

        span_out = span_class + diff_color_common;

      }

      span_out += "\"";

      // clear color before output line
      fprintf(stdout, "</span><span class=\"%s\">", normal_color);
      fprintf(stdout, "%c%d-%d", (char)'\n', data->line_old, data->line_new);
      fprintf(stdout, "</span><span %s>\t", span_out.c_str());

    }

    span_class = "class=\"";

    if(data->in_diff->back() == SESCOMMON)
      span_class += common_color;
    else if(data->in_diff->back() == SESDELETE)
      span_class += delete_color;
    else
      span_class += insert_color;

    span_class += " ";

  }

}

void comments(void* ctx, const xmlChar* ch) {

  // fprintf(stderr, "%s\n\n", __FUNCTION__);
}

