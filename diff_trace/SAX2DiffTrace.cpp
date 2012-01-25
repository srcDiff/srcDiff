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

const int COMMON = SESCOMMON;
const int INSERT = SESINSERT;
const int DELETE = SESDELETE;

SAX2DiffTrace::SAX2DiffTrace(long & options)
  : options(options) {

}

static element null_element;

static std::string collect_name_structures[] = { "function", "function_decl", "constructor", "constructor_decl", "destructor", "destructor_decl"
                                                 , "struct", "struct_decl", "class", "class_decl", "union", "union_decl"
                                                 , "decl_stmt", "call", "\0" };

static std::string collect_type_structures[] = { "decl_stmt", "\0" };

// helper method
int find_attribute_index(int nb_attributes, const xmlChar** attributes, const char* attribute);
std::string & trim_string(std::string & source);
std::string create_string_from_element(element & curelement, element & nextelement, int count, int operation, long & options);
std::string create_string_from_element_last_offset(element & curelement, element & nextelement, int offset, int operation, long & options);
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
  tracer.wait = false;
  tracer.collect = false;

  tracer.collect_name = false;
  tracer.wait_name = false;

  diff startdiff = { 0 };
  startdiff.operation = COMMON;

  tracer.diff_stack.push_back(startdiff);

  tracer.is_delete = false;
  tracer.is_insert = false;

  tracer.collect_text = false;
  tracer.collect_text_delete = false;
  tracer.collect_text_insert = false;

}

void SAX2DiffTrace::endDocument(void * ctx) {
  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  // fprintf(stderr, "%s\n\n", __FUNCTION__);

  fprintf(stdout, "\n");
}

bool SAX2DiffTrace::is_wait(const char * name, const char * prefix) {

  for(int i = 0; collect_name_structures[i][0]; ++i)
    if(collect_name_structures[i] == name)
      return true;

  return false;
}

bool SAX2DiffTrace::is_type(const char * name, const char * prefix) {

  for(int i = 0; collect_type_structures[i][0]; ++i)
    if(collect_type_structures[i] == name)
      return true;

  return false;

}


bool SAX2DiffTrace::is_collect(SAX2DiffTrace & tracer, const char * name, const char * prefix) {


  if(strcmp(name, "name") != 0)
    return false;

  unsigned int pos = tracer.elements.size() - 1;

  if(pos == tracer.collect_node_pos)
    return false;

  if(pos > 0) {

    for(; pos > tracer.collect_node_pos; --pos) {


      if(tracer.elements.at(pos).prefix != "diff" && tracer.elements.at(pos).name != "name") {

        bool is_decl_stmt = tracer.elements.at(tracer.collect_node_pos).name == "decl_stmt";
        bool is_function = tracer.elements.at(tracer.collect_node_pos).name == "function" || tracer.elements.at(tracer.collect_node_pos).name == "function";

        if(!is_decl_stmt && !is_function)
          break;

        else if(is_decl_stmt && tracer.elements.at(pos).name != "type" && tracer.elements.at(pos).name != "decl")
          break;

        else if(is_function && tracer.elements.at(pos).name != "type")
          break;

      }

    }

  } else {

    return false;

  }

  return pos == tracer.collect_node_pos;
}

bool SAX2DiffTrace::is_end_wait(SAX2DiffTrace & tracer, const char * name, const char * prefix, const char * context) {

  if((strcmp(context, "function") == 0 || strcmp(context, "function_decl") == 0) && strcmp(name, "parameter_list") == 0)
    return true;

  if((strcmp(context, "constructor") == 0 || strcmp(context, "constructor_decl") == 0) && strcmp(name, "parameter_list") == 0)
    return true;

  if((strcmp(context, "destructor") == 0 || strcmp(context, "destructor_decl") == 0) && strcmp(name, "parameter_list") == 0)
    return true;

  if((strcmp(context, "class") == 0 || strcmp(context, "struct") == 0 || strcmp(context, "union") == 0) && strcmp(name, "block") == 0)
    return true;

  if(strcmp(context, "decl_stmt") == 0 && (strcmp(name, "init") == 0 || strcmp(name, "block") == 0
                                           || (strcmp(name, "argument_list") == 0 && tracer.collect_node_pos == (tracer.elements.size() - 1))))
    return true;

  //if(strcmp(context, "decl") == 0 && strcmp(name, "init") == 0)
  //return true;

  //if(strcmp(context, "expr_stmt") == 0 && strcmp(name, "expr") == 0)
  //return true;

  if(strcmp(context, "call") == 0 && strcmp(name, "argument_list") == 0)
    return true;

  //if((strcmp(context, "class_decl") == 0 || strcmp(context, "struct_decl") == 0 || strcmp(context, "union_decl") == 0) && strcmp(name, "block") == 0)
  //return true;

  return false;
}

