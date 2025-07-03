// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcml_converter.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcml_converter.hpp>

#include <constants.hpp>
#include <shortest_edit_script.h>

#include <string>
#include <cctype>

#include <libxml/xmlreader.h>
#include <iostream>

std::mutex srcml_converter::mutex;

std::map<std::string, std::shared_ptr<srcML::node>> srcml_converter::start_tags;
std::map<std::string, std::shared_ptr<srcML::node>> srcml_converter::end_tags;

std::shared_ptr<srcML::node> srcml_converter::get_current_node(xmlTextReaderPtr reader, bool is_archive [[maybe_unused]]) {

  xmlNode * curnode = xmlTextReaderCurrentNode(reader);
  curnode->type = (xmlElementType)xmlTextReaderNodeType(reader);

  std::string full_name;
  if(curnode->ns && (const char*)curnode->ns->prefix) {

    full_name = (const char*)curnode->ns->prefix;
    full_name += ":";

  }

  full_name += (const char*)curnode->name;

  std::shared_ptr<srcML::node> node;
  if (false && !xmlTextReaderIsEmptyElement(reader) && xmlTextReaderNodeType(reader) == srcML::node_type::START && curnode->properties == 0) {

    std::map<std::string, std::shared_ptr<srcML::node>>::iterator lb = start_tags.lower_bound(full_name);
    if (lb != start_tags.end() && !(start_tags.key_comp()(full_name, lb->first))) {

      node = lb->second;
    } else {

      node = std::make_shared<srcML::node>(*curnode, XML_ELEMENT_NODE);
      node->set_empty(false);
      start_tags.insert(lb, std::map<std::string, std::shared_ptr<srcML::node>>::value_type(full_name, node));
    }

  } else if (false && xmlTextReaderNodeType(reader) == srcML::node_type::END) {

    std::map<std::string, std::shared_ptr<srcML::node>>::iterator lb = end_tags.lower_bound(full_name);
    if (lb != end_tags.end() && !(end_tags.key_comp()(full_name, lb->first))) {

      node = lb->second;
    } else {

      node = std::make_shared<srcML::node>(*curnode, XML_ELEMENT_NODE);
      node->set_empty(false);
      end_tags.insert(lb, std::map<std::string, std::shared_ptr<srcML::node>>::value_type(full_name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == srcML::node_type::START) {
    node = std::make_shared<srcML::node>(*curnode, curnode->type);
    node->set_empty(xmlTextReaderIsEmptyElement(reader));
  } else {
    node = std::make_shared<srcML::node>(*curnode, curnode->type);
  }

  return node;
}

std::shared_ptr<srcML::node> split_text(const char * characters_start,
                                       const char * characters_end,
                                       const std::shared_ptr<srcML::node> & parent) {

  std::shared_ptr<srcML::node> text = std::make_shared<srcML::node>(srcML::node_type::TEXT, "text");

  if(characters_start != characters_end) {

    text->set_content(std::string().append((const char *)characters_start, characters_end  - characters_start));

  }

  text->set_empty(true);
  text->set_parent(parent);
  text->set_move(0);
  
  return text;
}

void eat_element(xmlTextReaderPtr& reader) {
  int depth = xmlTextReaderDepth(reader);
  xmlTextReaderRead(reader);
  while (xmlTextReaderDepth(reader) > depth) {
    xmlTextReaderRead(reader);
  }
  xmlTextReaderRead(reader);
}

srcml_converter::srcml_converter(srcml_archive * archive, bool split_strings, int stream_source) 
  : archive(archive), split_strings(split_strings), stream_source(stream_source), output_buffer(0) {}

srcml_converter::~srcml_converter() {

  // libsrcml uses xmlBufferCreate to create this buffer, so we need to use
  // xmlFree to free it. srcml_memory_free just calls the normal free(), which
  // might not match how libxml2 made the allocation
  if(output_buffer) xmlFree(output_buffer);

}

// converts source code to srcML
void srcml_converter::convert(const std::string & language, void * context,
                              const std::function<ssize_t(void *, void *, size_t)> & read, const std::function<int(void *)> & close) {

  srcml_archive * unit_archive = srcml_archive_clone(archive);
  srcml_archive_enable_solitary_unit(unit_archive);
  srcml_archive_disable_hash(unit_archive);

  srcml_archive_write_open_memory(unit_archive, &output_buffer, &output_size);

  srcml_unit * unit = srcml_unit_create(unit_archive);

  srcml_unit_set_language(unit, language.c_str());

  srcml_unit_parse_io(unit, context, *read.target<ssize_t (*) (void *, void *, size_t)>(), *close.target<int (*) (void *)>());

  srcml_archive_write_unit(unit_archive, unit);

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

  return character == '(' || character ==')'  || character == '['  || character == ']'
      || character == ',' || character == '"' || character == '\'' || character == '\\'
      || character == ';' || character == '{' || character == '}'  || character == ':';

}

static bool is_comment_separate(const char character) {

  return character == '/' || character == '*' || character == '!';

}

static bool is_cpp_file_separate(const char character) {

  return character == '<' || character == '>';

}

// collect the differences
srcml_nodes srcml_converter::collect_nodes(xmlTextReaderPtr reader) const {

  srcml_nodes nodes;

  srcml_nodes element_stack;
  element_stack.push_back(std::make_shared<srcML::node>(srcML::node_type::START, "unit"));


  bool is_elseif = false;
  int not_done = 1;
  while(not_done) {

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      const char * characters = (const char *)xmlTextReaderConstValue(reader);

      bool is_string_literal 
        = element_stack.back()->get_name() == "literal"
         && !element_stack.back()->get_attributes().empty()
         && *element_stack.back()->get_attribute_value("type") == "string";

      // cycle through characters
      while((*characters) != 0) {

        const char * characters_start = characters;

        std::shared_ptr<srcML::node> text;

        // separate new line
        if(is_string_literal && !split_strings) {
          while((*characters) != 0) ++characters;

          text = split_text(characters_start, characters, element_stack.back());
        } else if(*characters == '\n') {

          ++characters;
          text = split_text(characters_start, characters, element_stack.back());
        }

        // separate non-new line whitespace
        else if(isspace(*characters)) {

          //while((*characters) != 0 && *characters != '\n' && isspace(*characters))
            ++characters;

            text = split_text(characters_start, characters, element_stack.back());

        } else if(element_stack.back()->get_name() == "comment"
                  && is_comment_separate(*characters)) {

          while((*characters) != 0 && is_comment_separate(*characters)) {
            ++characters;
            }

          text = split_text(characters_start, characters, element_stack.back());

        } else if(element_stack.back()->get_name() == "file"
                  && is_cpp_file_separate(*characters)) {

          while((*characters) != 0 && is_cpp_file_separate(*characters)) {
            ++characters;
            }

          text = split_text(characters_start, characters, element_stack.back());

        }

        // separate non whitespace
        else if(is_separate_token(*characters)) {

          bool first = true;
          bool last_was_collect = (*characters == '"' || *characters == '\\');

          // merge consecutive escape \ and "
          while(*characters && (first
                 || (is_string_literal && last_was_collect
                  && (*characters == '"'
                    || *characters == '\\')))) {

            first = false;
            last_was_collect = (*characters == '"' || *characters == '\\');

            if(*characters == '\\') {

              ++characters;
              if(!*characters) continue;

              if(*characters == 'x') {

                ++characters;
                size_t pos = 0;
                while(pos < 2 && isxdigit(*characters))
                  ++pos, ++characters;

                --characters;

              } else if(isdigit(*characters)) {

                ++characters;
                size_t pos = 0;
                while(pos < 2 && *characters >= '0' && *characters <= '7') {
                  ++pos, ++characters;
                  }

                --characters;

              } else if(*characters == 'u') {

                ++characters;
                size_t pos = 0;
                while(pos < 4 && isxdigit(*characters)) {
                  ++pos, ++characters;
                    }

                --characters;

              } else if(*characters =='U') {

                ++characters;
                size_t pos = 0;
                while(pos < 8 && isxdigit(*characters)) {
                  ++pos, ++characters;
                    }

                --characters;

              }

            }

            ++characters;

          }

          text = split_text(characters_start, characters, element_stack.back());

        } else {

          bool in_comment = element_stack.back()->get_name() == "comment";
          bool in_cpp_file = element_stack.back()->get_name() == "file";

          while((*characters) != 0 
                && !isspace(*characters)
                && !(in_comment && is_comment_separate(*characters))
                && !(in_cpp_file && is_cpp_file_separate(*characters))
                && !is_separate_token(*characters)) {
            ++characters;
      }

          // Copy the remainder after (
          text = split_text(characters_start, characters, element_stack.back());

        }

        // temp if for elseif, insert start tag
        if(is_elseif && *text->get_content() == "if") {
          is_elseif = false;
          std::shared_ptr<srcML::node> if_node = std::make_shared<srcML::node>(*element_stack.back());
          if_node->clear_attributes();
          if_node->set_temporary(true);
          nodes.push_back(if_node);
        }
        nodes.push_back(text);

      }

    } else {

      // text node does not need to be copied.
      mutex.lock();
      std::shared_ptr<srcML::node> node = get_current_node(reader, srcml_archive_get_options(archive));
      mutex.unlock();

      
      if(node->get_type() == srcML::node_type::START) {
        node->set_parent(element_stack.back());
      }
      


      // insert end if temp element for elseif and detect elseif
      if(node->get_type() == srcML::node_type::END
        && element_stack.back()->get_name() == "if" && !element_stack.back()->get_attributes().empty()
        && bool(element_stack.back()->get_attribute("type"))) {
        std::shared_ptr<srcML::node> end_node = std::make_shared<srcML::node>(*node);
        end_node->set_temporary(true);
        nodes.push_back(end_node);
      } else if(node->get_name() == "if" && !node->get_attributes().empty()
            && bool(element_stack.back()->get_attribute("type"))) {
          is_elseif = true;
      }

      if(node->get_type() == srcML::node_type::START && !node->is_empty()) {
        element_stack.push_back(node);
      }
      else if(node->get_type() == srcML::node_type::END) {
        element_stack.pop_back();
      }

      if(node->get_name() == "unit") return nodes;

      
      if(node->get_type() == srcML::node_type::START && node->get_parent()->is_simple()) {
        node->get_parent()->set_simple(false);
      }
      
      
      if(node->is_empty()) {
        node->set_empty(false);
        std::shared_ptr<srcML::node> end_node = std::make_shared<srcML::node>(*node);
        end_node->set_type(srcML::node_type::END);
        nodes.push_back(node);
        nodes.push_back(end_node);
      } else {
        nodes.push_back(node);
      }

    }

    not_done = xmlTextReaderRead(reader);

  }

  return nodes;

}
