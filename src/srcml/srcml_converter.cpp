#include <srcml_converter.hpp>

#include <string>
#include <cctype>

#include <libxml/xmlreader.h>
#include <iostream>

std::mutex srcml_converter::mutex;

std::map<std::string, std::shared_ptr<srcml_node>> srcml_converter::start_tags;
std::map<std::string, std::shared_ptr<srcml_node>> srcml_converter::end_tags;

std::shared_ptr<srcml_node> srcml_converter::get_current_node(xmlTextReaderPtr reader, const OPTION_TYPE & options) {

  xmlNode * curnode = xmlTextReaderCurrentNode(reader);

  std::string full_name;
  if(curnode->ns && (const char*)curnode->ns->prefix) {

    full_name = (const char*)curnode->ns->prefix;
    full_name += ":";

  }

  full_name += (const char*)curnode->name;

  std::shared_ptr<srcml_node> node;
  if (false && !xmlTextReaderIsEmptyElement(reader) && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT && curnode->properties == 0) {

    std::map<std::string, std::shared_ptr<srcml_node>>::iterator lb = start_tags.lower_bound(full_name);
    if (lb != start_tags.end() && !(start_tags.key_comp()(full_name, lb->first))) {

      node = lb->second;
    } else {

      node = std::make_shared<srcml_node>(*curnode, options & SRCML_OPTION_ARCHIVE);
      node->extra = 0;
      start_tags.insert(lb, std::map<std::string, std::shared_ptr<srcml_node>>::value_type(full_name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {

    std::map<std::string, std::shared_ptr<srcml_node>>::iterator lb = end_tags.lower_bound(full_name);
    if (lb != end_tags.end() && !(end_tags.key_comp()(full_name, lb->first))) {

      node = lb->second;
    } else {

      node = std::make_shared<srcml_node>(*curnode, options & SRCML_OPTION_ARCHIVE);
      node->extra = 0;
      end_tags.insert(lb, std::map<std::string, std::shared_ptr<srcml_node>>::value_type(full_name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    node = std::make_shared<srcml_node>(*curnode, options & SRCML_OPTION_ARCHIVE);
    node->free = true;
    node->extra = xmlTextReaderIsEmptyElement(reader);
  } else {
    node = std::make_shared<srcml_node>(*curnode, options & SRCML_OPTION_ARCHIVE);
    node->free = true;
  }

  node->type = (xmlElementType) xmlTextReaderNodeType(reader);

  return node;
}

std::shared_ptr<srcml_node> split_text(const char * characters_start, const char * characters_end) {

  std::shared_ptr<srcml_node> text = std::make_shared<srcml_node>();
  text->type = (xmlElementType)XML_READER_TYPE_TEXT;
  text->name = std::string("text");
  text->content = boost::optional<std::string>();

  if(characters_start != characters_end) {

    text->content = std::string();
    text->content->append((const char *)characters_start, characters_end  - characters_start);

  }

  text->is_empty = true;
  text->parent = boost::optional<std::shared_ptr<srcml_node>>();
  text->free = true;
  text->move = 0;

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

  if(output_buffer) srcml_memory_free(output_buffer);

}

// converts source code to srcML
void srcml_converter::convert(const std::string & language, void * context,
                              const std::function<int(void *, char *, size_t)> & read, const std::function<int(void *)> & close) {

  srcml_archive * unit_archive = srcml_archive_clone(archive);
  srcml_archive_disable_option(unit_archive, SRCML_OPTION_ARCHIVE | SRCML_OPTION_HASH);

  srcml_archive_write_open_memory(unit_archive, &output_buffer, &output_size);

  srcml_unit * unit = srcml_unit_create(unit_archive);

  srcml_unit_set_language(unit, language.c_str());

  srcml_unit_parse_io(unit, context, *read.target<int (*) (void *, char *, size_t)>(), *close.target<int (*) (void *)>());

  srcml_write_unit(unit_archive, unit);

  srcml_unit_free(unit);

  srcml_archive_close(unit_archive);
  srcml_archive_free(unit_archive);

}

srcml_nodes srcml_converter::create_nodes() const {
  
  xmlTextReaderPtr reader = xmlReaderForMemory(output_buffer, (int)output_size, 0, 0, XML_PARSE_HUGE);

  if (reader == NULL) throw std::string("Unable to open srcML output_buffer as XML");

  // read to unit
  xmlTextReaderRead(reader);

  // Read past unit tag open
  if(xmlTextReaderRead(reader) == 0) return srcml_nodes();

  // collect if non empty files
  srcml_nodes nodes = collect_nodes(reader);

  xmlFreeTextReader(reader);

  return nodes;

}

static bool is_separate_token(const char character) {


  return character == '(' || character ==')' || character == '[' || character == ']' || character == ',' || character == '"' || character == '\'' || character == '\\';

}

// collect the differences
srcml_nodes srcml_converter::collect_nodes(xmlTextReaderPtr reader) const {

  srcml_nodes nodes;

  srcml_nodes element_stack;
  element_stack.push_back(std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, "unit"));

  int not_done = 1;
  while(not_done) {

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      const char * characters = (const char *)xmlTextReaderConstValue(reader);

      // cycle through characters
      for (; (*characters) != 0;) {

        const char * characters_start = characters;

        std::shared_ptr<srcml_node> text;

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

          if(*characters == '\\') {

            ++characters;

            if(*characters == 'x') {

              ++characters;
              size_t pos = 0;
              while(pos < 2 && isxdigit(*characters))
                ++pos, ++characters;

              --characters;

            } else if(isdigit(*characters)) {

              ++characters;
              size_t pos = 0;
              while(pos < 2 && *characters >= '0' && *characters <= '7')
                ++pos, ++characters;

              --characters;

            } else if(*characters == 'u') {

              characters += 4;

            } else if(*characters =='U') {

              characters += 8;

            }

          }

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

    } else {

      // text node does not need to be copied.
      mutex.lock();
      std::shared_ptr<srcml_node> node = get_current_node(reader, srcml_archive_get_options(archive));
      mutex.unlock();

      if(node->type == (xmlElementType)XML_READER_TYPE_ELEMENT)
        node->parent = element_stack.back();

      if(node->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (node->extra & 0x1) == 0)
        element_stack.push_back(node);
      else if(node->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        element_stack.pop_back();

      if(node->name == "unit") return nodes;

      if(node->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (*node->parent)->is_simple)
        (*node->parent)->is_simple = false;
      
      nodes.push_back(node);

    }

    not_done = xmlTextReaderRead(reader);

  }

  return nodes;

}
