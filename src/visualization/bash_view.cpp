/*
  bash_view.cpp

  Michael John Decker
  mdecker6@kent.edu
*/

#include <bash_view.hpp>
#include <shortest_edit_script.hpp>

#include <libxml/parserInternals.h>

#include <cstring>

const char * const DELETE_CODE = "\x1b[9;48;5;210;1m";
const char * const INSERT_CODE = "\x1b[48;5;120;1m";

const char * const COMMON_CODE = "\x1b[0m";

const char * const LINE_CODE = "\x1b[36m";

const char * CARRIAGE_RETURN_SYMBOL = "\u23CE";

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

void bash_view::output_additional_context() {


  unsigned long line_delete = line_number_delete + 1 - additional_context.size();
  unsigned long line_insert = line_number_insert + 1 - additional_context.size();

  if(wait_change && last_context_line != (line_number_delete - 1))
    (*output) << COMMON_CODE << LINE_CODE << "@@ -" << line_delete << " +" << line_insert << " @@" << COMMON_CODE;

  if(additional_context.empty()) return;

  for(std::list<std::string>::const_iterator citr = additional_context.begin(); citr != additional_context.end(); ++citr) {

    (*output) << *citr;

    ++line_delete, ++line_insert;

  }

  additional_context.clear();
  length = 0;

}

void bash_view::characters(const char * ch, int len) {

  const char * code = COMMON_CODE;
  if(diff_stack.back() == SESDELETE) code = DELETE_CODE;
  else if(diff_stack.back() == SESINSERT) code = INSERT_CODE;

  if(code != COMMON_CODE) (*output) << code;

  for(int i = 0; i < len; ++i) {

    if(wait_change) {

      context.append(&ch[i], 1);

    } else {

      if(code != COMMON_CODE && ch[i] == '\n') (*output) << CARRIAGE_RETURN_SYMBOL << COMMON_CODE;
      (*output) << ch[i];

      if(code != COMMON_CODE && ch[i] == '\n') (*output) << code;

    }

    if(ch[i] == '\n') {

      if(is_after_change) {

        is_after_change = false;
        is_after_additional = true;

      } else if(is_after_additional) {

        ++after_edit_count;

        if(after_edit_count == num_context_lines) {

          is_after_additional = false;
          after_edit_count = 0;
          wait_change = true;
          last_context_line = line_number_delete;

        }

      } else if(wait_change && num_context_lines != 0) {

        if(length >= num_context_lines)
          additional_context.pop_front(), --length;

        additional_context.push_back(context);
        ++length;

      }

      if(code == COMMON_CODE || code == DELETE_CODE) ++line_number_delete;
      if(code == COMMON_CODE || code == INSERT_CODE) ++line_number_insert;

      context = "";

    }

  }

  if(code != COMMON_CODE) (*output) << COMMON_CODE;

}

void bash_view::characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  bash_view * data = (bash_view *)ctxt->_private;

  if(data->diff_stack.back() != SESCOMMON) {

    data->output_additional_context();

    data->is_after_additional = false;
    data->is_after_change = false;
    data->wait_change = false;

    data->output->write(data->context.c_str(), data->context.size());
    data->context = "";

  }

  data->characters((const char *)ch, len);

  if(data->diff_stack.back() != SESCOMMON) data->is_after_change  = true;

}

void bash_view::comment(void* ctx, const xmlChar* ch) {}

