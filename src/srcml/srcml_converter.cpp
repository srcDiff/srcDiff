#include <srcml_converter.hpp>

#include <string>

#include <libxml/xmlreader.h>

std::mutex srcml_converter::mutex;

std::map<std::string, srcml_node *> srcml_converter::start_tags;
std::map<std::string, srcml_node *> srcml_converter::end_tags;

srcml_node * srcml_converter::get_current_node(xmlTextReaderPtr reader, const OPTION_TYPE & options, int context) {

  xmlNode * curnode = xmlTextReaderCurrentNode(reader);

  std::string full_name;
  if(curnode->ns && (const char*)curnode->ns->prefix) {

    full_name = (const char*)curnode->ns->prefix;
    full_name += ":";

  }

  full_name += (const char*)curnode->name;

  srcml_node * node = 0;
  if (!xmlTextReaderIsEmptyElement(reader) && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT && curnode->properties == 0) {

    std::map<std::string, srcml_node *>::iterator lb = start_tags.lower_bound(full_name);
    if (lb != start_tags.end() && !(start_tags.key_comp()(full_name, lb->first))) {

      node = lb->second;
    } else {

      node = new srcml_node(*curnode, options & SRCML_OPTION_ARCHIVE);
      node->extra = 0;
      start_tags.insert(lb, std::map<std::string, srcml_node *>::value_type(full_name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {

    std::map<std::string, srcml_node *>::iterator lb = end_tags.lower_bound(full_name);
    if (lb != end_tags.end() && !(end_tags.key_comp()(full_name, lb->first))) {

      node = lb->second;
    } else {

      node = new srcml_node(*curnode, options & SRCML_OPTION_ARCHIVE);
      node->extra = 0;
      end_tags.insert(lb, std::map<std::string, srcml_node *>::value_type(full_name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    node = new srcml_node(*curnode, options & SRCML_OPTION_ARCHIVE);
    node->free = true;
    node->extra = xmlTextReaderIsEmptyElement(reader);
  } else {
    node = new srcml_node(*curnode, options & SRCML_OPTION_ARCHIVE);
    node->free = true;
  }

  node->type = (xmlElementType) xmlTextReaderNodeType(reader);

  return node;
}

srcml_node * split_text(const char * characters_start, const char * characters_end) {

  srcml_node * text = new srcml_node;
  text->type = (xmlElementType)XML_READER_TYPE_TEXT;
  text->name = std::string("text");
  text->content = boost::optional<std::string>();

  if(characters_start != characters_end) {

    const char * content = strndup((const char *)characters_start, characters_end  - characters_start);
    text->content = content;
  }
  text->ns = 0;
  text->properties = 0;
  text->is_empty = true;
  text->parent = boost::optional<std::string>();
  text->free = true;
  text->move = 0;
  text->nest = 0;

  return text;
}

void eat_element(xmlTextReaderPtr& reader) {
  int depth = xmlTextReaderDepth(reader);
  xmlTextReaderRead(reader);
  while (xmlTextReaderDepth(reader) > depth)
    xmlTextReaderRead(reader);
  xmlTextReaderRead(reader);
}

srcml_converter::srcml_converter(srcml_archive * archive, int stream_source) 
  : archive(archive), stream_source(stream_source), output_buffer(0) {}

srcml_converter::~srcml_converter() {

  if(output_buffer) free(output_buffer);

}

// converts source code to srcML
void srcml_converter::convert(const std::string & language, void * context,
                              const std::function<int(void *, char *, int)> & read, const std::function<int(void *)> & close,
                              const OPTION_TYPE & options) {

  srcml_archive * unit_archive = srcml_clone_archive(archive);
  srcml_archive_disable_option(unit_archive, SRCML_OPTION_ARCHIVE | SRCML_OPTION_HASH);

  srcml_write_open_memory(unit_archive, &output_buffer, &output_size);

  srcml_unit * unit = srcml_create_unit(unit_archive);

  srcml_unit_set_language(unit, language.c_str());

  srcml_parse_unit_io(unit, context, *read.target<int (*) (void *, char *, int)>(), *close.target<int (*) (void *)>());

  srcml_write_unit(unit_archive, unit);

  srcml_free_unit(unit);

  srcml_close_archive(unit_archive);
  srcml_free_archive(unit_archive);

}

std::vector<srcml_node *> srcml_converter::create_nodes() const {
  
  xmlTextReaderPtr reader = xmlReaderForMemory(output_buffer, output_size, 0, 0, XML_PARSE_HUGE);

  if (reader == NULL) throw std::string("Unable to open srcML output_buffer as XML");

  // read to unit
  xmlTextReaderRead(reader);

  // Read past unit tag open
  if(xmlTextReaderRead(reader) == 0) throw std::string("Error reading srcML.");

  // collect if non empty files
  std::vector<srcml_node *> nodes = collect_nodes(reader);

  xmlFreeTextReader(reader);

  return nodes;

}

static bool is_separate_token(const char character) {


  return character == '(' || character ==')' || character == '[' || character == ']' || character == ',';

}

// check if node is a indivisable group of three (atomic)
static bool is_atomic_srcml(std::vector<srcml_node *> & nodes, unsigned start) {

  static const char * atomic[] = { "name", "operator", "literal", "modifier", 0 };

  if((start + 2) >= nodes.size())
    return false;

  if((xmlReaderTypes)nodes.at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if((xmlReaderTypes)nodes.at(start + 2)->type != XML_READER_TYPE_END_ELEMENT)
    return false;

  if(!nodes.at(start)->name || !nodes.at(start + 2)->name)
    return false;

  if(*nodes.at(start)->name != *nodes.at(start + 2)->name)
    return false;

  for(int i = 0; atomic[i]; ++i)
    if(*nodes.at(start)->name == atomic[i])
      return true;

  return false;
}


// collect the differences
std::vector<srcml_node *> srcml_converter::collect_nodes(xmlTextReaderPtr reader) const {

  std::vector<srcml_node *> nodes;

  std::vector<boost::optional<std::string>> element_stack;
  element_stack.push_back(std::string("unit"));

  int not_done = 1;
  while(not_done) {

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      const char * characters = (const char *)xmlTextReaderConstValue(reader);

      // cycle through characters
      for (; (*characters) != 0;) {

        const char * characters_start = characters;

        srcml_node * text;

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
      srcml_node * node = get_current_node(reader, srcml_archive_get_options(archive), stream_source);
      mutex.unlock();

      if(node->type == (xmlElementType)XML_READER_TYPE_ELEMENT)
        node->parent = element_stack.back();

      if(node->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (node->extra & 0x1) == 0)
        element_stack.push_back(node->name);
      else if(node->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        element_stack.pop_back();

      if(*node->name == "unit") return nodes;;

      // save non-text node and get next node
      nodes.push_back(node);

    }

    not_done = xmlTextReaderRead(reader);

  }

  return nodes;

}
