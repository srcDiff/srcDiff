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
#include "shortest_edit_script.h"
#include "srcDiffUtility.hpp"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffCommon.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffDiff.hpp"
#include "srcMLUtility.hpp"

#include "xmlrw.h"

const char* DIFF_PREFIX = "diff:";
const char* DIFF_OLD = "diff:delete";
const char* DIFF_NEW = "diff:insert";
const char* DIFF_COMMON = "diff:common";
const char* DIFF_TYPE = "type";

// diff nodes
xmlNode diff_common_start = { 0 };
xmlNode diff_common_end = { 0 };
xmlNode diff_old_start = { 0 };
xmlNode diff_old_end = { 0 };
xmlNode diff_new_start = { 0 };
xmlNode diff_new_end = { 0 };

xmlNs diff = { 0, XML_LOCAL_NAMESPACE, (const xmlChar *)"http://www.sdml.info/srcDiff", (const xmlChar *)"diff", 0 };

// diff attribute
xmlAttr diff_type = { 0 };
xmlNode change = { 0 };
xmlNode whitespace = { 0 };

// special flush node
xmlNode flush;

/*
  Global structures to store of the collected xml nodes.
  Base reference structure for all node comparison and output
*/
std::vector<xmlNode *> nodes_old;
std::vector<xmlNode *> nodes_new;

void outputNamespaces(xmlTextWriterPtr xout, const OPTION_TYPE& options, int depth, bool outer, const char** num2prefix);

// constructor
srcDiffTool::srcDiffTool(int language,                // programming language of source code
				 const char* src_encoding,    // text encoding of source code
				 const char* xml_encoding,    // xml encoding of result srcML file
				 const char* srcdiff_filename,  // filename of result srcDiff file
				 OPTION_TYPE global_options,             // many and varied options
				 const char* directory,       // root unit directory
				 const char* filename,        // root unit filename
				 const char* version,         // root unit version
				 const char* uri[],           // uri prefixes
				 int tabsize                  // size of tabs
				 )
  : first(true),
    root_directory(directory), root_filename(filename), root_version(version),
    encoding(src_encoding), language(language), global_options(global_options), uri(uri), tabsize(tabsize)
{

  // diff tags
  diff_common_start.name = (xmlChar *) DIFF_COMMON;
  diff_common_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_common_start.extra = 0;

  diff_common_end.name = (xmlChar *) DIFF_COMMON;
  diff_common_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_common_end.extra = 0;

  diff_old_start.name = (xmlChar *) DIFF_OLD;
  diff_old_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_old_start.extra = 0;

  diff_old_end.name = (xmlChar *) DIFF_OLD;
  diff_old_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_old_end.extra = 0;

  diff_new_start.name = (xmlChar *) DIFF_NEW;
  diff_new_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_new_start.extra = 0;

  diff_new_end.name = (xmlChar *) DIFF_NEW;
  diff_new_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_new_end.extra = 0;

  diff_type.name = (xmlChar *) DIFF_TYPE;
  diff_type.type = (xmlElementType)XML_ATTRIBUTE_NODE;

  change.name = (xmlChar *)"text";
  change.type = (xmlElementType)XML_READER_TYPE_TEXT;
  change.content = (xmlChar *)"change";
  change.extra = 0;

  whitespace.name = (xmlChar *)"text";
  whitespace.type = (xmlElementType)XML_READER_TYPE_TEXT;
  whitespace.content = (xmlChar *)"whitespace";
  whitespace.extra = 0;

  flush.name = (xmlChar *)"text";
  flush.type = (xmlElementType)XML_READER_TYPE_TEXT;
  flush.content = (xmlChar *)"";
  flush.extra = 0;

  writer = xmlNewTextWriterFilename(srcdiff_filename, 0);

  if (writer == NULL) {
    fprintf(stderr, "Unable to open file '%s' as XML", srcdiff_filename);

    exit(1);
  } 

  if(!isoption(global_options, OPTION_XMLDECL))
    xmlTextWriterStartDocument(writer, XML_VERSION, xml_encoding, XML_DECLARATION_STANDALONE);

  output_srcml_file = xmlBufferCreate();

}