bool SAX2DiffTrace::is_end_collect(const char * name, const char * prefix, const char * context) {

  for(int i = 0; collect_name_structures[i][0]; ++i)
    if(collect_name_structures[i] == context && strcmp(name, "name") == 0)
      return true;

  return false;
}


// rename to end_collect
void SAX2DiffTrace::end_collect(SAX2DiffTrace & tracer) {

  // form paths
  for(int i = 0; i < tracer.signature_path_old.size(); ++i) {

    if(tracer.signature_path_old.at(i).empty())
      continue;

    std::string path = "";

    for(int j = 0; j < tracer.signature_path_old.at(i).size(); ++j) {

      if(j != 0)
        path += "/";

      path += tracer.signature_path_old.at(i).at(j) + "[last()";

      if(tracer.signature_path_offsets_old.at(i).at(j) != 0) {

        path +=  " - ";

        int temp_count = tracer.signature_path_offsets_old.at(i).at(j);
        int length;
        for(length = 0; temp_count > 0; temp_count /= 10, ++length)
          ;

        ++length;

        char * buffer = (char *)malloc(sizeof(char) * length);

        snprintf(buffer, length, "%d", tracer.signature_path_offsets_old.at(i).at(j));

        path += buffer;

        free(buffer);

      }

      path += "]";

    }

    tracer.elements.at(tracer.collect_node_pos).signature_path_old.at(i) = path;

  }

  tracer.signature_path_pos_old.clear();
  tracer.signature_path_old.clear();
  tracer.signature_path_offsets_old.clear();

  for(int i = 0; i < tracer.signature_path_new.size(); ++i) {

    if(tracer.signature_path_new.at(i).empty())
      continue;

    std::string path = "";

    for(int j = 0; j < tracer.signature_path_new.at(i).size(); ++j) {

      if(j != 0)
        path += "/";

      path += tracer.signature_path_new.at(i).at(j) + "[last()";

      if(tracer.signature_path_offsets_new.at(i).at(j) != 0) {

        path +=  " - ";

        int temp_count = tracer.signature_path_offsets_new.at(i).at(j);
        int length;
        for(length = 0; temp_count > 0; temp_count /= 10, ++length)
          ;

        ++length;

        char * buffer = (char *)malloc(sizeof(char) * length);

        snprintf(buffer, length, "%d", tracer.signature_path_offsets_new.at(i).at(j));

        path += buffer;

        free(buffer);

      }

      path += "]";

    }

    tracer.elements.at(tracer.collect_node_pos).signature_path_new.at(i) = path;

  }

  tracer.signature_path_pos_new.clear();
  tracer.signature_path_new.clear();
  tracer.signature_path_offsets_new.clear();

  tracer.wait = false;
  tracer.collect = false;

  if(!tracer.elements.at(tracer.collect_node_pos).signature_name_old.empty())
    trim_string(tracer.elements.at(tracer.collect_node_pos).signature_name_old.back());

  if(!tracer.elements.at(tracer.collect_node_pos).signature_name_new.empty())
    trim_string(tracer.elements.at(tracer.collect_node_pos).signature_name_new.back());

  if(tracer.output) {

    int num_missed = tracer.missed_diff_types.size();

    for(unsigned int i = 0; i < num_missed; ++i) {

      diff temp_diff = { 0 };
      temp_diff.operation = tracer.missed_diff_types.at(i);

      tracer.diff_stack.push_back(temp_diff);

      bool is_decl_stmt = tracer.elements.at(tracer.collect_node_pos).name == "decl_stmt";

      std::vector<element> save_elements;
      if(is_decl_stmt) {

        while(tracer.collect_node_pos < (tracer.elements.size() - 1)) {

            save_elements.push_back(tracer.elements.back());
            tracer.elements.pop_back();

          }

      }

      for(unsigned int j = 0; j < tracer.missed_diffs.at(i).size(); ++j)
        tracer.elements.push_back(tracer.missed_diffs.at(i).at(j));

      output_diff(tracer);

      for(unsigned int j = 0; j < tracer.missed_diffs.at(i).size(); ++j)
        tracer.elements.pop_back();

      if(is_decl_stmt) {

          while(!save_elements.empty()) {

            element save_element = save_elements.back();
            tracer.elements.push_back(save_element);
            save_elements.pop_back();

          }

      }

      tracer.diff_stack.pop_back();

    }

    tracer.missed_diff_types.clear();
    tracer.missed_diffs.clear();

    tracer.output = false;
  }

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

  tracer.collect_text = false;
  tracer.diff_stack.back().output_text = false;

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

    if(tracer.diff_stack.back().operation == COMMON) {

      tracer.collect_text_delete = false;
      tracer.collect_text_insert = false;

    } else if(tracer.diff_stack.back().operation == DELETE) {

      tracer.collect_text_delete = false;

    } else {

      tracer.collect_text_insert = false;

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

        std::string temp;

        curelement.signature_path_old.push_back(temp);
        curelement.signature_path_new.push_back(temp);

        curelement.signature_name_old.push_back(temp);
        curelement.signature_name_new.push_back(temp);

        if(sep != NULL) {

          *sep = '\0';

          curelement.signature_name_old.back() = filename;

          curelement.signature_name_new.back() = sep + 1;

          *sep = '|';

        } else {

          curelement.signature_name_old.back() = filename;

          curelement.signature_name_new.back() = filename;

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

    if(tracer.wait && is_end_wait(tracer, (const char *)localname, (const char *)prefix, tracer.elements.at(tracer.collect_node_pos).name.c_str())) {

      end_collect(tracer);

    }

    tracer.elements.push_back(curelement);

    if(curelement.name == "name") {

      tracer.collect_name = true;
      tracer.wait_name = true;
      tracer.collect_name_pos = tracer.elements.size() - 1;

      std::string temp;

      tracer.elements.back().signature_name_old.push_back(temp);
      tracer.elements.back().signature_name_new.push_back(temp);

    }

    if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0)
      ++tracer.diff_stack.back().level;

    if(tracer.wait)
      ++tracer.offset_pos;

    if(!tracer.wait && !tracer.wait_name) {

      if((tracer.wait = is_wait((const char *)localname, (const char *)prefix))) {

        tracer.collect_node_pos = tracer.elements.size() - 1;
        tracer.offset_pos = 0;

      }

    }


    if(tracer.wait && !tracer.collect) {

      if((tracer.collect = is_collect(tracer, (const char *)localname, (const char *)prefix))) {

        std::string temp;

        std::vector<int> offsets;
        std::vector<std::string> elements;

        tracer.signature_path_pos_old.push_back(offsets);
        tracer.signature_path_offsets_old.push_back(offsets);
        tracer.signature_path_old.push_back(elements);

        tracer.signature_path_pos_new.push_back(offsets);
        tracer.signature_path_offsets_new.push_back(offsets);
        tracer.signature_path_new.push_back(elements);

        tracer.elements.at(tracer.collect_node_pos).signature_path_old.push_back(temp);
        tracer.elements.at(tracer.collect_node_pos).signature_path_new.push_back(temp);

        tracer.elements.at(tracer.collect_node_pos).signature_name_old.push_back(temp);
        tracer.elements.at(tracer.collect_node_pos).signature_name_new.push_back(temp);

      } else {

      }

    }

    if(tracer.wait)
        update_offsets(tracer, tracer.offset_pos, tracer.diff_stack.back().operation);

    if(tracer.diff_stack.back().operation != COMMON && tracer.diff_stack.back().level == 1) {

      if(!tracer.wait && !tracer.wait_name)
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

  tracer.collect_text = false;
  tracer.diff_stack.back().output_text = false;

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

    if(strcmp((const char *)localname, "common") == 0
       || strcmp((const char *)localname, "delete") == 0
       || strcmp((const char *)localname, "insert") == 0) {

      tracer.diff_stack.pop_back();

    }

  }

  if(strcmp((const char *)localname, "name") == 0) {

    tracer.collect_name = false;
    tracer.wait_name = false;

    if(!tracer.wait)
      end_collect(tracer);

  }

  if(tracer.wait)
    --tracer.offset_pos;

  if(tracer.wait && is_wait((const char *)localname, (const char *)prefix)) {

    end_collect(tracer);

  }

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0 || !(tracer.options & OPTION_SRCML_RELATIVE)) {

    if(tracer.diff_stack.back().operation == COMMON) {

      tracer.collect_text_delete = false;
      tracer.collect_text_insert = false;

    } else if(tracer.diff_stack.back().operation == DELETE) {

      tracer.collect_text_delete = false;

    } else {

      tracer.collect_text_insert = false;

    }

    tracer.elements.pop_back();

  }

  if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0)
    --tracer.diff_stack.back().level;

  if(tracer.collect && is_end_collect((const char *)localname, (const char *)prefix, tracer.elements.at(tracer.collect_node_pos).name.c_str()))
    tracer.collect = false;

  tracer.collect_text = false;

}

