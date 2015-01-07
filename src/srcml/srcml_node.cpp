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

srcml_node::srcml_node(const xmlNode & node, bool is_archive) : type(node.type), extra(node.extra), is_empty(node.extra), free(false), move(0) {

  name = std::string((const char *)node.name);

  if(content)
    content = std::string((const char *)node.content);
  ns = 0;

  if(node.ns) {

    ns = new srcml_ns;

    if(node.ns->href)
      ns->href = std::string((const char *)node.ns->href);

    if(node.ns->prefix)
      ns->prefix = std::string((const char *)node.ns->prefix);
  }

  xmlAttrPtr attribute = node.properties;
  properties = 0;

  xmlNsPtr ns = node.nsDef;
  srcml_attr * ns_attr = 0;
  if(name == "unit" && ns) {

    while(is_archive && ns && (const char *)ns->href != std::string("http://www.sdml.info/srcML/cpp"))
        ns = ns->next;


    srcml_attr * attr = new srcml_attr;

    std::string ns_name = "xmlns";
    if(ns->prefix) {

      ns_name += ":";
      ns_name += (const char *)ns->prefix;

    }

    attr->name = ns_name;
    attr->value = std::string((const char *)ns->href);
    attr->next = 0;

    properties = attr;
    ns_attr = attr;

    ns = ns->next;

    while(!is_archive && ns) {

      srcml_attr * nattr = new srcml_attr;

      std::string ns_name = "xmlns";
      if(ns->prefix) {

        ns_name += ":";
        ns_name += (const char *)ns->prefix;

      }

      nattr->name = ns_name;
      nattr->value = std::string((const char *)ns->href);
      nattr->next = 0;

      attr->next = nattr;
      attr = nattr;
      ns_attr = nattr;

      ns = ns->next;

    }

  }

  if(attribute) {

    srcml_attr * attr = new srcml_attr;
    attr->name = strdup((const char *)attribute->name);
    attr->value = strdup((const char *)attribute->children->content);
    attr->next = 0;

    if(!properties)
      properties = attr;
    else
      ns_attr->next = attr;

    attribute = attribute->next;

    while (attribute) {

      srcml_attr * nattr = new srcml_attr;
      nattr->name = strdup((const char *)attribute->name);
      nattr->value = strdup((const char *)attribute->children->content);
      nattr->next = 0;

      attr->next = nattr;
      attr = nattr;

      attribute = attribute->next;

    }
  }

}

srcml_node::srcml_node(xmlElementType type, const std::string & name, srcml_ns * ns, const boost::optional<std::string> & content, srcml_attr * properties, unsigned short extra,
  const boost::optional<std::string> & parent, bool is_empty, bool free, int move)
  : type(type), name(name), ns(ns), content(content), properties(properties), extra(extra), parent(parent), is_empty(is_empty), free(false), move(0) {}

srcml_node::srcml_node(xmlElementType type, const std::string & name, const srcml_ns & ns) : type(type), name(name), ns(new srcml_ns(ns)),
 properties(0), extra(0), is_empty(false), free(false), move(0) {}

srcml_node::srcml_node(const srcml_node & node) : type(node.type), name(node.name), content(node.content), extra(node.extra),
  is_empty(node.is_empty), free(true), move(0) {

  ns = 0;
  if(node.ns) {

    ns = new srcml_ns;

    ns->href = node.ns->href;

    ns->prefix = node.ns->prefix;
  }

  srcml_attr * attribute = node.properties;
  properties = 0;
  if(attribute) {

    srcml_attr * attr;
    attr = new srcml_attr;
    attr->name = attribute->name;
    attr->value = attribute->value;
    attr->next = 0;

    properties = attr;;

    attribute = attribute->next;

    while (attribute) {

      srcml_attr * nattr = new srcml_attr;
      nattr->name = attribute->name;
      nattr->value = attribute->value;
      nattr->next = 0;

      attr->next = nattr;
      attr = nattr;

      attribute = attribute->next;

    }
  }

  parent = node.parent;

}

void srcml_attr::free_srcml_attr(srcml_attr * properties) {

  srcml_attr * attr = properties;
  while(attr) {

    srcml_attr * save_attr = attr;
    attr = attr->next;

    delete save_attr;

  }  

}

srcml_ns::srcml_ns(const srcml_ns & ns) : href(ns.href), prefix(ns.prefix) {}

srcml_node::~srcml_node() {

 if(ns) delete ns;

  srcml_attr::free_srcml_attr(properties);

}

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
