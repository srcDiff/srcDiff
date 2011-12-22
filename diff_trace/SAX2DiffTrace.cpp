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
#include "difftraceapps.hpp"
#include "../src/shortest_edit_script.h"

SAX2DiffTrace::SAX2DiffTrace(long & options)
  : options(options) {

}

// helper method
int find_attribute_index(int nb_attributes, const xmlChar** attributes, const char* attribute);
std::string & trim_string(std::string & source);

xmlSAXHandler SAX2DiffTrace::factory() {

  xmlSAXHandler sax = { 0 };

  sax.initialized = XML_SAX2_MAGIC;

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

  tracer.is_delete = false;
  tracer.is_insert = false;

  tracer.collect_text_delete = false;
  tracer.collect_text_insert = false;


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

  if(strcmp(name, "class") == 0)
    return true;

  if(strcmp(name, "struct") == 0)
    return true;

  if(strcmp(name, "union") == 0)
    return true;

  return false;
}

static bool is_end_collect(const char * name, const char * prefix, const char * context) {

  if((strcmp(context, "function") == 0 || strcmp(context, "function_decl") == 0) && strcmp(name, "parameter_list") == 0)
    return true;

  if((strcmp(context, "class") == 0 || strcmp(context, "struct") == 0 || strcmp(context, "union") == 0) && strcmp(name, "name") == 0)
    return true;

  return false;
}

void add_child(std::map<std::string, int> & children, std::string & child) {

  std::map<std::string, int>::iterator pos = children.find(child);

  if(pos != children.end()) {

    ++children[child];

  } else {

    children[child] = 1;

  }

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

      curdiff.operation = DELETE;

    } else if(strcmp((const char *)localname, "insert") == 0) {

      curdiff.operation = INSERT;

    }


    tracer.diff_stack.push_back(curdiff);

  }

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0 || !(tracer.options & OPTION_SRCML_RELATIVE)) {

    if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0) {

      if(tracer.diff_stack.back().operation == COMMON) {

        tracer.collect_text_delete = false;
        tracer.collect_text_insert = false;

      } else if(tracer.diff_stack.back().operation == DELETE) {

        tracer.collect_text_delete = false;

      } else {

        tracer.collect_text_insert = false;

      }
    }

    element curelement;
    curelement.name = (const char *)localname;
    if(prefix)
      curelement.prefix = (const char *)prefix;
    else
      curelement.prefix = "";

    curelement.uri = (const char *)URI;

    if(strcmp((const char *)localname, "unit") == 0) {

      int idx = find_attribute_index(nb_attributes, attributes, "filename");

      if(idx != -1) {

        int length = attributes[idx + 4] - attributes[idx + 3];

        const char * filename = strndup((const char *)attributes[idx + 3], length);

        char * sep = index(filename, '|');

        if(sep < (filename + length)) {

          *sep = '\0';

          curelement.signature_old = filename;

          curelement.signature_new = sep + 1;

          *sep = '|';

        } else {

          curelement.signature_old = filename;

          curelement.signature_new = filename;

        }

      }

    }

    if(tracer.elements.size() > 0) {

      std::string tag;
      if(prefix && strcmp((const char *)prefix, "") != 0) {

        tag += (const char *)prefix;
        tag += ":";

      }
      tag += (const char *)localname;

      add_child(tracer.elements.back().children, tag);

      if(tracer.diff_stack.back().operation == COMMON) {

        add_child(tracer.elements.back().children_old, tag);
        add_child(tracer.elements.back().children_new, tag);

      } else if(tracer.diff_stack.back().operation == DELETE) {

        add_child(tracer.elements.back().children_old, tag);

      } else {

        add_child(tracer.elements.back().children_new, tag);

      }

    }

    tracer.elements.push_back(curelement);

    if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0)
      ++tracer.diff_stack.back().level;

    if(!tracer.collect) {

      if((tracer.collect = is_collect((const char *)localname, (const char *)prefix)))
        tracer.collect_node_pos = tracer.elements.size() - 1;

    }

    if(tracer.diff_stack.back().operation != COMMON && tracer.diff_stack.back().level == 1) {


      if(!tracer.collect)
        output_diff(tracer);

      else {

        std::vector<element> temp_stack;

        for(unsigned int i = tracer.collect_node_pos + 1; i < tracer.elements.size(); ++i)
          temp_stack.push_back(tracer.elements.at(i));

        tracer.missed_diff_types.push_back(tracer.diff_stack.back().operation);
        tracer.missed_diffs.push_back(temp_stack);

        tracer.output = true;

      }
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

      tracer.diff_stack.pop_back();

    }

  }
  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0) {

    if(tracer.diff_stack.back().operation == COMMON) {

      tracer.collect_text_delete = false;
      tracer.collect_text_insert = false;

    } else if(tracer.diff_stack.back().operation == DELETE) {

      tracer.collect_text_delete = false;

    } else {

      tracer.collect_text_insert = false;

    }
  }

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0 || !(tracer.options & OPTION_SRCML_RELATIVE))
    tracer.elements.pop_back();

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0)
    --tracer.diff_stack.back().level;

  if(tracer.collect && is_end_collect((const char *)localname, (const char *)prefix, tracer.elements.at(tracer.collect_node_pos).name.c_str())) {

    if(strcmp((const char *)localname, "name") == 0) {

      std::string pre = "./";
      if(!prefix || strcmp((const char *)prefix, "") == 0) {

        if(strcmp((const char *)URI, "http://www.sdml.info/srcML/src") == 0)
          pre += "src:";

      } else {

        pre += (const char *)prefix;
        pre += ":";

      }

      pre += "name='";

      tracer.elements.at(tracer.collect_node_pos).signature_old = pre + tracer.elements.at(tracer.collect_node_pos).signature_old + "'";
      tracer.elements.at(tracer.collect_node_pos).signature_new = pre + tracer.elements.at(tracer.collect_node_pos).signature_new + "'";

    }

    trim_string(tracer.elements.at(tracer.collect_node_pos).signature_old);
    trim_string(tracer.elements.at(tracer.collect_node_pos).signature_new);

    tracer.collect = false;

    if(tracer.output) {

      int num_missed = tracer.missed_diff_types.size();

      for(unsigned int i = 0; i < num_missed; ++i) {

        diff temp_diff = { 0 };
        temp_diff.operation = tracer.missed_diff_types.at(i);

        tracer.diff_stack.push_back(temp_diff);

        for(unsigned int j = 0; j < tracer.missed_diffs.at(i).size(); ++j)
          tracer.elements.push_back(tracer.missed_diffs.at(i).at(j));

        output_diff(tracer);

        for(unsigned int j = 0; j < tracer.missed_diffs.at(i).size(); ++j)
          tracer.elements.pop_back();

        tracer.diff_stack.pop_back();

      }

      tracer.missed_diff_types.clear();
      tracer.missed_diffs.clear();

      tracer.output = false;

    }
  }

}

