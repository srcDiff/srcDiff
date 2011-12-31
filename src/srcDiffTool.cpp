/*
  srcDiffTool.cpp

  Copyright (C) 2011  SDML (www.sdml.info)

  This file is part of the srcDiff translator.

  The srcDiff translator is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcDiff translator is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcDiff translator; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Class for straightforward translation from source code to srcDiff
*/

#include "srcDiffTool.hpp"
#include "srcmlns.hpp"
#include "srcmlapps.hpp"
#include "srcMLTranslator.hpp"
#include "shortest_edit_script.h"
#include "srcDiffUtility.hpp"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffCommon.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffDiff.hpp"
#include "srcMLUtility.hpp"
#include "pthread.h"

#include "xmlrw.hpp"

const char* DIFF_PREFIX = "diff:";
const char* DIFF_OLD = "delete";
const char* DIFF_NEW = "insert";
const char* DIFF_COMMON = "common";
const char* DIFF_TYPE = "type";

// diff nodes
xNode diff_common_start;
xNode diff_common_end;
xNode diff_old_start;
xNode diff_old_end;
xNode diff_new_start;
xNode diff_new_end;

xNs diff = {"http://www.sdml.info/srcDiff", "diff"};

// diff attribute
xAttr diff_type = { 0 };
const char * change = "change";
const char * whitespace = "whitespace";

// special flush node
xNode flush;

// constructor
srcDiffTool::srcDiffTool(int language,                // programming language of source code
                         const char* src_encoding,    // text encoding of source code
                         const char* xml_encoding,    // xml encoding of result srcML file
                         const char* srcdiff_filename,  // filename of result srcDiff file
                         OPTION_TYPE global_options,             // many and varied options
                         METHOD_TYPE method,
                         const char* directory,       // root unit directory
                         const char* filename,        // root unit filename
                         const char* version,         // root unit version
                         const char* uri[],           // uri prefixes
                         int tabsize                  // size of tabs
                         )
  : first(true),
    root_directory(directory), root_filename(filename), root_version(version),
    src_encoding(src_encoding), xml_encoding(xml_encoding), language(language), global_options(global_options), method(method), uri(uri), tabsize(tabsize), rbuf_old(DELETE), rbuf_new(INSERT)
{

  diff.prefix = uri[7];

  // diff tags
  diff_common_start.name = DIFF_COMMON;
  diff_common_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_common_start.ns = &diff;
  diff_common_start.extra = 0;

  diff_common_end.name = DIFF_COMMON;
  diff_common_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_common_end.ns = &diff;
  diff_common_end.extra = 0;

  diff_old_start.name = DIFF_OLD;
  diff_old_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_old_start.ns = &diff;
  diff_old_start.extra = 0;

  diff_old_end.name = DIFF_OLD;
  diff_old_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_old_end.ns = &diff;
  diff_old_end.extra = 0;

  diff_new_start.name = DIFF_NEW;
  diff_new_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_new_start.ns = &diff;
  diff_new_start.extra = 0;

  diff_new_end.name = DIFF_NEW;
  diff_new_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_new_end.ns = &diff;
  diff_new_end.extra = 0;

  diff_type.name = DIFF_TYPE;
  //diff_type.type = (xmlElementType)XML_ATTRIBUTE_NODE;

  flush.name = "text";
  flush.type = (xmlElementType)XML_READER_TYPE_TEXT;
  flush.content = "";
  flush.extra = 0;


  // writer state
  wstate.writer = xmlNewTextWriterFilename(srcdiff_filename, 0);

  wstate.method = method;

  if (wstate.writer == NULL) {
    fprintf(stderr, "Unable to open file '%s' as XML\n", srcdiff_filename);

    exit(1);
  }

  output_srcml_file_old = xmlBufferCreate();
  output_srcml_file_new = xmlBufferCreate();

}

