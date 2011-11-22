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

void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns);
void merge_filename(xmlNodePtr unit_old, xmlNodePtr unit_new);

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlNodePtr unit_old, xmlNodePtr unit_new) {

  // get units from source code
  xmlNodePtr unit = unit_old;

  // add diff namespace
  addNamespace(&unit->nsDef, &diff);

  merge_filename(unit, unit_new);

  return unit;
}

void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns) {

  xmlNsPtr namespaces = *nsDef;

  if(namespaces) {

    for(; namespaces->next; namespaces = namespaces->next)
      ;

    namespaces->next = ns;
  }
  else
    *nsDef = ns;

}

void merge_filename(xmlNodePtr unit_old, xmlNodePtr unit_new) {

  xmlNodePtr unit = unit_old;

  std::string filename_old = "";
  xmlAttrPtr attr;
  for(attr = unit->properties; attr; attr = attr->next)
    if(strcmp((const char *)attr->name, "filename") == 0) {

      filename_old += (const char *)attr->children->content;
      break;
    }

  std::string filename_new = "";
  xmlAttrPtr attr_new;
  for(attr_new = unit_new->properties; attr_new; attr_new = attr_new->next)
    if(strcmp((const char *)attr_new->name, "filename") == 0) {

      filename_new += (const char *)attr_new->children->content;
      break;
    }

  std::string * filename = NULL;
  if(attr && attr_new) {

    if(filename_old == filename_new)
      return;

    filename = new std::string(filename_old + "|" + filename_new);
    attr->children->content = (xmlChar *)filename->c_str();
    return;

  }

  if(attr_new) {
    
    attr = unit->properties;
    if(attr) {
      
      for(; attr->next; attr = attr->next)
        ;
      
      attr->next = attr_new;
      
    } else {
      
      unit->properties = attr_new;
    }
  }

}

