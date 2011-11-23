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

xmlNode diff_common_start;
xmlNode diff_common_end;
xmlNode diff_old_start;
xmlNode diff_old_end;
xmlNode diff_new_start;
xmlNode diff_new_end;

/*
  Global structures to store of the collected xml nodes.
  Base reference structure for all node comparison and output
*/
std::vector<xmlNode *> nodes_old;
std::vector<xmlNode *> nodes_new;

// constructor
srcDiffTool::srcDiffTool(int language, const char* srcml_filename, OPTION_TYPE& op)
  : 
  out(0, srcml_filename, "", DEFAULT_XML_ENCODING, op, (const char**)DEFAULT_URI_PREFIX, 8, 0),
    first(true),
    root_directory(""), root_filename(""), root_version(""),
    encoding(DEFAULT_TEXT_ENCODING), options(op)
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

}

// constructor
srcDiffTool::srcDiffTool(int language,                // programming language of source code
				 const char* src_encoding,    // text encoding of source code
				 const char* xml_encoding,    // xml encoding of result srcML file
				 const char* srcml_filename,  // filename of result srcML file
				 OPTION_TYPE& op,             // many and varied options
				 const char* directory,       // root unit directory
				 const char* filename,        // root unit filename
				 const char* version,         // root unit version
				 const char* uri[],           // uri prefixes
				 int tabsize                  // size of tabs
				 )
  : out(0, srcml_filename, "", xml_encoding, op, uri, tabsize, 0),
    first(true),
    root_directory(directory), root_filename(filename), root_version(version),
    encoding(src_encoding), options(op)
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

}

// close the output
void srcDiffTool::close() {

  close();
}

// Translate from input stream to output stream
void srcDiffTool::translate(const char* path_one, const char* path_two, const char* unit_directory,
				const char* unit_filename, const char* unit_version,
				int language) {

  // root unit for compound srcML documents
  if (first && ((options & OPTION_NESTED) > 0))
    out.startUnit(0, root_directory, root_filename, root_version, true);

  // create the reader for the old file
  xmlTextReaderPtr reader_old = NULL;
  xmlBuffer * output_file = xmlBufferCreate();

  // translate file one
  translate_to_srcML(path_one, 0, 0, output_file);
  reader_old = xmlReaderForMemory((const char*) xmlBufferContent(output_file), output_file->use, 0, 0, 0);

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

  xmlBufferEmpty(output_file);

  xmlFreeTextReader(reader_old);

  // group nodes
  std::vector<std::vector<int> *> node_set_old = create_node_set(nodes_old, 0, nodes_old.size());

  /*

    Input for file two

  */

  xmlTextReaderPtr reader_new = NULL;

    // translate file two
    translate_to_srcML(path_two, 0, 0, output_file);

    // create the reader for the new file
    reader_new = xmlReaderForMemory((const char*) xmlBufferContent(output_file), output_file->use, 0, 0, 0);

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

  // free the buffer
    xmlBufferFree(output_file);

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
  wstate.writer = out.xout;

  new_diff = new diff_set;
  new_diff->operation = COMMON;
  wstate.output_diff.push_back(new_diff);

  /*

    Output srcDiff

  */

  // create srcdiff unit
  //  xmlNodePtr unit = create_srcdiff_unit(unit_old, unit_new);

  // output srcdiff unit
  //  output_node(rbuf_old, rbuf_new, unit, COMMON, wstate);

  // run on file level
  if(is_old || is_new)
    output_diffs(rbuf_old, &node_set_old, rbuf_new, &node_set_new, wstate);

  // output remaining whitespace
  output_white_space_all(rbuf_old, rbuf_new, wstate);

  // output srcdiff unit ending tag
  //  if(is_old && is_new)
    //    output_node(rbuf_old, rbuf_new, unit_end, COMMON, wstate);

  nodes_old.clear();
  nodes_new.clear();


  first = false;

}

// destructor
srcDiffTool::~srcDiffTool() {

}