void SAX2DiffTrace::update_offsets(SAX2DiffTrace & tracer, int offset, int operation) {

  if(operation == COMMON || operation == DELETE) {

    for(int j = tracer.signature_path_pos_old.size() - 1; j >= 0; --j) {

      if(tracer.signature_path_old.at(j).empty())
        continue;

      for(int i = 0; i < offset; ++i) {

        if(i >= tracer.signature_path_old.at(j).size())
          break;

        element curelement = tracer.elements.at(tracer.collect_node_pos + 1 + i);
        std::string path;
        if(curelement.prefix == "")
          path += "src:";
        else
          path += curelement.prefix + ":";

        path += curelement.name;

        if(tracer.signature_path_old.at(j).at(i) != path)
          break;

        if(curelement.prefix == "")
          path = curelement.name;

        if(i == (offset - 1) && tracer.signature_path_pos_old.at(j).at(i) != tracer.elements.at(tracer.collect_node_pos + i).children[path])
          ++tracer.signature_path_offsets_old.at(j).at(i);

        if(tracer.signature_path_pos_old.at(j).at(i) != tracer.elements.at(tracer.collect_node_pos + i).children[path])
          break;

      }

    }
    
  }

  if(operation == COMMON || operation == INSERT) {

    for(int j = tracer.signature_path_pos_new.size() - 1; j >= 0; --j) {

      if(tracer.signature_path_new.at(j).empty())
        continue;

      for(int i = 0; i < offset; ++i) {

        if(i >= tracer.signature_path_new.at(j).size())
          break;

        element curelement = tracer.elements.at(tracer.collect_node_pos + 1 + i);
        std::string path;
        if(curelement.prefix == "")
          path += "src:";
        else
          path += curelement.prefix + ":";

        path += curelement.name;

        if(tracer.signature_path_new.at(j).at(i) != path)
          break;

        if(curelement.prefix == "")
          path = curelement.name;

        if(i == (offset - 1) && tracer.signature_path_pos_new.at(j).at(i) != tracer.elements.at(tracer.collect_node_pos + i).children[path])
          ++tracer.signature_path_offsets_new.at(j).at(i);          

        if(tracer.signature_path_pos_new.at(j).at(i) != tracer.elements.at(tracer.collect_node_pos + i).children[path])
          break;

      }

    }
    
  }

}

