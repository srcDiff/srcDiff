#include <srcMLUtility.hpp>
#include <srcml.h>
#ifdef SVN
#include <svn_io.hpp>
#endif

#include <string.h>

extern xmlNs diff;



// converts source code to srcML
void srcml_translator::translate(const char * path, const char * language, const char * filename, const char * directory, const char * version) {

  if(path == 0 || path[0] == 0 || path[0] == '@') throw no_file_exception();

  srcml_archive * unit_archive = srcml_clone_archive(archive);
  srcml_archive_disable_option(unit_archive, SRCML_OPTION_ARCHIVE | SRCML_OPTION_HASH);

  srcml_write_open_memory(unit_archive, output_buffer, output_size);

  srcml_unit * unit = srcml_create_unit(unit_archive);

#ifdef SVN
  if(!isoption(options, OPTION_SVN)) {
#endif

    srcml_parse_unit_filename(unit, path);

#ifdef SVN
  } else {

    void * context = svnReadOpen(path);

    const char * end = index(path, '@');
    const char * filename = strndup(path, end - path);
    srcml_unit_set_language(unit, srcml_archive_check_extension(unit_archive, filename));
    free((void *)filename);

    srcml_parse_unit_io(unit, context, svnRead, svnReadClose);

}
#endif

  srcml_write_unit(unit_archive, unit);

  srcml_free_unit(unit);

  srcml_close_archive(unit_archive);
  srcml_free_archive(unit_archive);

}

void srcml_translator::create_nodes_from_srcml(pthread_mutex_t * mutex, std::vector<xNode *> & nodes) {
  
  char * output_buffer;
  int output_size;

  xmlTextReaderPtr reader = NULL;

  translate(path, archive, language, filename, directory, version, &output_buffer, &output_size, options);

  reader = xmlReaderForMemory(output_buffer, output_size, 0, 0, XML_PARSE_HUGE);

  if (reader == NULL) {

    if(!isoption(srcml_archive_get_options(archive), OPTION_QUIET))
       fprintf(stderr, "Unable to open file '%s' as XML\n", "output_buffer");

    exit(1);
  }

  // read to unit
  xmlTextReaderRead(reader);

  // Read past unit tag open
  if(xmlTextReaderRead(reader) == 0) throw std::string("Error reading srcML.");

  // collect if non empty files
  collect_nodes(&nodes, reader, srcml_archive_get_options(archive), context, mutex);

  xmlFreeTextReader(reader);

  free(output_buffer);

}

bool srcml_translator::is_separate_token(const char character) {


  return character == '(' || character ==')' || character == '[' || character == ']' || character == ',';

}


// collect the differnces
void srcml_translator::collect_nodes(std::vector<xNode *> * nodes, xmlTextReaderPtr reader, OPTION_TYPE options, int context, pthread_mutex_t * mutex) {

  std::vector<std::string> element_stack;
  element_stack.push_back("unit");

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
        else if(is_separate_token(*characters)) {

          ++characters;

          // Copy the remainder after (
          text = split_text(characters_start, characters);

        } else {

	  // collect all 
          while((*characters) != 0 && !isspace(*characters) && !is_separate_token(*characters))
            ++characters;

          /*
	  // break up ( and )
          if((characters_start + 1) && (*characters_start) == '(' && (*(characters_start + 1)) == ')') {

            xNode * atext = split_text(characters_start, characters_start + 1);
            nodes->push_back(atext);
            ++characters_start;

          }
          */

          // Copy the remainder after (
          text = split_text(characters_start, characters);

        }

        nodes->push_back(text);

      }
    }
    else {

      // text node does not need to be copied.
      pthread_mutex_lock(mutex);
      xNodePtr node = getRealCurrentNode(reader, options, context);
      pthread_mutex_unlock(mutex);

      if(node->type == (xmlElementType)XML_READER_TYPE_ELEMENT)
        node->parent = strdup(element_stack.back().c_str());

      if(node->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (node->extra & 0x1) == 0)
        element_stack.push_back(node->name);
      else if(node->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        element_stack.pop_back();

      if(strcmp((const char *)node->name, "unit") == 0)
        return;

      // save non-text node and get next node
      nodes->push_back(node);

    }

    not_done = xmlTextReaderRead(reader);

  }

}

// check if node is a indivisable group of three (atomic)
bool srcml_translator::is_atomic_srcml(std::vector<xNodePtr> * nodes, unsigned start) {

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

