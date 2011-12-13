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
#include <utility>

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
  tracer.collect = false;

  diff startdiff = { 0 };
  startdiff.operation = COMMON;

  tracer.diff_stack.push_back(startdiff);

}

void SAX2DiffTrace::endDocument(void * ctx) {
  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  // fprintf(stderr, "%s\n\n", __FUNCTION__);

  fprintf(stdout, "\n");
}

static bool is_collect(const char * name, const char * prefix) {

  if(strcmp(name, "function") == 0)
    return true;

  if(strcmp(name, "function_decl") == 0)
    return true;

  return false;
}

static bool is_end_collect(const char * name, const char * prefix) {

  if(strcmp(name, "parameter_list") == 0)
    return true;

  return false;
}

void SAX2DiffTrace::startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                                   int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                                   const xmlChar** attributes) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  SAX2DiffTrace & tracer = *(SAX2DiffTrace *)ctxt->_private;

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  if(strcmp((const char *)localname, "unit") == 0) {

    int idx = find_attribute_index(nb_attributes, attributes, "filename");

    if(idx != -1) {

      int length = attributes[idx + 4] - attributes[idx + 3];

      const char * filename = strndup((const char *)attributes[idx + 3], length);

      char * sep = index(filename, '|');

      if(sep < (filename + length)) {

        *sep = '\0';

        tracer.filename_old = filename;

        tracer.filename_new = sep + 1;

        *sep = '|';

      } else {

        tracer.filename_old = filename;

        tracer.filename_new = filename;


      }

    }

  }

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    diff curdiff = { 0 };

    if(strcmp((const char *)localname, "common") == 0) {

      curdiff.operation = COMMON;

    } else if(strcmp((const char *)localname, "delete") == 0) {

      curdiff.operation = DELETE;

    } else if(strcmp((const char *)localname, "insert") == 0) {

      curdiff.operation = INSERT;

    }

    tracer.diff_stack.push_back(curdiff);

  } else {

    if(tracer.elements.size() > 0) {

      std::map<std::string, int>::iterator pos = tracer.diff_stack.back().children.find(std::string((const char *)localname));

      if(pos != tracer.diff_stack.back().children.end()) {

        ++tracer.diff_stack.back().children[std::string((const char *)localname)];

      } else {

        tracer.diff_stack.back().children[std::string((const char *)localname)] = 1;

      }

    }

    element curelement;
    curelement.name = (const char *)localname;
    if(prefix)
      curelement.prefix = (const char *)prefix;
    else
      curelement.prefix = "";

    curelement.uri = (const char *)URI;

    if(tracer.elements.size() > 0) {

      std::map<std::string, int>::iterator pos = tracer.elements.back().children.find(std::string((const char *)localname));

      if(pos != tracer.elements.back().children.end()) {

        ++tracer.elements.back().children[std::string((const char *)localname)];

      } else {

        tracer.elements.back().children[std::string((const char *)localname)] = 1;

      }

    }

    tracer.elements.push_back(curelement);
    ++tracer.diff_stack.back().level;

    if(!tracer.collect) {
      
      if((tracer.collect = is_collect((const char *)localname, (const char *)prefix)))
        tracer.collect_node_pos = tracer.elements.size() - 1;

      
    }

    if(tracer.diff_stack.back().operation != COMMON && tracer.diff_stack.back().level == 1) {

      output_diff(tracer);

    }

  }


}

void SAX2DiffTrace::endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  SAX2DiffTrace & tracer = *(SAX2DiffTrace *)ctxt->_private;

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    if(strcmp((const char *)localname, "common") == 0
       || strcmp((const char *)localname, "delete") == 0
       || strcmp((const char *)localname, "insert") == 0) {


      for(std::map<std::string, int>::iterator pos = tracer.diff_stack.back().children.begin();
          pos != tracer.diff_stack.back().children.end(); ++pos) {

        tracer.elements.back().children[pos->first] -= pos->second;
      }

      tracer.diff_stack.pop_back();

    }

  } else {

    tracer.elements.pop_back();
    --tracer.diff_stack.back().level;

    if(tracer.collect && is_end_collect((const char *)localname, (const char *)prefix)) {


      tracer.collect = false;

      tracer.elements.at(tracer.collect_node_pos).signature = trim_string(tracer.elements.at(tracer.collect_node_pos).signature);

      //output_diff(tracer);
    }

  }

}