// Translate from input stream to output stream
void srcDiffTool::translate(const char* path_one, const char* path_two, OPTION_TYPE local_options, 
                            const char* unit_directory, const char* unit_filename, const char* unit_version,
                            int language) {
  
  // root unit for compound srcML documents
  if (first && ((global_options & OPTION_NESTED) > 0)) {

    startUnit(0, global_options, root_directory, root_filename, root_version, uri, writer);
    xmlTextWriterWriteRawLen(writer, BAD_CAST "\n\n", 2);

  }

  first = false;

  // create the reader for the old file
  xmlTextReaderPtr reader_old = NULL;

  // translate file one
  //translate_to_srcML(path_one, 0, 0, output_srcml_file);
  translate_to_srcML(language, encoding, "UTF-8", output_srcml_file, local_options, unit_directory, path_one, unit_version, 0, 8);
  reader_old = xmlReaderForMemory((const char*) xmlBufferContent(output_srcml_file), output_srcml_file->use, 0, 0, 0);

  if (reader_old == NULL) {

    fprintf(stderr, "Unable to open file '%s' as XML", path_one);
    
    exit(1);
  }

  // read to unit
  xmlTextReaderRead(reader_old);

  xmlNodePtr unit_old = getRealCurrentNode(reader_old);

  // Read past unit tag open
  int is_old = xmlTextReaderRead(reader_old);

  // collect if non empty files
  xmlNodePtr unit_end = NULL;
  if(is_old) {

    collect_nodes(&nodes_old, reader_old);
    unit_end = getRealCurrentNode(reader_old);

  }

  xmlBufferEmpty(output_srcml_file);

  xmlFreeTextReader(reader_old);

  // group nodes
  std::vector<std::vector<int> *> node_set_old = create_node_set(nodes_old, 0, nodes_old.size());

  /*

    Input for file two

  */

  xmlTextReaderPtr reader_new = NULL;

    // translate file two
  //translate_to_srcML(path_two, 0, 0, output_srcml_file);
  translate_to_srcML(language, encoding,  "UTF-8", output_srcml_file, local_options, unit_directory, path_two, unit_version, 0, 8);

    // create the reader for the new file
    reader_new = xmlReaderForMemory((const char*) xmlBufferContent(output_srcml_file), output_srcml_file->use, 0, 0, 0);

  if (reader_new == NULL) {

    fprintf(stderr, "Unable to open file '%s' as XML", path_two);

    exit(1);
  }

  // read to unit
  xmlTextReaderRead(reader_new);

  xmlNodePtr unit_new = getRealCurrentNode(reader_new);

  int is_new = xmlTextReaderRead(reader_new);

  // collect if non empty files
  if(is_new) {

    collect_nodes(&nodes_new, reader_new);

    unit_end = getRealCurrentNode(reader_new);

  }

  xmlBufferEmpty(output_srcml_file);

  xmlFreeTextReader(reader_new);

  std::vector<std::vector<int> *> node_set_new = create_node_set(nodes_new, 0, nodes_new.size());

  /*

    Setup readers and writer.

   */

  // delete reader state
  reader_state rbuf_old = { 0 };
  rbuf_old.stream_source = DELETE;

  diff_set * new_diff = new diff_set;
  new_diff->operation = COMMON;
  rbuf_old.open_diff.push_back(new_diff);

  // insert reader state
  reader_state rbuf_new = { 0 };
  rbuf_new.stream_source = INSERT;

  new_diff = new diff_set;
  new_diff->operation = COMMON;
  rbuf_new.open_diff.push_back(new_diff);

  // writer state
  writer_state wstate = { 0 };
  wstate.writer = writer;

  new_diff = new diff_set;
  new_diff->operation = COMMON;
  wstate.output_diff.push_back(new_diff);

  /*

    Output srcDiff

  */

  Language l(language);
  startUnit(l.getLanguageString(), local_options, unit_directory, unit_filename, unit_version, uri, writer);


  // create srcdiff unit
  xmlNodePtr unit = create_srcdiff_unit(unit_old, unit_new);

  //// output srcdiff unit
  update_diff_stack(rbuf_old.open_diff, unit, COMMON);
  update_diff_stack(rbuf_new.open_diff, unit, COMMON);
  update_diff_stack(wstate.output_diff, unit, COMMON);

  // run on file level
  if(is_old || is_new)
  output_diffs(rbuf_old, &node_set_old, rbuf_new, &node_set_new, wstate);

  // output remaining whitespace
  output_white_space_all(rbuf_old, rbuf_new, wstate);

  // output srcdiff unit ending tag
  //if(is_old && is_new)
  //output_node(rbuf_old, rbuf_new, unit_end, COMMON, wstate);

  output_node(rbuf_old, rbuf_new, &flush, COMMON, wstate);

  if(isoption(global_options, OPTION_NESTED)) {

    xmlTextWriterEndElement(writer);
    xmlTextWriterWriteRawLen(writer, BAD_CAST "\n\n", 2);

  }

  free_node_sets(node_set_old);
  free_node_sets(node_set_new);

  // Because of grouping need to output a common to end grouping need to deallocate as well
  for(unsigned int i = 0; i < nodes_old.size(); ++i) {

    if((xmlReaderTypes)nodes_old.at(i)->type == XML_READER_TYPE_TEXT) {

      delete nodes_old.at(i);

    }
  }

  for(unsigned int i = 0; i < nodes_new.size(); ++i) {

    if((xmlReaderTypes)nodes_new.at(i)->type == XML_READER_TYPE_TEXT) {

      delete nodes_new.at(i);

    }
  }

  nodes_old.clear();
  nodes_new.clear();

}

