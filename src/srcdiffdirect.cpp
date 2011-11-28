
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

  int language = Language::getLanguageFromFilename(argv[1]);

  OPTION_TYPE options = OPTION_CPP_MARKUP_ELSE | OPTION_CPP | OPTION_XMLDECL | OPTION_XML | OPTION_LITERAL | OPTION_OPERATOR | OPTION_MODIFIER;

  std::string filename = argv[1];
  filename += "|";
  filename += argv[2];

  const char * uri[] = {

    "",
    "cpp",
    "err", 
    "lit",
    "op",
    "type", 
    "pos", 
    "diff", 

  };

  srcDiffTool translator = srcDiffTool(language, "ISO-8859-1", "UTF-8", srcdiff_file, options, 0, 0, 0, uri, 8);
  translator.translate(argv[1], argv[2], options, 0, 0 /*filename.c_str()*/, 0, language);

  return 0;
}
