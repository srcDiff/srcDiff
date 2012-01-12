#include "srcMLUtility.hpp"
#include "srcDiffDiff.hpp"
#include <srcMLTranslator.hpp>
#include <Language.hpp>

extern xmlNs diff;

// converts source code to srcML
void translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir, xmlBuffer* output_buffer) {

  // get language from file extension
  int language = Language::getLanguageFromFilename(source_file);

  // select basic options
  OPTION_TYPE options = OPTION_CPP_MARKUP_ELSE | OPTION_CPP | OPTION_XMLDECL | OPTION_XML  | OPTION_LITERAL | OPTION_OPERATOR | OPTION_MODIFIER;

  // create translator object
  srcMLTranslator translator(language, output_buffer, options);

  // set input file (must be done)
  translator.setInput(source_file);

  // translate file
  translator.translate(NULL, dir, source_file, NULL, language);

  // close the input file
  translator.close();

}

void translate_to_srcML(int language, const char* src_encoding, const char* xml_encoding, xmlBuffer* output_buffer, OPTION_TYPE& options,
                const char* directory, const char* filename, const char* version, const char* uri[], int tabsize) {

  // create translator object
  srcMLTranslator translator(language, src_encoding, xml_encoding, output_buffer, options, directory, filename, version, uri, tabsize);

  try {

  // set input file (must be done)
  translator.setInput(filename);

  // translate file
  translator.translate(filename, directory, filename, version, language);

  } catch(...) {

  // close the input file
  translator.close();
  throw FileError();

  }

  // close the input file
  translator.close();

}

void * create_nodes_from_srcML_thread(void * arguments) {

  create_nodes_args & args = *(create_nodes_args *)arguments;

    create_nodes_from_srcML(args.language, args.src_encoding, args.xml_encoding, args.output_buffer, args.options,
    args.directory, args.filename, args.version, args.uri, args.tabsize,
                            args.nodes, args.unit_start, args.no_error, args.context);

    return NULL;

}


void create_nodes_from_srcML(int language, const char* src_encoding, const char* xml_encoding, xmlBuffer* output_buffer, OPTION_TYPE& options,
                             const char* directory, const char* filename, const char* version, const char* uri[], int tabsize,
                             std::vector<xNode *> & nodes, xNodePtr * unit_start, int & no_error, int context) {
  
  xmlTextReaderPtr reader = NULL;
  xNodePtr unit_end = NULL;
  //std::vector<std::vector<int> *> node_set;

  // translate file one
  try {

    if(!filename || filename[0] == 0)
      throw FileError();

  translate_to_srcML(language, src_encoding, xml_encoding, output_buffer, options, directory, filename, version, uri, 8);

  reader = xmlReaderForMemory((const char*) xmlBufferContent(output_buffer), output_buffer->use, 0, 0, 0);

  if (reader == NULL) {

    if(!isoption(options, OPTION_QUIET))
       fprintf(stderr, "Unable to open file '%s' as XML\n", filename);

    exit(1);
  }

  // read to unit
  xmlTextReaderRead(reader);

  *unit_start = getRealCurrentNode(reader, options, context);

  // Read past unit tag open
  no_error = xmlTextReaderRead(reader);

  // collect if non empty files
  if(no_error) {

    collect_nodes(&nodes, reader, options, context);
    unit_end = getRealCurrentNode(reader, options, context);

  }

  xmlFreeTextReader(reader);

  // group nodes
  //node_set = create_node_set(nodes, 0, nodes.size());

  } catch(...) {

    no_error = -1;

    //if(!isoption(global_options, OPTION_QUIET))
    //fprintf(stderr, "Unable to open file '%s'\n", filename);
    
  }

  xmlBufferEmpty(output_buffer);

}

// create srcdiff unit
xNodePtr create_srcdiff_unit(xNodePtr unit_old, xNodePtr unit_new) {

  // get units from source code
  xNodePtr unit = unit_old;

  // add diff namespace
  //addNamespace(&unit->nsDef, &diff);

  //merge_filename(unit, unit_new);

  return unit;
}

/*
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

void merge_filename(xNodePtr unit_old, xNodePtr unit_new) {

  xNodePtr unit = unit_old;

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
*/
// collect the differnces
void collect_nodes(std::vector<xNode *> * nodes, xmlTextReaderPtr reader, OPTION_TYPE & options, int context) {

  int not_done = 1;
  while(not_done) {

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      const char * characters = (const char *)xmlTextReaderConstValue(reader);

      // cycle through characters
      for (; (*characters) != 0;) {

        const char * characters_start = characters;

        xNode * text;

        // separate new line
        if(*characters == '\n') {

          ++characters;
          text = split_text(characters_start, characters);
        }

        // separate non-new line whitespace
        else if(isspace(*characters)) {

          //while((*characters) != 0 && *characters != '\n' && isspace(*characters))
            ++characters;

	    // kind of want a look up table for this
	    //const char * content = strndup((const char *)characters_start, characters  - characters_start);
            text = split_text(characters_start, characters);

        }

        // separate non whitespace
        else {

	  // collect all 
          while((*characters) != 0 && !isspace(*characters))
            ++characters;

	  // break up ( and )
          if((characters_start + 1) && (*characters_start) == '(' && (*(characters_start + 1)) == ')') {

            xNode * atext = split_text(characters_start, characters_start + 1);
            nodes->push_back(atext);
            ++characters_start;

          }

          // Copy the remainder after (
          text = split_text(characters_start, characters);

        }

        nodes->push_back(text);

      }
    }
    else {

      // text node does not need to be copied.
      xNodePtr node = getRealCurrentNode(reader, options, context);

      if(strcmp((const char *)node->name, "unit") == 0)
        return;

      // save non-text node and get next node
      nodes->push_back(node);

    }

    not_done = xmlTextReaderRead(reader);

  }

}

// check if node is a indivisable group of three (atomic)
bool is_atomic_srcml(std::vector<xNodePtr> * nodes, unsigned start) {

  static const char * atomic[] = { "name", "operator", "literal", "modifier", 0 };

  if((start + 2) >= nodes->size())
    return false;

  if((xmlReaderTypes)nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if((xmlReaderTypes)nodes->at(start + 2)->type != XML_READER_TYPE_END_ELEMENT)
    return false;

  if(strcmp((const char *)nodes->at(start)->name, (const char *)nodes->at(start + 2)->name) != 0)
    return false;

  for(int i = 0; atomic[i]; ++i)
    if(strcmp((const char *)nodes->at(start)->name, atomic[i]) == 0)
      return true;

  return false;
}

