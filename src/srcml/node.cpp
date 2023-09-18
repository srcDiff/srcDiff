/*
  node.cpp

  Copyright (C) 2018 srcML, LLC. (www.srcML.org)

  This file is part of a translator from source code to srcReader

  srcReader is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  srcReader is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcML translator; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include <node.hpp>

#include <srcml.h>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

#ifdef __MINGW32__
#include <mingw32.hpp>
#endif

srcML::node::node_type xml_type2srcml_type(xmlElementType type) {
  static std::unordered_map<unsigned int, srcML::node::node_type> type_map = {

    { XML_READER_TYPE_ELEMENT, srcML::node::node_type::START },
    { XML_READER_TYPE_END_ELEMENT, srcML::node::node_type::END },
    { XML_READER_TYPE_TEXT, srcML::node::node_type::TEXT },
    { XML_READER_TYPE_SIGNIFICANT_WHITESPACE, srcML::node::node_type::WS },

  };

  try {
    return type_map.at((unsigned int)type);
  } catch(const std::out_of_range & error) {
    return srcML::node::node_type::OTHER;
  }

}

srcML::node::node()
  : type(srcML::node::node_type::OTHER), name(), ns(SRC_NAMESPACE), content(),
    ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(true), move(0), user_data(), extra(0) {}

srcML::node::node(const xmlNode & node, xmlElementType xml_type) 
  : type(xml_type2srcml_type(xml_type)), name(), ns(), content(),
    ns_definition(), parent(), attributes(), temporary(false), empty(node.extra), simple(true), move(0), user_data(), extra(node.extra) {

  name = std::string((const char *)node.name);

  if(node.content)
    content = std::string((const char *)node.content);

  ns = srcML::name_space::get_namespace(node.ns);

  if(type != srcML::node::node_type::START) return;

  xmlNsPtr node_ns = node.nsDef;
  while(node_ns) {
    ns_definition.emplace_back(srcML::name_space::get_namespace(node_ns));
    node_ns = node_ns->next;
  }

  xmlAttrPtr attribute = node.properties;
  while (attribute) {
    srcml_attribute new_attribute = srcml_attribute(attribute);
    attributes.emplace(std::make_pair(new_attribute.full_name(), new_attribute));
    attribute = attribute->next;
  }

}

srcML::node::node(const std::string & text) : type(srcML::node::node_type::TEXT), name("text"), 
  ns(SRC_NAMESPACE), content(text), ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(true), move(0), user_data(), extra(0) {}

srcML::node::node(const srcML::node & node) : type(node.type), name(node.name), ns(node.ns),
  content(node.content), ns_definition(node.ns_definition), parent(node.parent), attributes(node.attributes), temporary(node.temporary), empty(node.empty), simple(node.simple), 
  move(node.move), user_data(node.user_data) {}

srcML::node::node(node::node_type type, const std::string & name, const std::shared_ptr<srcML::name_space> & ns) 
  : type(type), name(name), ns(ns), content(), ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(false), move(0), user_data(), 
  extra(0) {}

  srcML::node::~node() {}

std::string srcML::node::full_name() const {

  if(ns->get_prefix()) return *ns->get_prefix() + ":" + name;

  return name;
} 

const srcml_attribute * srcML::node::get_attribute(const std::string & attribute) const {

  srcML::node::srcml_attribute_map_citr attribute_itr = attributes.find(attribute);
  if(attribute_itr == attributes.end()) return nullptr;
  return &attribute_itr->second;

}

srcml_attribute * srcML::node::get_attribute(const std::string & attribute) {

  srcML::node::srcml_attribute_map_itr attribute_itr = attributes.find(attribute);
  if(attribute_itr == attributes.end()) return nullptr;
  return &attribute_itr->second;

}

const std::optional<std::string> & srcML::node::get_attribute_value(const std::string & attribute) const {

  srcML::node::srcml_attribute_map_citr attribute_itr = attributes.find(attribute);
  assert(attribute_itr != attributes.end());

  return attribute_itr->second.get_value();

}

bool srcML::node::operator==(const srcML::node & node) const {
  return type == node.type && name == node.name && content == node.content && attributes == node.attributes;
}

bool srcML::node::operator!=(const srcML::node & node) const {
  return !operator==(node);
}

bool srcML::node::is_start() const {
  return type == srcML::node::node_type::START;
}

bool srcML::node::is_end() const {
  return type == srcML::node::node_type::END;
}

bool srcML::node::is_empty() const {
  return empty;
}


bool srcML::node::is_text() const {
  return type == srcML::node::node_type::TEXT;
}

bool srcML::node::is_whitespace() const {
  return is_text() && std::isspace((*content)[0]);
}

bool srcML::node::is_temporary() const {
  return temporary;
}

bool srcML::node::is_simple() const {
  return simple;
}

bool srcML::node::is_new_line() const {
  return is_text() && (*content)[0] == '\n';
}

bool srcML::node::is_open_tag() const {
  return type == srcML::node::node_type::START;
}

void srcML::node::clear_attributes() {
  attributes.clear();
}

void srcML::node::set_attributes(const srcml_attribute_map & input) {
  attributes = input;
}

void srcML::node::emplace_attribute(const std::string & name, const srcml_attribute & attr) {
  attributes.emplace(name, attr);
}

void srcML::node::set_type(srcML::node::node_type input) {
  type = input;
}

void srcML::node::set_empty(bool input) {
  empty = input;
}

void srcML::node::set_temporary(bool input) {
  temporary = input;
}

void srcML::node::set_parent(std::shared_ptr<srcML::node> input) {
  parent = input;
}

void srcML::node::set_simple(bool input) {
  simple = input;
}

void srcML::node::set_content(std::optional<std::string> input) {
  content = input;
}

void srcML::node::set_name(std::string input) {
  name = input;
}

void srcML::node::set_move(int input) {
  move = input;
}

const srcML::node::srcml_attribute_map & srcML::node::get_attributes() const {
  return attributes;
}

srcML::node::node_type srcML::node::get_type() const {
  return type;
}

const std::string & srcML::node::get_name() const {
  return name;
}

const std::optional<std::string> & srcML::node::get_content() const {
  return content;
}

int srcML::node::get_move() const {
  return move;
}

std::optional<std::shared_ptr<srcML::node>> srcML::node::get_parent() const {
  return parent;
}

std::shared_ptr<srcML::name_space> srcML::node::get_namespace() const {
  return ns;
}

std::ostream & srcML::operator<<(std::ostream & out, const srcML::node & node) {
  if(node.is_text()) {
    out << "text: '" << (node.get_content() ? *node.get_content() : "") << '\'';
  } else {
      out << '<';
    if(node.is_end()) {
      out << '/';
    }
    out << node.get_name();
  }

  for(const srcML::node::srcml_attribute_map_pair & attribute_pair : node.get_attributes()) {
    out << ' ' << attribute_pair.first << '=' << *attribute_pair.second.get_value();
  }

  if(!node.is_text()) {
    out << '>';
  }

  return out;
}