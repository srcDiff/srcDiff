
/*
  srcdiff.cpp

  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/


#include "srcDiffTranslator.hpp"

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

// TODO:  Get the output encoding from the input
const char* XML_DECLARATION_STANDALONE = "yes";
const char* XML_VERSION = "1.0";

const char* output_encoding = "UTF-8";

int main(int argc, char * argv[]) {

  // test for correct input
  if(argc < 3) {

    fprintf(stderr, "Usage: srcdiff oldFile newFile dir\n");
    return 1;
  }

  int is_srcML = strcmp(argv[1], "--srcml") == 0;

  char * filename_one;
  char * filename_two;
  if(!is_srcML) {
    filename_one = argv[1];
    filename_two = argv[2];
  } else {
    filename_one = argv[2];
    filename_two = argv[3];

  }
    

  const char * srcdiff_file = "-";

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

  int status = srcdiff_translate(filename_one, filename_two, is_srcML, writer);

  // cleanup writer
  xmlTextWriterEndDocument(writer);
  xmlFreeTextWriter(writer);

  return status;
}