void SAX2DiffTrace::characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  SAX2DiffTrace & tracer = *(SAX2DiffTrace *)ctxt->_private;

  if(tracer.collect) {

    if(tracer.diff_stack.back().operation == COMMON) {

      tracer.elements.at(tracer.collect_node_pos).signature_old.append((const char *)ch, (const char *)ch + len);
      tracer.elements.at(tracer.collect_node_pos).signature_new.append((const char *)ch, (const char *)ch + len);

    } else if(tracer.diff_stack.back().operation == DELETE)
      tracer.elements.at(tracer.collect_node_pos).signature_old.append((const char *)ch, (const char *)ch + len);
    else
      tracer.elements.at(tracer.collect_node_pos).signature_new.append((const char *)ch, (const char *)ch + len);

  }

  std::string tag = "text()";

  add_child(tracer.elements.back().children, tag);

  if(tracer.diff_stack.back().operation == COMMON) {

    if(!tracer.collect_text_delete)
      add_child(tracer.elements.back().children_old, tag);

    if(!tracer.collect_text_insert)
      add_child(tracer.elements.back().children_new, tag);

    tracer.collect_text_delete = true;
    tracer.collect_text_insert = true;

  } else if(tracer.diff_stack.back().operation == DELETE) {

    if(!tracer.collect_text_delete)
      add_child(tracer.elements.back().children_old, tag);

    tracer.collect_text_delete = true;

  } else {

    if(!tracer.collect_text_insert)
      add_child(tracer.elements.back().children_new, tag);

    tracer.collect_text_insert = true;

  }

  int i;
  for(i = 0; i < len; ++i) {

    if(!isspace((char)ch[i]))
      break;

  }

  if(tracer.diff_stack.back().operation != COMMON
     && ((tracer.options & OPTION_WHITESPACE) || (len != 0 && i != len))
     && tracer.diff_stack.back().level == 0) {

    element curelement;
    curelement.name = "text()";
    curelement.prefix = "";
    curelement.uri = "";

    if((tracer.options & OPTION_WHITESPACE) && i == len)
      if(tracer.diff_stack.back().operation == DELETE)
        for(int i = 0; i < len; ++i)
          curelement.signature_old += (char)ch[i];

    tracer.elements.push_back(curelement);

    if(!tracer.collect)
      output_diff(tracer);
    else {

      std::vector<element> temp_stack;

      for(unsigned int i = tracer.collect_node_pos + 1; i < tracer.elements.size(); ++i)
        temp_stack.push_back(tracer.elements.at(i));

      tracer.missed_diff_types.push_back(tracer.diff_stack.back().operation);
      tracer.missed_diffs.push_back(temp_stack);

      tracer.output = true;

    }

    tracer.elements.pop_back();

  }

}

