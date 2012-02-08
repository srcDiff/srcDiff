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

#include "SAX2ColorDiff.hpp"

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

void output_start_document(std::ostream & colordiff_file, std::string & css_url) {

  colordiff_file << "<html>\n";
  colordiff_file << "<head>\n";

  if(css_url == "") {

  colordiff_file << "<style type=\"text/css\">\n";
  colordiff_file << "h1\n";
  colordiff_file << "{\n";
  colordiff_file << "font-size:12pt;\n";
  colordiff_file << "margin-bottom:0px;\n";
  colordiff_file << "}\n";
  colordiff_file << ".normal\n";
  colordiff_file << "{\n";
  colordiff_file << "color:black;\n";
  colordiff_file << "background-color:white;\n";
  colordiff_file << "font-weight:normal;\n";
  colordiff_file << "}\n";
  colordiff_file << ".common\n";
  colordiff_file << "{\n";
  colordiff_file << "color:black;\n";
  colordiff_file << "}\n";
  colordiff_file << ".delete\n";
  colordiff_file << "{\n";
  colordiff_file << "color:red;\n";
  colordiff_file << "font-weight:bold;\n";
  //colordiff_file << "text-decoration:line-through;\n";
  colordiff_file << "}\n";
  colordiff_file << ".insert\n";
  colordiff_file << "{\n";
  colordiff_file << "color:blue;\n";
  colordiff_file << "font-weight:bold;\n";
  //colordiff_file << "text-decoration:underline;\n";
  colordiff_file << "}\n";
  colordiff_file << ".diff_common\n";
  colordiff_file << "{\n";
  colordiff_file << "background-color:white;\n";
  colordiff_file << "}\n";
  colordiff_file << ".diff_change\n";
  colordiff_file << "{\n";
  colordiff_file << "background-color:yellow;\n";
  colordiff_file << "font-weight:bold;\n";
  colordiff_file << "}\n";
  colordiff_file << ".diff_delete\n";
  colordiff_file << "{\n";
  colordiff_file << "background-color:#FA8258;\n";
  colordiff_file << "font-weight:bold;\n";
  colordiff_file << "}\n";
  colordiff_file << ".diff_insert\n";
  colordiff_file << "{\n";
  colordiff_file << "background-color:#CED8F6;\n";
  colordiff_file << "font-weight:bold;\n";
  colordiff_file << "}\n";
  colordiff_file << ".line\n";
  colordiff_file << "{\n";
  colordiff_file << "display:inline-block;\n";
  colordiff_file << "width:5em;\n";
  colordiff_file << "text-align:right;\n";
  colordiff_file << "margin-right:1em;\n";
  colordiff_file << "}\n";
  colordiff_file << "div.key\n";
  colordiff_file << "{\n";
  colordiff_file << "position:fixed;\n";
  colordiff_file << "right:10px;\n";
  colordiff_file << "left:auto;\n";
  colordiff_file << "text-align:center;\n";
  colordiff_file << "}\n";
  colordiff_file << "div.key span\n";
  colordiff_file << "{\n";
  colordiff_file << "display:block;\n";
  colordiff_file << "margin-bottom:5px;\n";
  colordiff_file << "}\n";
  colordiff_file << "</style>\n";

  } else {
 
    colordiff_file << "<link href=\"" << css_url << "\" rel=\"stylesheet\" type=\"text/css\"></link>";

  }

  colordiff_file << "</head>\n";
  colordiff_file << "<body>\n";

  colordiff_file << "<div class=\"key\">\n";
  colordiff_file << "<span class=\"" << diff_color_insert << "\"><cite>diff</cite> Insert</span>"; 
  colordiff_file << "<span class=\"" << insert_color << "\"><cite>srcDiff</cite> Insert</span>"; 
  colordiff_file << "<span class=\"" << insert_color << " " << diff_color_insert << "\"><cite>diff</cite> &amp; <cite>srcDiff</cite> Insert</span>"; 
  colordiff_file << "<span class=\"" << diff_color_delete << "\"><cite>diff</cite> Delete</span>"; 
  colordiff_file << "<span class=\"" << delete_color << "\"><cite>srcDiff</cite> Delete</span>"; 
  colordiff_file << "<span class=\"" << delete_color << " " << diff_color_delete << "\"><cite>diff</cite> &amp; <cite>srcDiff</cite> Delete</span>"; 
  colordiff_file << "<span class=\"" << diff_color_change << "\"><cite>diff</cite> Change</span>"; 
  colordiff_file << "</div>\n";

  colordiff_file << "<pre>";

}

void output_end_document(std::ostream & colordiff_file) {

  colordiff_file << "</pre>\n";
  colordiff_file << "</body>\n";
  colordiff_file << "</html>\n";

}

void startDocument(void* ctx) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  std::string file_name = data->file_one;

  if(data->file_one != data->file_two) {

    if(data->file_one != "" && data->file_two != "")
      file_name += "|";

    std::vector<std::string> path_one;
    int start = 0, end = 0;
    for(; (end = data->file_one.find("/", start)) != std::string::npos; start = end + 1)
      path_one.push_back(data->file_one.substr(start, end));

    path_one.push_back(data->file_one.substr(start));

    std::vector<std::string> path_two;
    start = 0;
    end = 0;
    for(; (end = data->file_two.find("/", start)) != std::string::npos; start = end + 1)
      path_two.push_back(data->file_two.substr(start, end));

    path_two.push_back(data->file_two.substr(start));

    file_name += data->file_two;

  }

  data->colordiff_file << "<div class=\"srcdiff\" filename1=\"" << data->file_one << "\" filename2=\"" << data->file_two << "\">";
  data->colordiff_file << "<h1>" << file_name << "</h1>\n";

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

  if(data->line_old < data->lines_old.size() || data->line_new < data->lines_new.size())
    data->colordiff_file << "<span class=\"line\">" << data->line_old << "-" << data->line_new << "</span>";
  data->colordiff_file << "<span " << span_out.c_str() << ">";

}

void endDocument(void* ctx) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  struct source_diff * data = (source_diff *)ctxt->_private;

  //fprintf(stderr, "%s\n\n", __FUNCTION__);

  data->colordiff_file << "</span><span class=\"" << normal_color << "\"/></span>";
  data->colordiff_file << "</div>\n";
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

  data->colordiff_file << "</span><span " << span_out.c_str() << ">";

  for (int i = 0; i < len; ++i) {

    if ((char)ch[i] == '&')
      data->colordiff_file << "&amp;";
    else if ((char)ch[i] == '<')
      data->colordiff_file << "&lt;";
    else if ((char)ch[i] == '>')
      data->colordiff_file << "&gt;";
    else if((char)ch[i] != '\n')
      data->colordiff_file << (char)ch[i];

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
      data->colordiff_file << "</span><span class=\"" << normal_color << "\">";
      data->colordiff_file << (char)'\n';
      if(data->line_old < data->lines_old.size() || data->line_new < data->lines_new.size())
         data->colordiff_file << "<span class=\"line\">" << data->line_old << "-" << data->line_new << "</span>";
      data->colordiff_file << "</span><span " << span_out.c_str() << ">";

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

