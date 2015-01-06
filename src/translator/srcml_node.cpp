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

typedef std::map<std::string, srcml_node*> NodeMap;

NodeMap starttags;
NodeMap endtags;

srcml_node::srcml_node(const xmlNode & node, bool is_archive) {

  type = node.type;

  name = strdup((const char *)node.name);

  content = 0;
  if(node.content)
    content = strdup((const char *)node.content);
  ns = 0;

  if(node.ns) {

    ns = new xNs;

    ns->href = 0;

    if(node.ns->href)
      ns->href = strdup((const char *)node.ns->href);

    ns->prefix = 0;
    if(node.ns->prefix)
      ns->prefix = strdup((const char *)node.ns->prefix);
  }

  xmlAttrPtr attribute = node.properties;
  properties = 0;

  xmlNsPtr ns = node.nsDef;
  xAttr * ns_attr = 0;
  if(strcmp((const char*)name, "unit") == 0 && ns) {

    while(is_archive && ns && strcmp((const char *)ns->href, "http://www.sdml.info/srcML/cpp") != 0)
        ns = ns->next;


    xAttr * attr = new xAttr;

    std::string ns_name = "xmlns";
    if(ns->prefix) {

      ns_name += ":";
      ns_name += (const char *)ns->prefix;

    }

    attr->name = strdup((const char *)ns_name.c_str());
    attr->value = strdup((const char *)ns->href);
    attr->next = 0;

    properties = attr;
    ns_attr = attr;

    ns = ns->next;

    while(!is_archive && ns) {

      xAttr * nattr = new xAttr;

      std::string ns_name = "xmlns";
      if(ns->prefix) {

        ns_name += ":";
        ns_name += (const char *)ns->prefix;

      }

      nattr->name = strdup((const char *)ns_name.c_str());
      nattr->value = strdup((const char *)ns->href);
      nattr->next = 0;

      attr->next = nattr;
      attr = nattr;
      ns_attr = nattr;

      ns = ns->next;

    }

  }

  if(attribute) {

    xAttr * attr = new xAttr;
    attr->name = strdup((const char *)attribute->name);
    attr->value = strdup((const char *)attribute->children->content);
    attr->next = 0;

    if(!properties)
      properties = attr;
    else
      ns_attr->next = attr;

    attribute = attribute->next;

    while (attribute) {

      xAttr * nattr = new xAttr;
      nattr->name = strdup((const char *)attribute->name);
      nattr->value = strdup((const char *)attribute->children->content);
      nattr->next = 0;

      attr->next = nattr;
      attr = nattr;

      attribute = attribute->next;

    }
  }

  extra = node.extra;
  is_empty = node.extra;

  parent = 0;

  free = false;
  move = 0;
  nest = 0;

}

srcml_node::srcml_node(xmlElementType type, const char * name, xNs * ns, const char * content, xAttr * properties, unsigned short extra,
  const char * parent, bool is_empty, bool free, int move, int nest)
  : type(type), name(name), ns(ns), content(content), properties(properties), extra(extra), parent(parent), is_empty(is_empty), free(true), move(0), nest(0) {}

srcml_node::srcml_node(const srcml_node & node) : type(node.type), extra(node.extra), is_empty(node.is_empty), free(node.free), move(node.move), nest(node.nest) {


  name = strdup((const char *)node.name);

  content = 0;
  if(node.content)
    content = strdup((const char *)node.content);

  ns = 0;
  if(node.ns) {

    ns = new xNs;

    ns->href = 0;

    if(node.ns->href)
      ns->href = strdup((const char *)node.ns->href);

    ns->prefix = 0;
    if(node.ns->prefix)
      ns->prefix = strdup((const char *)node.ns->prefix);
  }

  xAttr * attribute = node.properties;
  properties = 0;
  if(attribute) {

    xAttr * attr;
    attr = new xAttr;
    attr->name = strdup((const char *)attribute->name);
    attr->value = strdup((const char *)attribute->value);
    attr->next = 0;

    properties = attr;;

    attribute = attribute->next;

    while (attribute) {

      xAttr * nattr = new xAttr;
      nattr->name = strdup((const char *)attribute->name);
      nattr->value = strdup((const char *)attribute->value);
      nattr->next = 0;

      attr->next = nattr;
      attr = nattr;

      attribute = attribute->next;

    }
  }

  if(node.parent)
    parent = strdup(node.parent);
  else
    parent = 0;

}

void srcml_node::freeXAttr(xAttr * properties) {

  xAttr * attr = properties;
  while(attr) {

    xAttr * save_attr = attr;
    attr = attr->next;

    free((void *)save_attr->name);
    free((void *)save_attr->value);
    delete save_attr;

  }  

}

srcml_node::~srcml_node() {

 if(ns) {

    if(ns->href)
      free((void *)ns->href);

    if(ns->prefix)
      free((void *)ns->prefix);

    delete ns;
  }

  freeXAttr(properties);

  if(strcmp(name, "text") != 0)
    free((void *)name);

  if(content)
    free((void *)content);

  if(parent)
    free((void *)parent);

}

bool operator==(const srcml_node & node) {

  return type == node.type &&
    (strcmp((char*) name, (char*) node.name) == 0) && 
    (((xmlReaderTypes)type != XML_READER_TYPE_TEXT && (xmlReaderTypes)type != XML_READER_TYPE_SIGNIFICANT_WHITESPACE) ||
      (strcmp((char*) content, (char*) node.content) == 0));
}

bool srcml_node::is_white_space() {

  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
  return (xmlReaderTypes)type == XML_READER_TYPE_TEXT && isspace((char)content[0]);

}

bool srcml_node::is_new_line() {

  return (xmlReaderTypes)type == XML_READER_TYPE_TEXT && content[0] == '\n';

}

bool srcml_node::is_text() {

  return (xmlReaderTypes)type == XML_READER_TYPE_TEXT;
}