// Translate from input stream to output stream
void srcDiffTool::translate(const char* path_one, const char* path_two, OPTION_TYPE local_options,
                            const char* unit_directory, const char* unit_filename, const char* unit_version,
                            int language) {

  // root unit for compound srcML documents
  if (first && ((global_options & OPTION_NESTED) > 0)) {

    startUnit(0, global_options, root_directory, root_filename, root_version, uri, wstate.writer);
    xmlTextWriterWriteRawLen(wstate.writer, BAD_CAST "\n\n", 2);

  }

  first = false;

  // create the reader for the old file
  xNodePtr unit_old = 0;
  std::vector<std::vector<int> *> node_set_old;

  int is_old = 0;
  create_nodes_args args_old = { language, src_encoding, xml_encoding, output_srcml_file_old, local_options
                                 , unit_directory, path_one, unit_version, uri, 8
                                 , rbuf_old.nodes, &unit_old, is_old };
  pthread_t thread_old;
  if(pthread_create(&thread_old, NULL, create_nodes_from_srcML_thread, (void *)&args_old)) {

    is_old = -2;
    
  }

  if(!isoption(global_options, OPTION_THREAD) && is_old != -2 && pthread_join(thread_old, NULL)) {

    is_old = -2;

  }

  /*

    Input for file two

  */

  xNodePtr unit_new = 0;
  std::vector<std::vector<int> *> node_set_new;

  int is_new = 0;
  create_nodes_args args_new = { language, src_encoding, xml_encoding, output_srcml_file_new, local_options
                                 , unit_directory, path_two, unit_version, uri, 8
                                 , rbuf_new.nodes, &unit_new, is_new };

  pthread_t thread_new;
  if(pthread_create(&thread_new, NULL, create_nodes_from_srcML_thread, (void *)&args_new)) {

    is_new = -2;

  }

  if(isoption(global_options, OPTION_THREAD) && is_old != -2 && pthread_join(thread_old, NULL)) {

    is_old = -2;

  }

  if(is_new != -2 && pthread_join(thread_new, NULL)) {

    is_new = -2;

  }

  if(is_old && is_old > -1)
    node_set_old = create_node_set(rbuf_old.nodes, 0, rbuf_old.nodes.size());


  if(is_new && is_new > -1)
    node_set_new = create_node_set(rbuf_new.nodes, 0, rbuf_new.nodes.size());

  /*

    Setup readers and writer.

  */

  diff_set old_diff;
  old_diff.operation = COMMON;
  rbuf_old.open_diff.push_back(&old_diff);

  diff_set new_diff;
  new_diff.operation = COMMON;
  rbuf_new.open_diff.push_back(&new_diff);

  diff_set output_diff;
  output_diff.operation = COMMON;
  wstate.output_diff.push_back(&output_diff);

  /*
    unsigned int i;
    for(i = 0; i < rbuf_old.nodes.size() && i < rbuf_new.nodes.size(); ++i) {

    if(rbuf_old.nodes.at(i)->type != rbuf_new.nodes.at(i)->type)
    break;

    if((xmlReaderTypes)rbuf_old.nodes.at(i)->type != XML_READER_TYPE_TEXT
    && (xmlReaderTypes)rbuf_old.nodes.at(i)->type != XML_READER_TYPE_SIGNIFICANT_WHITESPACE)
    continue;

    if(strcmp(rbuf_old.nodes.at(i)->name, rbuf_new.nodes.at(i)->name) != 0)
    break;

    if(strcmp(rbuf_old.nodes.at(i)->content, rbuf_new.nodes.at(i)->content) != 0)
    break;

    }

    if(i == rbuf_old.nodes.size() && i == rbuf_new.nodes.size()) {

    for(i = 0; i < rbuf_old.nodes.size(); ++i)
    outputNode(*rbuf_old.nodes.at(i), wstate.writer);


    } else {
  */

  // create srcdiff unit
  //xNodePtr unit = create_srcdiff_unit(unit_old, unit_new);

  /*

    Output srcDiff

  */

  // output srcdiff unit
  if(!rbuf_old.nodes.empty() && !rbuf_new.nodes.empty()) {

    update_diff_stack(rbuf_old.open_diff, unit_old, COMMON);
    update_diff_stack(rbuf_new.open_diff, unit_new, COMMON);
    update_diff_stack(wstate.output_diff, unit_old, COMMON);

  } else if(rbuf_old.nodes.empty() && rbuf_new.nodes.empty()) {

    update_diff_stack(rbuf_old.open_diff, &diff_common_start, COMMON);
    update_diff_stack(rbuf_new.open_diff, &diff_common_start, COMMON);
    update_diff_stack(wstate.output_diff, &diff_common_start, COMMON);

    if(is_old <= -1 && is_new <= -1) {

      fprintf(stderr, "Error with file '%s' and file '%s'\n", path_one, path_two);

      exit(STATUS_INPUTFILE_PROBLEM);

    }

  } else if(rbuf_old.nodes.empty()) {

    update_diff_stack(rbuf_old.open_diff, &diff_common_start, COMMON);
    update_diff_stack(rbuf_new.open_diff, unit_new, COMMON);
    update_diff_stack(wstate.output_diff, unit_new, COMMON);

  } else {

    update_diff_stack(rbuf_old.open_diff, unit_old, COMMON);
    update_diff_stack(rbuf_new.open_diff, &diff_common_start, COMMON);
    update_diff_stack(wstate.output_diff, unit_old, COMMON);

  }

  Language l(language);
  startUnit(l.getLanguageString(), local_options, unit_directory, unit_filename, unit_version, uri, wstate.writer);

  // run on file level
  if(is_old || is_new)
    output_diffs(rbuf_old, &node_set_old, rbuf_new, &node_set_new, wstate);

  // output remaining whitespace
  output_white_space_all(rbuf_old, rbuf_new, wstate);

  // output srcdiff unit ending tag
  //if(is_old && is_new)
  //output_node(rbuf_old, rbuf_new, unit_end, COMMON, wstate);

  output_node(rbuf_old, rbuf_new, &flush, COMMON, wstate);

  free_node_sets(node_set_old);
  free_node_sets(node_set_new);

  // }

  if(isoption(global_options, OPTION_NESTED)) {

    xmlTextWriterEndElement(wstate.writer);
    xmlTextWriterWriteRawLen(wstate.writer, BAD_CAST "\n\n", 2);

  }

  if(unit_old && unit_old->free)
    freeXNode(unit_old);

  if(unit_new && unit_new->free)
    freeXNode(unit_new);

  // Because of grouping need to output a common to end grouping need to deallocate as well
  for(unsigned int i = 0; i < rbuf_old.nodes.size(); ++i) {

    if(rbuf_old.nodes.at(i)->free) {

      freeXNode(rbuf_old.nodes.at(i));

    }
  }

  for(unsigned int i = 0; i < rbuf_new.nodes.size(); ++i) {

    if(rbuf_new.nodes.at(i)->free) {

      freeXNode(rbuf_new.nodes.at(i));

    }
  }

  rbuf_old.clear();
  rbuf_new.clear();
  wstate.clear();

}

