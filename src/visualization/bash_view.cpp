/*
  bash_view.cpp

  Michael John Decker
  mdecker6@kent.edu
*/

#include <bash_view.hpp>
#include <ShortestEditScript.hpp>

#include <libxml/parserInternals.h>

// forward declarations
static xmlParserCtxtPtr createURLParserCtxt(const char * srcdiff);
static void parseDocument(xmlParserCtxtPtr ctxt);

int bash_view::transform(const char * srcdiff) {

  // create the ctxt
  xmlParserCtxtPtr ctxt = createURLParserCtxt(srcdiff);

  // setup sax handler
  xmlSAXHandler sax = bash_view::factory();
  ctxt->sax = &sax;

  ctxt->_private = this;

  parseDocument(ctxt);

  // local variable, do not want xmlFreeParserCtxt to free
  ctxt->sax = NULL;

  // all done with parsing
  xmlFreeParserCtxt(ctxt);

  return 0;
}

// create the ctxt
static xmlParserCtxtPtr createURLParserCtxt(const char * srcdiff) {

  xmlParserCtxtPtr ctxt = xmlCreateMemoryParserCtxt(srcdiff, strlen(srcdiff));
  //xmlCtxtUseOptionsInternal(ctxt, XML_PARSE_COMPACT, NULL);

  if (ctxt == NULL) {

    // report error
    xmlErrorPtr ep = xmlGetLastError();
    fprintf(stderr, "%s: %s", "ExtractsrcML", ep->message);
    exit(1);
  }

  return ctxt;
}

// parse the document
static void parseDocument(xmlParserCtxtPtr ctxt) {

  // process the document
  int status;
  if ((status = xmlParseDocument(ctxt)) == -1) {

    xmlErrorPtr ep = xmlCtxtGetLastError(ctxt);

    // report error
    char* partmsg = strdup(ep->message);
    partmsg[strlen(partmsg) - 1] = '\0';

    fprintf(stderr, "%s: %s in '%s'\n", "ExtractsrcML", partmsg, ep->file);
    exit(1);
  }

}

xmlSAXHandler bash_view::factory() {

  xmlSAXHandler sax = { 0 };

  sax.initialized    = XML_SAX2_MAGIC;

  sax.startDocument = &bash_view::startDocument;
  sax.endDocument = &bash_view::endDocument;

  sax.startElementNs = &bash_view::startElementNs;
  sax.endElementNs = &bash_view::endElementNs;

  sax.characters = &bash_view::characters;
  sax.comment = &bash_view::comment;

  return sax;
}

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

void bash_view::process_characters(const char * ch, int len) {

  for(int i = 0; i < len; ++i) {

    if(diff_stack.back() == SESCOMMON)
      context.append(&ch[i], 1);

    if(ch[i] == '\n') {

      if(is_after_change)
        output->write(context.c_str(), context.size());

      is_line_output = false;
      is_after_change = false;

      ++line_number;
      context = "";
      continue;

    }


  }


}

const char * delete_code = "\x1b[101;1m";
const char * insert_code = "\x1b[102;1m";

const char * common_code = "\x1b[0m";

void bash_view::characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  bash_view * data = (bash_view *)ctxt->_private;

  if(data->diff_stack.back() != SESCOMMON) {

    if(!data->is_line_output)
      (*data->output) << data->line_number + 1 << ":\t";

    data->is_line_output = true;

    data->output->write(data->context.c_str(), data->context.size());
    data->context = "";

  }

  if(data->diff_stack.back() == SESDELETE)
    (*data->output) << delete_code;
  else if(data->diff_stack.back() == SESINSERT)
    (*data->output) << insert_code;

  data->process_characters((const char *)ch, len);

  if(data->diff_stack.back() != SESCOMMON) {

    data->output->write((const char *)ch, len);
    (*data->output) << common_code;
    data->is_after_change = true;

  }

}

void bash_view::comment(void* ctx, const xmlChar* ch) {}

