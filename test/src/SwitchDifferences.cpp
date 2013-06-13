/*
  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include <Options.hpp>

#include "../../src/xmlrw.hpp"

const char * XML_VERSION = "1.0";
const char * output_encoding = "UTF-8";
const char * XML_DECLARATION_STANDALONE = "yes";

OPTION_TYPE options;

// macros
#define SIZEPLUSLITERAL(s) sizeof(s) - 1, BAD_CAST s
#define LITERALPLUSSIZE(s) BAD_CAST s, sizeof(s) - 1

const char * const UNIT_TAG = "unit";
const char * const DIFF_PREFIX = "diff";
const char * const DELETE_TAG = "delete";
const char * const INSERT_TAG = "insert";
const char * const TYPE_ATTR = "type";
const char * const CHANGE_ATTR_VALUE = "change";

const char * get_attr(xmlNodePtr node, const char * attribute) {

  for(xmlAttrPtr attr = node->properties; attr; attr = attr->next)
    if(strcmp((const char *)attr->name, attribute) == 0)
      return (const char *)attr->children->content;

  return 0;

}

int main(int argc, char * argv[]) {

  /*
    Create xmlreader and the xmlwriter
  */

  xmlTextReaderPtr reader = NULL;

  xmlTextWriterPtr writer = NULL;

  // create the reader for the old file
  reader = xmlNewTextReaderFilename("/dev/stdin");
  if (reader == NULL) {
    fprintf(stderr, "Unable to open file '%s' as XML", "/dev/stdin");

    return 1;
  }

  // create the writer
  writer = xmlNewTextWriterFilename("/dev/stdout", 0);
  if (writer == NULL) {
    fprintf(stderr, "Unable to open file '%s' as XML", "/dev/stdout");

    return 1;
  }

  // issue the xml declaration
  xmlTextWriterStartDocument(writer, XML_VERSION, output_encoding, XML_DECLARATION_STANDALONE);

  bool is_change = false;
  bool wait_end = false;
  bool output_saved = false;
  std::vector<xmlNodePtr> nodes;
  while(xmlTextReaderRead(reader) == 1) {

    xmlNodePtr node = xmlTextReaderCurrentNode(reader);

    if(node->ns && node->ns->prefix && strcmp((const char *)node->ns->prefix, DIFF_PREFIX) == 0) {

      if(strcmp((const char *)node->name, DELETE_TAG) == 0) {

        free((void *)node->name);
        node->name = (const xmlChar *)strdup(INSERT_TAG);

        if(!isendelement(reader) && get_attr(node, TYPE_ATTR) && strcmp(get_attr(node, TYPE_ATTR), CHANGE_ATTR_VALUE) == 0)
          is_change = true;

      } else if(strcmp((const char *)node->name, INSERT_TAG) == 0) {

        free((void *)node->name);
        node->name = (const xmlChar *)strdup(DELETE_TAG);

        if(is_change && !isendelement(reader) && get_attr(node, TYPE_ATTR) && strcmp(get_attr(node, TYPE_ATTR), CHANGE_ATTR_VALUE) == 0) {

          is_change = false;
          wait_end = true;

        }

        if(wait_end && isendelement(reader)) {

          wait_end = false;
          output_saved = true;

        }

      }

    }

    if(is_change)
      nodes.push_back(node);
    else {

      //outputNode(*node, writer);

    }

    if(output_saved) {

      for(int i = 0; i < nodes.size(); ++i) {

        //outputNode(*nodes[i], writer);

      }

      output_saved = false;
      nodes.clear();

    }

  }

  xmlFreeTextReader(reader);

  xmlTextWriterEndDocument(writer);
  xmlFreeTextWriter(writer);

  return 0;
}

