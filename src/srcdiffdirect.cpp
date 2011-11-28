
/*
  srcdiff.cpp

  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/


#include <string.h>
#include "srcDiffTool.hpp"
#include <Options.hpp>
#include <srcMLTranslator.hpp>
#include <Language.hpp>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

int main(int argc, char * argv[]) {

  // test for correct input
  if(argc < 3) {

    fprintf(stderr, "Usage: srcdiff oldFile newFile dir\n");
    return 1;
  }

  const char * srcdiff_file = "-";

  Language::register_standard_file_extensions();

  OPTION_TYPE options = OPTION_CPP_MARKUP_ELSE | OPTION_CPP | OPTION_XMLDECL | OPTION_XML  | OPTION_LITERAL | OPTION_OPERATOR | OPTION_MODIFIER;

  //srcDiffTool translator = srcDiffTool(1, srcdiff_file, options);

  return 0;
}