// destructor
srcDiffTool::~srcDiffTool() {

  xmlTextWriterEndElement(wstate.writer);

  // cleanup writer
  xmlTextWriterEndDocument(wstate.writer);
  xmlFreeTextWriter(wstate.writer);

  // free the buffer
  xmlBufferFree(output_srcml_file_old);
  xmlBufferFree(output_srcml_file_new);

}

void srcDiffTool::startUnit(const char * language,
                            OPTION_TYPE& options,             // many and varied options
                            const char* directory,       // root unit directory
                            const char* filename,        // root unit filename
                            const char* version,         // root unit version
                            const char* uri[],           // uri prefixes
                            xmlTextWriterPtr writer) {

  static int depth = 0;

  if(depth == 0 && !isoption(global_options, OPTION_XMLDECL))
    xmlTextWriterStartDocument(writer, XML_VERSION, xml_encoding, XML_DECLARATION_STANDALONE);

  // start of main tag
  std::string maintag = uri[0];
  if (!maintag.empty())
    maintag += ":";
  maintag += "unit";

  // start of main tag
  xmlTextWriterStartElement(writer, BAD_CAST maintag.c_str());

  // outer units have namespaces
  if (!isoption(options, OPTION_NAMESPACEDECL)) {
    outputNamespaces(options, depth);
  }

  // list of attributes
  const char* const attrs[][2] = {

    // language attribute
    { UNIT_ATTRIBUTE_LANGUAGE, language },

    // directory attribute
    { UNIT_ATTRIBUTE_DIRECTORY, depth == 0 && isoption(global_options, OPTION_DIRECTORY) ? root_directory : directory },

    // filename attribute
    { UNIT_ATTRIBUTE_FILENAME, depth == 0 && isoption(global_options, OPTION_FILENAME) ? root_filename : filename },

    // version attribute
    { UNIT_ATTRIBUTE_VERSION,  depth == 0 && isoption(global_options, OPTION_VERSION) ? root_version : version },

    // position tab setting
    //{ tabattribute.c_str(), isoption(options, OPTION_POSITION) ? stabs.str().c_str() : 0 },

  };

  // output attributes
  for (unsigned int i = 0; i < sizeof(attrs) / sizeof(attrs[0]); ++i) {
    if (!attrs[i][1])
      continue;

    xmlTextWriterWriteAttribute(writer, BAD_CAST attrs[i][0], BAD_CAST attrs[i][1]);
  }

  ++depth;

}

