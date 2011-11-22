/*
  srcdiff.cpp

  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/


#include "srcDiffTranslator.hpp"

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

int main(int argc, char * argv[]) {

  /*

    Setup output file

  */

  // create the writer
  xmlTextWriterPtr writer = NULL;
  writer = xmlNewTextWriterFilename(srcdiff_file, 0);
  if (writer == NULL) {
    fprintf(stderr, "Unable to open file '%s' as XML", srcdiff_file);

    exit(1);
  }

  // issue the xml declaration
  xmlTextWriterStartDocument(writer, XML_VERSION, output_encoding, XML_DECLARATION_STANDALONE);

  int status = srcdiff_translate(argc, argv, writer);

  // cleanup writer
  xmlTextWriterEndDocument(writer);
  xmlFreeTextWriter(writer);

  return status;
}
