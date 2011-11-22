#include "srcMLUtility.hpp"
#include <Options.hpp>
#include <srcMLTranslator.hpp>
#include <Language.hpp>

// converts source code to srcML
xmlBuffer* translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir) {

  // register default language extensions
  Language::register_standard_file_extensions();

  // get language from file extension
  int language = Language::getLanguageFromFilename(source_file);

  // select basic options
  OPTION_TYPE options = OPTION_CPP_MARKUP_ELSE | OPTION_CPP | OPTION_XMLDECL | OPTION_XML  | OPTION_LITERAL | OPTION_OPERATOR | OPTION_MODIFIER;

  xmlBuffer* output_buffer = xmlBufferCreate();

  // create translator object
  srcMLTranslator translator(language, output_buffer, options);

  // set input file (must be done)
  translator.setInput(source_file);

  // translate file
  translator.translate(NULL, dir, source_file, NULL, language);

  // close the input file
  translator.close();

  return output_buffer;
}