void SAX2DiffTrace::characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  SAX2DiffTrace & tracer = *(SAX2DiffTrace *)ctxt->_private;

  if(tracer.collect) {

    tracer.elements.at(tracer.collect_node_pos).signature.append((const char *)ch, (const char *)ch + len);

  }

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    if(tracer.elements.size() > 0) {

      std::map<std::string, int>::iterator pos = tracer.diff_stack.back().children.find(std::string("text()"));

      if(pos != tracer.diff_stack.back().children.end()) {

        ++tracer.diff_stack.back().children[std::string("text()")];

      } else {

        tracer.diff_stack.back().children[std::string("text()")] = 1;

      }

    }

    if(tracer.elements.size() > 0) {

      std::map<std::string, int>::iterator pos = tracer.elements.back().children.find(std::string("text()"));

      if(pos != tracer.elements.back().children.end()) {

        ++tracer.elements.back().children[std::string("text()")];

      } else {

        tracer.elements.back().children[std::string("text()")] = 1;

      }

    }

  int i;
  for(i = 0; i < len; ++i) {

    if(!isspace((char)ch[i]))
      break;

  }

  if(tracer.diff_stack.back().operation != COMMON && len != 0 && i != len && tracer.diff_stack.back().level == 0) {

    element curelement;
    curelement.name = "text()";
    curelement.prefix = "";
    curelement.uri = "";

    tracer.elements.push_back(curelement);

    output_diff(tracer);

    tracer.elements.pop_back();

  }

}

void SAX2DiffTrace::comments(void* ctx, const xmlChar* ch) {

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

}

void output_diff(SAX2DiffTrace & tracer) {

  if(tracer.diff_stack.back().operation == DELETE)
    fprintf(stdout, "Delete:\t");
  else
    fprintf(stdout, "Insert:\t");

  fprintf(stdout, "/");

  for(unsigned int i = 0; i < tracer.elements.size() - 1; ++i) {

    std::string element = "";

    if(tracer.elements.at(i).prefix != "") {

      element += tracer.elements.at(i).prefix.c_str();
      element += ":";

    } else if(tracer.elements.at(i).uri == "http://www.sdml.info/srcML/src") {

      element += "src:";

    }

    element += tracer.elements.at(i).name.c_str();

    if(tracer.elements.at(i).name == "unit") {

      element += "[@filename=\"";

      if(tracer.diff_stack.back().operation == DELETE)
        element += tracer.filename_old;
      else
        element += tracer.filename_new;

      element += "\"]";

    } else if(is_collect(tracer.elements.at(i).name.c_str(), tracer.elements.at(i).prefix.c_str())) {

        element += "[src:signature(\"\"";
        element += tracer.elements.at(i).signature;
        element += "\")]";

    } else if(i > 0) {

      int count = tracer.elements.at(i - 1).children[std::string(tracer.elements.at(i).name)];

      char * buffer = (char *)malloc(sizeof(char) * count);

      snprintf(buffer, count + 1, "%d", count);

      element += "[";
      element += buffer;
      element += "]";

      free(buffer);

    }

    element += "/";

    fprintf(stdout, "%s", element.c_str());

  }

    std::string element = "";

    if(tracer.elements.back().prefix != "") {

      element += tracer.elements.back().prefix.c_str();
      element += ":";

    } else if(tracer.elements.back().uri == "http://www.sdml.info/srcML/src") {

      element += "src:";

    }

    element += tracer.elements.back().name.c_str();

    if(tracer.elements.back().name == "unit") {

      element += "[";

      if(tracer.diff_stack.back().operation == DELETE)
        element += tracer.filename_old;
      else
        element += tracer.filename_new;

      element += "]";

    } else if(is_collect(tracer.elements.back().name.c_str(), tracer.elements.back().prefix.c_str())) {

        element += "[src:signature(\"\"";
        element += tracer.elements.back().signature;
        element += "\")]";

    } else if(tracer.elements.size() > 1) {

      int count = tracer.elements.at(tracer.elements.size() - 2).children[std::string(tracer.elements.back().name)];

      char * buffer = (char *)malloc(sizeof(char) * count);

      snprintf(buffer, count + 1, "%d", count);

      element += "[";
      element += buffer;
      element += "]";

      free(buffer);

    }

    element += "\n";

    fprintf(stdout, "%s", element.c_str());

}

// index of attribute in attributes
int find_attribute_index(int nb_attributes, const xmlChar** attributes, const char* attribute) {

  for (int i = 0, index = 0; attributes[index]; ++i, index += 5)
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
