/*
  srcml_node.cpp

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

#include <srcml_node.hpp>

#include <srcml.h>

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

std::shared_ptr<srcml_node::srcml_namespace> srcml_node::SRC_NAMESPACE 
  = std::make_shared<srcml_node::srcml_namespace>("http://www.srcML.org/srcML/src");

std::shared_ptr<srcml_node::srcml_namespace> srcml_node::CPP_NAMESPACE
  = std::make_shared<srcml_node::srcml_namespace>("http://www.srcML.org/srcML/cpp", "cpp");

std::shared_ptr<srcml_node::srcml_namespace> srcml_node::DIFF_NAMESPACE 
  = std::make_shared<srcml_node::srcml_namespace>("http://www.srcML.org/srcDiff", "diff");

std::unordered_map<std::string, std::shared_ptr<srcml_node::srcml_namespace>> srcml_node::namespaces = {};

srcml_node::srcml_namespace::srcml_namespace(const std::string & uri, const std::optional<std::string> & prefix) : uri(uri), prefix(prefix) {}

srcml_node::srcml_namespace::srcml_namespace(xmlNsPtr ns) 
  : uri(), prefix() {

    if(!ns) return;

    if(ns->href)   uri = std::string((const char *)ns->href);
    if(ns->prefix) prefix = std::string((const char *)ns->prefix);
}

srcml_node::srcml_namespace::srcml_namespace(const srcml_namespace & ns) 
  : uri(ns.uri), prefix(ns.prefix) {}

std::string srcml_node::srcml_namespace::get_uri() const {
  return uri;
}

std::optional<std::string> srcml_node::srcml_namespace::get_prefix() const {
  return prefix;
}

srcml_node::srcml_attribute::srcml_attribute(xmlAttrPtr attribute)
  : name((const char *)attribute->name),
    value(attribute->children && attribute->children->content ? 
          std::string((const char *)attribute->children->content) : std::optional<std::string>()),
    ns(get_namespace(attribute->ns)) {}

srcml_node::srcml_attribute::srcml_attribute(
    const std::string & name,
    std::shared_ptr<srcml_namespace> ns,
    std::optional<std::string> value) : name(name), ns(ns), value(value) {}

std::string srcml_node::srcml_attribute::full_name() const {
  if(ns && ns->prefix) return *ns->prefix + ":" + name;
  return name;
}

bool srcml_node::srcml_attribute::operator==(const srcml_attribute & that) const {
  return ns == that.ns && name == that.name && value == that.value;
}

bool srcml_node::srcml_attribute::operator!=(const srcml_attribute & that) const {
  return !this->operator==(that);
}

void srcml_node::srcml_namespace::set_uri(std::string input) {
  uri = input;
}

void srcml_node::srcml_namespace::set_prefix(std::optional<std::string> input) {
  prefix = input;
}

srcml_node::srcml_node_type xml_type2srcml_type(xmlElementType type) {
  static std::unordered_map<unsigned int, srcml_node::srcml_node_type> type_map = {

    { XML_READER_TYPE_ELEMENT, srcml_node::srcml_node_type::START },
    { XML_READER_TYPE_END_ELEMENT, srcml_node::srcml_node_type::END },
    { XML_READER_TYPE_TEXT, srcml_node::srcml_node_type::TEXT },
    { XML_READER_TYPE_SIGNIFICANT_WHITESPACE, srcml_node::srcml_node_type::WS },

  };

  try {
    return type_map.at((unsigned int)type);
  } catch(const std::out_of_range & error) {
    return srcml_node::srcml_node_type::OTHER;
  }

}

std::shared_ptr<srcml_node::srcml_namespace> srcml_node::get_namespace(xmlNsPtr ns) {

  static bool init_namespace = true;

  if(init_namespace) {
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcML/src", SRC_NAMESPACE));
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcML/cpp", CPP_NAMESPACE));
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcDiff",   DIFF_NAMESPACE));
      init_namespace = false;
  }

  if(!ns) return SRC_NAMESPACE;

  typedef std::unordered_map<std::string, std::shared_ptr<srcml_namespace>>::const_iterator namespaces_citr;
  namespaces_citr citr = namespaces.find((const char *)ns->href);
  if(citr != namespaces.end()) return citr->second;

  namespaces_citr added_citr = namespaces.emplace(std::make_pair((const char *)ns->href, std::make_shared<srcml_namespace>(ns))).first;
  return added_citr->second;
}


srcml_node::srcml_node()
  : type(srcml_node_type::OTHER), name(), ns(SRC_NAMESPACE), content(),
    ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(true), move(0), user_data(), extra(0) {}

srcml_node::srcml_node(const xmlNode & node, xmlElementType xml_type) 
  : type(xml_type2srcml_type(xml_type)), name(), ns(), content(),
    ns_definition(), parent(), attributes(), temporary(false), empty(node.extra), simple(true), move(0), user_data(), extra(node.extra) {

  name = std::string((const char *)node.name);

  if(node.content)
    content = std::string((const char *)node.content);

  ns = get_namespace(node.ns);

  xmlNsPtr node_ns = node.nsDef;
  while(node_ns) {
    ns_definition.emplace_back(get_namespace(node_ns));
    node_ns = node_ns->next;
  }

  xmlAttrPtr attribute = node.properties;
  while (attribute) {
    srcml_attribute new_attribute = srcml_attribute(attribute);
    attributes.emplace(std::make_pair(new_attribute.full_name(), new_attribute));
    attribute = attribute->next;
  }

}

srcml_node::srcml_node(const std::string & text) : type(srcml_node_type::TEXT), name("text"), 
  ns(SRC_NAMESPACE), content(text), ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(true), move(0), user_data(), extra(0) {}

srcml_node::srcml_node(const srcml_node & node) : type(node.type), name(node.name), ns(node.ns),
  content(node.content), ns_definition(node.ns_definition), parent(node.parent), attributes(node.attributes), temporary(node.temporary), empty(node.empty), simple(node.simple), 
  move(node.move), user_data(node.user_data) {}

srcml_node::srcml_node(srcml_node::srcml_node_type type, const std::string & name, const std::shared_ptr<srcml_namespace> & ns) 
  : type(type), name(name), ns(ns), content(), ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(false), move(0), user_data(), 
  extra(0) {}

srcml_node::~srcml_node() {}

std::string srcml_node::full_name() const {

  if(ns->prefix) return *ns->prefix + ":" + name;

  return name;
} 

const srcml_node::srcml_attribute * srcml_node::get_attribute(const std::string & attribute) const {

  srcml_node::srcml_attribute_map_citr attribute_itr = attributes.find(attribute);
  if(attribute_itr == attributes.end()) return nullptr;
  return &attribute_itr->second;

}

srcml_node::srcml_attribute * srcml_node::get_attribute(const std::string & attribute) {

  srcml_node::srcml_attribute_map_itr attribute_itr = attributes.find(attribute);
  if(attribute_itr == attributes.end()) return nullptr;
  return &attribute_itr->second;

}

const std::string * srcml_node::get_attribute_value(const std::string & attribute) const {

  srcml_node::srcml_attribute_map_citr attribute_itr = attributes.find(attribute);
  if(attribute_itr == attributes.end() || !attribute_itr->second.value) return nullptr;
  return &*attribute_itr->second.value;

}

std::string * srcml_node::get_attribute_value(const std::string & attribute) {

  srcml_node::srcml_attribute_map_itr attribute_itr = attributes.find(attribute);
  if(attribute_itr == attributes.end() || !attribute_itr->second.value) return nullptr;
  return &*attribute_itr->second.value;

}

bool srcml_node::operator==(const srcml_node & node) const {
  return type == node.type && name == node.name && content == node.content;
}

bool srcml_node::operator!=(const srcml_node & node) const {
  return !operator==(node);
}

bool srcml_node::is_start() const {
  return type == srcml_node_type::START;
}

bool srcml_node::is_end() const {
  return type == srcml_node_type::END;
}

bool srcml_node::is_empty() const {
  return empty;
}


bool srcml_node::is_text() const {
  return type == srcml_node_type::TEXT;
}

bool srcml_node::is_whitespace() const {
  return is_text() && std::isspace((*content)[0]);
}

bool srcml_node::is_temporary() const {
  return temporary;
}

bool srcml_node::is_simple() const {
  return simple;
}

bool srcml_node::is_new_line() const {
  return is_text() && (*content)[0] == '\n';
}

bool srcml_node::is_open_tag() const {
  return type == srcml_node_type::START;
}

void srcml_node::clear_attributes() {
  attributes.clear();
}

void srcml_node::set_attributes(const srcml_attribute_map & input) {
  for (srcml_node::srcml_attribute_map_citr it = input.begin(); it != input.end(); ++it) {
    attributes.emplace(it->first, it->second);
  }
}

void srcml_node::set_type(srcml_node_type input) {
  type = input;
}

void srcml_node::set_empty(bool input) {
  empty = input;
}

void srcml_node::set_temporary(bool input) {
  temporary = input;
}

void srcml_node::set_parent(std::shared_ptr<srcml_node> input) {
  parent = input;
}

void srcml_node::set_simple(bool input) {
  simple = input;
}

void srcml_node::set_content(std::optional<std::string> input) {
  content = input;
}

void srcml_node::set_name(std::string input) {
  name = input;
}

void srcml_node::set_move(int input) {
  move = input;
}

std::string srcml_node::srcml_attribute::get_name() const {
  return name;
}

std::optional<std::string> srcml_node::srcml_attribute::get_value() const {
  return value;
}

const srcml_node::srcml_attribute_map & srcml_node::get_attributes() const {
  return attributes;
} 

srcml_node::srcml_node_type srcml_node::get_type() const {
  return type;
}

std::string srcml_node::get_name() const {
  return name;
}

std::optional<std::string> srcml_node::get_content() const {
  return content;
}

int srcml_node::get_move() const {
  return move;
}

std::ostream & operator<<(std::ostream & out, const srcml_node & node) {
  if(node.is_text()) {
    out << "text: '" << (node.content ? *node.content : "") << '\'';
  } else {
      out << '<';
    if(node.is_end()) {
      out << '/';
    }
    out << node.name;
  }

  for(const srcml_node::srcml_attribute_map_pair & attribute_pair : node.attributes) {
    out << ' ' << attribute_pair.first << '=' << *attribute_pair.second.value;
  }

  if(!node.is_text()) {
    out << '>';
  }

  return out;
}