void SAX2DiffTrace::characters(void* ctx, const xmlChar* ch, int len) {

  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr)ctx;
  SAX2DiffTrace & tracer = *(SAX2DiffTrace *)ctxt->_private;

  if(tracer.collect_name) {

    if(tracer.diff_stack.back().operation == COMMON) {

      tracer.elements.back().signature_name_old.at(tracer.collect_name_pos).append((const char *)ch, len);
      tracer.elements.back().signature_name_new.at(tracer.collect_name_pos).append((const char *)ch, len);

    } else if(tracer.diff_stack.back().operation == DELETE) {

      tracer.elements.back().signature_name_old.at(tracer.collect_name_pos).append((const char *)ch, len);

    } else if(tracer.diff_stack.back().operation == INSERT) {

      tracer.elements.back().signature_name_new.at(tracer.collect_name_pos).append((const char *)ch, len);

    }

  }
    
  if(tracer.collect) {

    std::vector<int> poss;
    std::vector<int> offsets;
    std::vector<std::string> paths;

    std::string path = "";

    // build the path
    if((tracer.diff_stack.back().operation == COMMON && (tracer.signature_path_old.back().empty() || tracer.signature_path_new.back().empty())
        || (tracer.diff_stack.back().operation == DELETE && tracer.signature_path_old.back().empty())
        || tracer.diff_stack.back().operation == INSERT && tracer.signature_path_new.back().empty())) {

      for(int pos = tracer.collect_node_pos + 1; pos < tracer.elements.size(); ++pos) {

        int count = 0;
        std::string tag;
        if(tracer.elements.at(pos).prefix != "") {

          tag += tracer.elements.at(pos).prefix;
          tag += ":";

        }

        tag += tracer.elements.at(pos).name;

        if(!(tracer.options & OPTION_SRCML_RELATIVE))
          count = tracer.elements.at(pos - 1).children[tag];
        else if(tracer.diff_stack.back().operation == DELETE)
          count = tracer.elements.at(pos - 1).children_old[tag];
        else if(tracer.diff_stack.back().operation == INSERT)
          count = tracer.elements.at(pos - 1).children_new[tag];

        if(tracer.elements.at(pos).prefix == "" && tracer.elements.at(pos).uri == "http://www.sdml.info/srcML/src") {

          tag = "src:" + tag;

        }

        int position = pos - tracer.collect_node_pos + 1;

        poss.push_back(count);
        offsets.push_back(0);
        paths.push_back(tag);

      }

      if(tracer.diff_stack.back().operation == COMMON) {

        tracer.elements.at(tracer.collect_node_pos).signature_path_old.back() = path;
        tracer.elements.at(tracer.collect_node_pos).signature_path_new.back() = path;

        bool is_empty = tracer.signature_path_old.back().empty();

        if(is_empty) {

          tracer.signature_path_pos_old.back() = poss;
          tracer.signature_path_offsets_old.back() = offsets;
          tracer.signature_path_old.back() = paths;

        }

        is_empty = tracer.signature_path_new.back().empty();

        if(is_empty) {

          tracer.signature_path_pos_new.back() = poss;
          tracer.signature_path_offsets_new.back() = offsets;
          tracer.signature_path_new.back() = paths;

        }

      } else if(tracer.diff_stack.back().operation == DELETE) {

        if(tracer.signature_path_old.back().empty()) {

          tracer.elements.at(tracer.collect_node_pos).signature_path_old.back() = path;

          tracer.signature_path_pos_old.back() = poss;
          tracer.signature_path_offsets_old.back() = offsets;
          tracer.signature_path_old.back() = paths;

        }

      } else if(tracer.diff_stack.back().operation == INSERT) {

        if(tracer.signature_path_new.back().empty()) {

          tracer.elements.at(tracer.collect_node_pos).signature_path_new.back() = path;

          tracer.signature_path_pos_new.back() = poss;
          tracer.signature_path_offsets_new.back() = offsets;
          tracer.signature_path_new.back() = paths;

        }

      }

    }

    if(tracer.diff_stack.back().operation == COMMON) {


      tracer.elements.at(tracer.collect_node_pos).signature_name_old.back().append((const char *)ch, (const char *)ch + len);
      tracer.elements.at(tracer.collect_node_pos).signature_name_new.back().append((const char *)ch, (const char *)ch + len);

      tracer.elements.at(tracer.collect_node_pos).signature_path_old.back() = path;
      tracer.elements.at(tracer.collect_node_pos).signature_path_new.back() = path;

    } else if(tracer.diff_stack.back().operation == DELETE) {

      tracer.elements.at(tracer.collect_node_pos).signature_name_old.back().append((const char *)ch, (const char *)ch + len);

      tracer.elements.at(tracer.collect_node_pos).signature_path_old.back() = path;

    } else if(tracer.diff_stack.back().operation == INSERT) {

      tracer.elements.at(tracer.collect_node_pos).signature_name_new.back().append((const char *)ch, (const char *)ch + len);

      tracer.elements.at(tracer.collect_node_pos).signature_path_new.back() = path;

    }

  }

  std::string tag = "text()";

  if(!tracer.collect_text)
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

  } else if(tracer.diff_stack.back().operation == INSERT) {

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
     && len != 0 && ((tracer.options & OPTION_WHITESPACE) || (!tracer.collect_text))
     && tracer.diff_stack.back().level == 0) {

    tracer.diff_stack.back().text_num = tracer.elements.back().children[tag];
    tracer.diff_stack.back().output_text = true;

  }

  if(tracer.diff_stack.back().operation != COMMON
     && len != 0 && ((tracer.options & OPTION_WHITESPACE)
                     || (tracer.diff_stack.back().output_text && tracer.elements.back().children[tag] == tracer.diff_stack.back().text_num && i != len))
     && tracer.diff_stack.back().level == 0) {

    tracer.diff_stack.back().output_text = false;

    element curelement;
    curelement.name = "text()";
    curelement.prefix = "";
    curelement.uri = "";

    tracer.elements.push_back(curelement);

    if(!tracer.wait)
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

  tracer.collect_text = true;

}