void SAX2DiffTrace::comments(void* ctx, const xmlChar* ch) {

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

}

std::string create_string_from_element(element & curelement, element & nextelement, int count, int operation) {
  std::string element = "";

  if(curelement.prefix != "") {

    element += curelement.prefix.c_str();
    element += ":";

  } else if(curelement.uri == "http://www.sdml.info/srcML/src") {

    element += "src:";

  }
  element += curelement.name.c_str();

  if(count > 0) {

    int temp_count = count;
    int length;
    for(length = 0; temp_count > 0; temp_count /= 10, ++length)
      ;

    if(count == 0)
      ++length;

    ++length;

    char * buffer = (char *)malloc(sizeof(char) * length);

    snprintf(buffer, length, "%d", count);

    element += "[position()=";
    element += buffer;
    //element += "]";

    free(buffer);

  }

  if(curelement.name == "unit"
     && ((operation == DELETE && curelement.signature_old != "")
         || (operation == INSERT && curelement.signature_new != ""))) {

    element += "[@filename='";

    if(operation == DELETE)
      element += curelement.signature_old;
    else
      element += curelement.signature_new;

    element += "'";

  } else if(strcmp(curelement.name.c_str(), "function") == 0
            || strcmp(curelement.name.c_str(), "function_decl") == 0) {

    element += " and src:signature('";
    if(operation == DELETE)
      element += curelement.signature_old;
    else
      element += curelement.signature_new;
    element += "')";

  } else if(strcmp(curelement.name.c_str(), "class") == 0
            || strcmp(curelement.name.c_str(), "struct") == 0
            || strcmp(curelement.name.c_str(), "union") == 0) {

    element += " and ";
    if(operation == DELETE)
      element += curelement.signature_old;
    else
      element += curelement.signature_new;

  } else if(strcmp(curelement.name.c_str(), "text()") == 0
            && (strcmp(curelement.signature_old.c_str(), "") != 0
                || strcmp(curelement.signature_new.c_str(), "") != 0)) {

    element += " and fn:contains(., '";
    if(operation == DELETE)
      element += curelement.signature_old;
    else
      element += curelement.signature_new;
    element += "')";

  }

  /* else if(strcmp(nextelement.name.c_str(), "") == 0) {

     element += " and ";
     if(operation == DELETE)
     element += "deleted()";
     else
     element += "inserted()";

     }*/

  element += "]";

  return element;

}

void output_diff(SAX2DiffTrace & tracer) {

  static element null_element;

  if(tracer.options & OPTION_SRCML_RELATIVE) {

    if(tracer.diff_stack.back().operation == DELETE)
      fprintf(stdout, "delete(");
    else
      fprintf(stdout, "insert(");

  }

  for(unsigned int i = 0; i < tracer.elements.size(); ++i) {

    int count = 0;
    if(i > 0) {

      std::string tag;
      if(tracer.elements.at(i).prefix != "") {

        tag += tracer.elements.at(i).prefix;
        tag += ":";

      }

      tag += tracer.elements.at(i).name;

      if(!(tracer.options & OPTION_SRCML_RELATIVE))
        count = tracer.elements.at(i - 1).children[tag];
      else if(tracer.diff_stack.back().operation == DELETE)
        count = tracer.elements.at(i - 1).children_old[tag];
      else
        count = tracer.elements.at(i - 1).children_new[tag];

    }

    element next_element = null_element;
    if((i + 1) < tracer.elements.size())
      next_element = tracer.elements.at(i + 1);

    std::string element = "/";

    element += create_string_from_element(tracer.elements.at(i), next_element, count, tracer.diff_stack.back().operation);


    fprintf(stdout, "%s", element.c_str());

  }

  if(tracer.options & OPTION_SRCML_RELATIVE) {

    fprintf(stdout, "%s", ")");

  }

  fprintf(stdout, "%s", "\n");


}

// index of attribute in attributes
int find_attribute_index(int nb_attributes, const xmlChar** attributes, const char* attribute) {

  for (int i = 0, index = 0; i < nb_attributes; ++i, index += 5)
    if (strcmp((const char*) attributes[index], attribute) == 0)
      return index;

  return -1;
}

// trim spaces from end of string
std::string & trim_string(std::string & source) {

  //fprintf(stderr, "%s\n", source.c_str());

  std::string::iterator pos;
  for(pos = source.begin(); pos != source.end() && (pos + 1) != source.end();)
    if(isspace(*pos) && isspace(*(pos + 1)))
      pos = source.erase(pos);
    else if(isspace(*pos) && *(pos + 1) == ',')
      pos = source.erase(pos);
    else
      ++pos;

  return source;
}