void srcDiffTool::outputNamespaces(const OPTION_TYPE& options, int depth) {

  // figure out which namespaces are needed
  char const * const ns[] = {

    // main srcML namespace declaration always used
    (depth == 0) ? SRCML_SRC_NS_URI : 0,

    // main cpp namespace declaration
    isoption(OPTION_CPP, options) && !isoption(OPTION_NESTED, options) ? SRCML_CPP_NS_URI : 0,

    // optional debugging xml namespace
    (depth == 0) && isoption(OPTION_DEBUG, options)    ? SRCML_ERR_NS_URI : 0,

    // optional literal xml namespace
    (depth == 0) && isoption(OPTION_LITERAL, options)  ? SRCML_EXT_LITERAL_NS_URI : 0,

    // optional operator xml namespace
    (depth == 0) && isoption(OPTION_OPERATOR, options) ? SRCML_EXT_OPERATOR_NS_URI : 0,

    // optional modifier xml namespace
    (depth == 0) && isoption(OPTION_MODIFIER, options) ? SRCML_EXT_MODIFIER_NS_URI : 0,

    // optional position xml namespace
    (depth == 0) && isoption(OPTION_POSITION, options) ? SRCML_EXT_POSITION_NS_URI : 0,

    // optional diff xml namespace
    (depth == 0) ? SRCML_DIFF_NS_URI : 0,
  };

  // output the namespaces
  for (unsigned int i = 0; i < sizeof(ns) / sizeof(ns[0]); ++i) {
    if (!ns[i])
      continue;

    std::string prefix = "xmlns";
    if (uri[i][0] != '\0') {
      prefix += ':';
      prefix += uri[i];
    }

    xmlTextWriterWriteAttribute(wstate.writer, BAD_CAST prefix.c_str(), BAD_CAST ns[i]);
  }
}

void srcDiffTool::set_nested(bool is_nested) {

  if(is_nested)
    global_options |= OPTION_NESTED;
  else
    global_options &= ~OPTION_NESTED;
}

void srcDiffTool::set_root_directory(const char * root_directory) {


  this->root_directory = root_directory;
}
