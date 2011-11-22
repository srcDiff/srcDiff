/*
  srcdiff.cpp

  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include "srcDiffTranslator.hpp"

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

#include "shortest_edit_script.h"
#include "srcDiffUtility.hpp"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffCommon.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffDiff.hpp"
#include "srcMLUtility.hpp"

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#ifdef __MINGW32__
#include "mingw32.hpp"
#endif

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


int srcdiff_translate(const char * filename_one, const char * filename_two, int options, xmlTextWriterPtr writer) {

  bool is_srcML = (bool)options;

  // TODO: Error handling? Is the return NULL if bad?

  /*

    Input for file one

  */

  // create the reader for the old file
  xmlTextReaderPtr reader_old = NULL;
  xmlBuffer * output_file = NULL;
  if(!is_srcML) {

    // translate file one
    output_file = translate_to_srcML(filename_one, 0, 0);
    reader_old = xmlReaderForMemory((const char*) xmlBufferContent(output_file), output_file->use, 0, 0, 0);

  } else {

    reader_old = xmlNewTextReaderFilename(filename_one);

  }

  if (reader_old == NULL) {

    fprintf(stderr, "Unable to open file '%s' as XML", filename_one);

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

  if(!is_srcML)
    xmlBufferEmpty(output_file);

  xmlFreeTextReader(reader_old);

  // group nodes
  std::vector<std::vector<int> *> node_set_old = create_node_set(nodes_old, 0, nodes_old.size());

  /*

    Input for file two

  */

  xmlTextReaderPtr reader_new = NULL;
  if(!is_srcML) {

    // translate file two
    output_file = translate_to_srcML(filename_two, 0, 0);

    // create the reader for the new file
    reader_new = xmlReaderForMemory((const char*) xmlBufferContent(output_file), output_file->use, 0, 0, 0);

  } else {

    reader_new = xmlNewTextReaderFilename(filename_two);

  }

  if (reader_new == NULL) {

    fprintf(stderr, "Unable to open file '%s' as XML", filename_two);

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
  if(!is_srcML)
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
  wstate.writer = writer;

  new_diff = new diff_set;
  new_diff->operation = COMMON;
  wstate.output_diff.push_back(new_diff);

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

  /*

    Output srcDiff

  */

  // create srcdiff unit
  xmlNodePtr unit = create_srcdiff_unit(unit_old, unit_new);

  // output srcdiff unit
  output_node(rbuf_old, rbuf_new, unit, COMMON, wstate);

  // run on file level
  if(is_old || is_new)
    output_diffs(rbuf_old, &node_set_old, rbuf_new, &node_set_new, wstate);

  // output remaining whitespace
  output_white_space_all(rbuf_old, rbuf_new, wstate);

  // output srcdiff unit ending tag
  if(is_old && is_new)
    output_node(rbuf_old, rbuf_new, unit_end, COMMON, wstate);

  nodes_old.clear();
  nodes_new.clear();

  return 0;
}
