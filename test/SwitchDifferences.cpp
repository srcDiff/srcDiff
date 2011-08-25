/*
  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "../src/xmlrw.h"
#include "../src/diffrw.h"

// macros
#define SIZEPLUSLITERAL(s) sizeof(s) - 1, BAD_CAST s
#define LITERALPLUSSIZE(s) BAD_CAST s, sizeof(s) - 1

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

    bool in_out_diff = false;
    bool exited_out_diff = false;
    bool wait_out_diff = false;
    std::vector<xmlNode *> buffer;
    while(xmlTextReaderRead(reader) == 1) {

      xmlNodePtr node = getRealCurrentNode(reader);

      if(strcmp((const char *)node->name, "old") == 0) {

        if(in_out_diff)
          exited_out_diff = true;

        in_out_diff = !in_out_diff;
        continue;
      }

      if(exited_out_diff) {

        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));

        for(int i = 0; i < buffer.size(); ++i)
          outputNode(*buffer[i], writer);

        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

        exited_out_diff = false;
        buffer = std::vector<xmlNode *>();
      } else if(in_out_diff) {

        buffer.push_back(node);
        continue;
      }

        outputNode(*node, writer);
    }

    xmlFreeTextReader(reader);

    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);

  return 0;
}