void SAX2DiffTrace::comments(void* ctx, const xmlChar* ch) {

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

}

std::string create_string_from_element(element & curelement, element & nextelement, int count, int operation, long & options) {

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

    element += "[";
    element += buffer;
    element += "]";

    free(buffer);

  } else {

    element += "[1]";

  }

  if(curelement.name == "unit"
     && !curelement.signature_name_old.empty()
     && !curelement.signature_name_new.empty()) {

    element += "[@filename='";

    if(!(options & OPTION_SRCML_RELATIVE)) {

      element += curelement.signature_name_old.back();

      if(curelement.signature_name_old.back() != curelement.signature_name_new.back()) {

        element += "|";
        element += curelement.signature_name_new.back();

      }

    } else {

      if(operation == DELETE)
        element += curelement.signature_name_old.back();
      else
        element += curelement.signature_name_new.back();

    }

    element += "']";


  } else {

    bool collected = false;

    for(int i = 0; !collected && collect_name_structures[i][0]; ++i)
      if(curelement.name == collect_name_structures[i])
        collected = true;

    if(collected) {

      for(int i = 0; i < curelement.signature_name_old.size(); ++i) {

        if(curelement.signature_name_old.at(i) != "") {

          element += "[";
          element += curelement.signature_path_old.at(i) + "='" + curelement.signature_name_old.at(i) + "'";
          element += "]";
        }

        if(curelement.signature_name_new.at(i) != ""
           && ((curelement.signature_name_old.at(i) + curelement.signature_path_old.at(i)) != (curelement.signature_name_new.at(i) + curelement.signature_path_new.at(i)))) {

          element += "[";
          element += curelement.signature_path_new.at(i) + "='" + curelement.signature_name_new.at(i) + "'";
          element += "]";

        }

      }

    }

  }

  return element;

}

void output_diff(SAX2DiffTrace & tracer) {

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

    element += create_string_from_element(tracer.elements.at(i), next_element, count, tracer.diff_stack.back().operation, tracer.options);

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
