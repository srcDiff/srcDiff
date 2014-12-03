/*
  SAX2ColorDiff.cpp

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <SAX2BashView.hpp>
#include <ShortestEditScript.hpp>

void bash_view::startDocument(void* ctx) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  bash_view * data = (bash_view *)ctxt->_private;

  data->diff_stack.push_back(SESCOMMON);


}

void bash_view::endDocument(void* ctx) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  bash_view * data = (bash_view *)ctxt->_private;

}

void bash_view::startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                    int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                    const xmlChar** attributes) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  bash_view * data = (bash_view *)ctxt->_private;

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    if(strcmp((const char *)localname, "common") == 0)
      data->diff_stack.push_back(SESCOMMON);
    else if(strcmp((const char *)localname, "delete") == 0)
      data->diff_stack.push_back(SESDELETE);
    else if(strcmp((const char *)localname, "insert") == 0)
      data->diff_stack.push_back(SESINSERT);
    
  }

}

void bash_view::endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  bash_view * data = (bash_view *)ctxt->_private;

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    if(strcmp((const char *)localname, "common") == 0
       || strcmp((const char *)localname, "delete") == 0
       || strcmp((const char *)localname, "insert") == 0)
      data->diff_stack.pop_back();
  }

}

const char * delete_code = "\x1b[101;1m;";
const char * insert_code = "\x1b[102;1m;";

const char * common_code = "\x1b[0m;";

void bash_view::characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  bash_view * data = (bash_view *)ctxt->_private;

  if(data->diff_stack.back() == SESCOMMON)
      return;

  if(data->diff_stack.back() == SESDELETE)
    data->output << delete_code;
  else
    data->output << insert_code;

  data->output.write((const char *)ch, len);

  data->output << common_code;

}

void bash_view::comment(void* ctx, const xmlChar* ch) {}

