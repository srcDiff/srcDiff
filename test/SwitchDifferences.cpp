/*
  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include "../bin/Options.hpp"
#include "../bin/srcMLTranslator.hpp"
#include "../bin/Language.hpp"
#include "shortest_edit_script.h"

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "xmlrw.h"
#include "diffrw.h"

// macros
#define SIZEPLUSLITERAL(s) sizeof(s) - 1, BAD_CAST s
#define LITERALPLUSSIZE(s) BAD_CAST s, sizeof(s) - 1

// stores information during xml Text Reader processing
struct reader_buffer {

  int line_number;
  unsigned char * characters;
  std::vector<xmlNode *> * buffer;
};

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

    // create the writer
    writer = xmlNewTextWriterFilename("/dev/stdout", 0);
    if (writer == NULL) {
      fprintf(stderr, "Unable to open file '%s' as XML", "/dev/stdout");

      return 1;
    }

    // issue the xml declaration
    xmlTextWriterStartDocument(writer, XML_VERSION, output_encoding, XML_DECLARATION_STANDALONE);


    xmlFreeTextReader(reader);


    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);

  return 0;
}

