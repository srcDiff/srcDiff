/*
  srcml_node.cpp

  Copyright (C) 2006  SDML (www.sdml.info)

  This file is part of a translator from source code to srcDiff

  The extractor is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcML translator is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcML translator; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Useful xml Reader Writer functions
*/

#include <srcml_node.hpp>

#include <iostream>
#include <cstdlib>

#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include <shortest_edit_script.h>
#include <srcml.h>

#ifdef __MINGW32__
#include <mingw32.hpp>
#endif

srcml_node::srcml_ns::srcml_ns(const srcml_ns & ns) : href(ns.href), prefix(ns.prefix) {}

bool srcml_node::srcml_attr::operator==(const srcml_attr & attr) const {

  if(name != attr.name) return false;

  if(value == attr.value && (!value || *value == *attr.value)) return true;

  return false;


}

bool srcml_node::srcml_attr::operator!=(const srcml_attr & attr) const {

  return !this->operator==(attr);


}

srcml_node::srcml_node(const xmlNode & node, bool is_archive) : type(node.type), extra(node.extra), is_empty(node.extra), free(false), move(0) {

  name = std::string((const char *)node.name);

  if(content)
    content = std::string((const char *)node.content);

  if(node.ns) {

    ns = srcml_ns();

    if(node.ns->href)
      ns->href = std::string((const char *)node.ns->href);

    if(node.ns->prefix)
      ns->prefix = std::string((const char *)node.ns->prefix);
  }

  xmlNsPtr node_ns = node.nsDef;
  if(name == "unit" && ns) {

    while(is_archive && node_ns && (const char *)node_ns->href != std::string("http://www.sdml.info/srcML/cpp"))
        node_ns = node_ns->next;

    std::string ns_name = "xmlns";
    if(node_ns->prefix) {

      ns_name += ":";
      ns_name += (const char *)node_ns->prefix;

    }

    properties.emplace_back(ns_name, std::string((const char *)node_ns->href));

    node_ns = node_ns->next;
    while(!is_archive && node_ns) {

      std::string ns_name = "xmlns";
      if(node_ns->prefix) {

        ns_name += ":";
        ns_name += (const char *)node_ns->prefix;

      }

      properties.emplace_back(ns_name, std::string((const char *)node_ns->href));

      node_ns = node_ns->next;

    }

  }

  xmlAttrPtr attribute = node.properties;
  if(attribute) {

    properties.emplace_back(std::string((const char *)attribute->name), std::string((const char *)attribute->children->content));

    attribute = attribute->next;
    while (attribute) {

      properties.emplace_back(std::string((const char *)attribute->name), std::string((const char *)attribute->children->content));

      attribute = attribute->next;

    }

  }

}

srcml_node::srcml_node(xmlElementType type, const std::string & name, const boost::optional<srcml_ns> & ns, const boost::optional<std::string> & content,
const std::list<srcml_attr> & properties, unsigned short extra, const boost::optional<std::shared_ptr<srcml_node>> & parent, bool is_empty)
  : type(type), name(name), ns(ns), content(content), properties(properties), extra(extra), parent(parent), is_empty(is_empty), free(false), move(0) {}

srcml_node::srcml_node(xmlElementType type, const std::string & name, const srcml_ns & ns) : type(type), name(name), ns(ns),
 properties(0), extra(0), is_empty(false), free(false), move(0) {}

srcml_node::srcml_node(const srcml_node & node) : type(node.type), name(node.name), content(node.content), extra(node.extra),
  parent(node.parent), is_empty(node.is_empty), free(true), move(0) {

  if(node.ns)
    ns = srcml_ns(node.ns->href, node.ns->prefix);

  for(const srcml_attr & attr : node.properties)
    properties.push_back(attr);

}

srcml_node::~srcml_node() {}

bool srcml_node::operator==(const srcml_node & node) const {

  return type == node.type
    && name == node.name
    && (((xmlReaderTypes)type != XML_READER_TYPE_TEXT && (xmlReaderTypes)type != XML_READER_TYPE_SIGNIFICANT_WHITESPACE)
      || (content == node.content && (!content || *content == *node.content)));
}

bool srcml_node::is_white_space() const {

  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
  return (xmlReaderTypes)type == XML_READER_TYPE_TEXT && content && isspace((*content)[0]);

}

bool srcml_node::is_new_line() const {

  return (xmlReaderTypes)type == XML_READER_TYPE_TEXT && content && (*content)[0] == '\n';

}

bool srcml_node::is_text() const {

  return (xmlReaderTypes)type == XML_READER_TYPE_TEXT;
}
