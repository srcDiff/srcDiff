#include <srcml_converter.hpp>

#include <string>

#include <libxml/xmlreader.h>

static std::mutex mutex;

srcml_converter::srcml_converter(srcml_archive * archive, int stream_source) 
  : archive(archive), stream_source(stream_source), output_buffer(0) {}

srcml_converter::~srcml_converter() {

  if(output_buffer) free(output_buffer);

}

// converts source code to srcML
void srcml_converter::convert(const std::string & path, void * context,
                              std::function<int(void *, char *, int)> read, std::function<int(void *)> close,
                              const OPTION_TYPE & options) {

  srcml_archive * unit_archive = srcml_clone_archive(archive);
  srcml_archive_disable_option(unit_archive, SRCML_OPTION_ARCHIVE | SRCML_OPTION_HASH);

  srcml_write_open_memory(unit_archive, &output_buffer, &output_size);

  srcml_unit * unit = srcml_create_unit(unit_archive);

  srcml_unit_set_language(unit, srcml_archive_check_extension(unit_archive, path.c_str()));

  srcml_parse_unit_io(unit, context, *read.target<int (*) (void *, char *, int)>(), *close.target<int (*) (void *)>());

  srcml_write_unit(unit_archive, unit);

  srcml_free_unit(unit);

  srcml_close_archive(unit_archive);
  srcml_free_archive(unit_archive);

}

std::vector<xNodePtr> srcml_converter::create_nodes() {
  
  xmlTextReaderPtr reader = xmlReaderForMemory(output_buffer, output_size, 0, 0, XML_PARSE_HUGE);

  if (reader == NULL) throw std::string("Unable to open srcML output_buffer as XML");

  // read to unit
  xmlTextReaderRead(reader);

  // Read past unit tag open
  if(xmlTextReaderRead(reader) == 0) throw std::string("Error reading srcML.");

  // collect if non empty files
  std::vector<xNodePtr> nodes = collect_nodes(reader);

  xmlFreeTextReader(reader);

  return nodes;

}

static bool is_separate_token(const char character) {


  return character == '(' || character ==')' || character == '[' || character == ']' || character == ',';

}

// check if node is a indivisable group of three (atomic)
static bool is_atomic_srcml(std::vector<xNodePtr> & nodes, unsigned start) {

  static const char * atomic[] = { "name", "operator", "literal", "modifier", 0 };

  if((start + 2) >= nodes.size())
    return false;

  if((xmlReaderTypes)nodes.at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if((xmlReaderTypes)nodes.at(start + 2)->type != XML_READER_TYPE_END_ELEMENT)
    return false;

  if(strcmp((const char *)nodes.at(start)->name, (const char *)nodes.at(start + 2)->name) != 0)
    return false;

  for(int i = 0; atomic[i]; ++i)
    if(strcmp((const char *)nodes.at(start)->name, atomic[i]) == 0)
      return true;

  return false;
}


// collect the differences
std::vector<xNodePtr> srcml_converter::collect_nodes(xmlTextReaderPtr reader) {

  std::vector<xNodePtr> nodes;

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

        xNodePtr text;

        // separate new line
        if(*characters == '\n') {

          ++characters;
          text = split_text(characters_start, characters);
        }

        // separate non-new line whitespace
        else if(isspace(*characters)) {

          //while((*characters) != 0 && *characters != '\n' && isspace(*characters))
            ++characters;

            text = split_text(characters_start, characters);

        }

        // separate non whitespace
        else if(is_separate_token(*characters)) {

          ++characters;

          // Copy the remainder after (
          text = split_text(characters_start, characters);

        } else {

          while((*characters) != 0 && !isspace(*characters) && !is_separate_token(*characters))
            ++characters;

          // Copy the remainder after (
          text = split_text(characters_start, characters);

        }

        nodes.push_back(text);

      }
    }
    else {

      // text node does not need to be copied.
      mutex.lock();
      xNodePtr node = getRealCurrentNode(reader, srcml_archive_get_options(archive), stream_source);
      mutex.unlock();

      if(node->type == (xmlElementType)XML_READER_TYPE_ELEMENT)
        node->parent = strdup(element_stack.back().c_str());

      if(node->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (node->extra & 0x1) == 0)
        element_stack.push_back(node->name);
      else if(node->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        element_stack.pop_back();

      if(strcmp((const char *)node->name, "unit") == 0) return nodes;;

      // save non-text node and get next node
      nodes.push_back(node);

    }

    not_done = xmlTextReaderRead(reader);

  }

  return nodes;

}