// destructor
srcDiffTool::~srcDiffTool() {

  xmlTextWriterEndElement(writer);

  // cleanup writer                                                                                                                                  
  xmlTextWriterEndDocument(writer);
  xmlFreeTextWriter(writer);

  // free the buffer
  xmlBufferFree(output_srcml_file);

}

void srcDiffTool::startUnit(const char * language,
               OPTION_TYPE& options,             // many and varied options                                                               
               const char* directory,       // root unit directory                                                                   
               const char* filename,        // root unit filename                                                                    
               const char* version,         // root unit version                                                                     
               const char* uri[],           // uri prefixes                                                                          
               xmlTextWriterPtr writer) {

  static int depth = 0;

  // start of main tag
  std::string maintag = uri[0];
  if (!maintag.empty())
    maintag += ":";
  maintag += "unit";

  // start of main tag
  xmlTextWriterStartElement(writer, BAD_CAST maintag.c_str());

  // outer units have namespaces
  if (!isoption(options, OPTION_NAMESPACEDECL)) {
    outputNamespaces(writer, options, depth, true, uri);
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

void outputNamespaces(xmlTextWriterPtr xout, const OPTION_TYPE& options, int depth, bool outer, const char** num2prefix) {

    // figure out which namespaces are needed
    char const * const ns[] = {

      // main srcML namespace declaration always used
      (depth == 0) ? SRCML_SRC_NS_URI : 0,

      // main cpp namespace declaration
      isoption(OPTION_CPP, options) && (isoption(OPTION_NESTED, options) == !outer) ? SRCML_CPP_NS_URI : 0,

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
      if (num2prefix[i][0] != '\0') {
	prefix += ':';
	prefix += num2prefix[i];
      }

      xmlTextWriterWriteAttribute(xout, BAD_CAST prefix.c_str(), BAD_CAST ns[i]);
    }
}

