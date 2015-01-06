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

srcml_node * createInternalNode(xmlNode & node, bool is_archive) {

}

void freeXAttr(xAttrPtr properties) {

  xAttrPtr attr = properties;
  while(attr) {

    xAttr * save_attr = attr;
    attr = attr->next;

    free((void *)save_attr->name);
    free((void *)save_attr->value);
    delete save_attr;

  }  

}

void freesrcml_node(srcml_node * node) {

  if(node->ns) {

    if(node->ns->href)
      free((void *)node->ns->href);

    if(node->ns->prefix)
      free((void *)node->ns->prefix);

    delete node->ns;
  }

  freeXAttr(node->properties);

  if(strcmp(node->name, "text") != 0)
    free((void *)node->name);

  if(node->content)
    free((void *)node->content);

  if(node->parent)
    free((void *)node->parent);

  delete node;

}

bool operator==(const srcml_node& n1, const srcml_node& n2) {

  return n1.type == n2.type &&
    (strcmp((char*) n1.name, (char*) n2.name) == 0) && (
                                                        ((xmlReaderTypes)n1.type != XML_READER_TYPE_TEXT && (xmlReaderTypes)n1.type != XML_READER_TYPE_SIGNIFICANT_WHITESPACE) ||
                                                        (strcmp((char*) n1.content, (char*) n2.content) == 0)
                                                        );
}

srcml_node* getRealCurrentNode(xmlTextReaderPtr reader, OPTION_TYPE options, int context) {

  srcml_node* pnode = getCurrentNode(reader, options, context);

  //  pnode->extra = xmlTextReaderIsEmptyElement(reader);

  return pnode;
}

srcml_node * copysrcml_node(srcml_nodePtr node) {


  srcml_node * srcml_node = new srcml_node;

  srcml_node->type = node->type;

  srcml_node->name = strdup((const char *)node->name);

  srcml_node->content = 0;
  if(node->content)
    srcml_node->content = strdup((const char *)node->content);
  srcml_node->ns = 0;

  if(node->ns) {

    srcml_node->ns = new xNs;

    srcml_node->ns->href = 0;

    if(node->ns->href)
      srcml_node->ns->href = strdup((const char *)node->ns->href);

    srcml_node->ns->prefix = 0;
    if(node->ns->prefix)
      srcml_node->ns->prefix = strdup((const char *)node->ns->prefix);
  }

  xAttr * attribute = node->properties;
  srcml_node->properties = 0;
  if(attribute) {

    xAttr * attr;
    attr = new xAttr;
    attr->name = strdup((const char *)attribute->name);
    attr->value = strdup((const char *)attribute->value);
    attr->next = 0;

    srcml_node->properties = attr;;

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

  srcml_node->extra = node->extra;
  srcml_node->is_empty = node->extra;

  if(node->parent)
    srcml_node->parent = strdup(node->parent);
  else
    srcml_node->parent = 0;

  srcml_node->free = true;
  srcml_node->move = 0;
  srcml_node->nest = 0;

  return srcml_node;
}

srcml_node* getCurrentNode(xmlTextReaderPtr reader, OPTION_TYPE options, int context) {

  xmlNode* curnode = xmlTextReaderCurrentNode(reader);

  std::string full_name;
  if(curnode->ns && (const char*)curnode->ns->prefix) {

    full_name = (const char*)curnode->ns->prefix;
    full_name += ":";

  }

  full_name += (const char*)curnode->name;

  srcml_node * node = 0;
  if (!xmlTextReaderIsEmptyElement(reader) && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT && curnode->properties == 0) {

    NodeMap::iterator lb = starttags.lower_bound(full_name);
    if (lb != starttags.end() && !(starttags.key_comp()(full_name, lb->first))) {

      node = lb->second;
    } else {

      node = createInternalNode(*curnode, options & SRCML_OPTION_ARCHIVE);
      node->extra = 0;
      starttags.insert(lb, NodeMap::value_type(full_name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {

    NodeMap::iterator lb = endtags.lower_bound(full_name);
    if (lb != endtags.end() && !(endtags.key_comp()(full_name, lb->first))) {

      node = lb->second;
    } else {

      node = createInternalNode(*curnode, options & SRCML_OPTION_ARCHIVE);
      node->extra = 0;
      endtags.insert(lb, NodeMap::value_type(full_name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    node = createInternalNode(*curnode, options & SRCML_OPTION_ARCHIVE);
    node->free = true;
    node->extra = xmlTextReaderIsEmptyElement(reader);
  } else {
    node = createInternalNode(*curnode, options & SRCML_OPTION_ARCHIVE);
    node->free = true;
  }

  node->type = (xmlElementType) xmlTextReaderNodeType(reader);

  return node;
}

srcml_node * split_text(const char * characters_start, const char * characters_end) {

  srcml_node * text = new srcml_node;
  text->type = (xmlElementType)XML_READER_TYPE_TEXT;
  text->name = "text";
  text->content = 0;

  if(characters_start != characters_end) {

    const char * content = strndup((const char *)characters_start, characters_end  - characters_start);
    text->content = content;
  }
  text->ns = 0;
  text->properties = 0;
  text->is_empty = true;
  text->parent = 0;
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

// output current XML node in reader
void outputNode(const srcml_node & node, srcml_unit * unit) {

  bool isemptyelement = false;

  switch (node.type) {
  case XML_READER_TYPE_ELEMENT:

    // record if this is an empty element since it will be erased by the attribute copying
    isemptyelement = node.extra & 0x1;

    // start the element
    srcml_write_start_element(unit, node.ns->prefix, node.name, 0);

    // copy all the attributes
    {
      xAttr * attribute = node.properties;
      while (attribute) {

        srcml_write_attribute(unit, 0, attribute->name, 0, attribute->value);
        attribute = attribute->next;
      }
    }

    // end now if this is an empty element
    if (isemptyelement) {

      srcml_write_end_element(unit);
    }

    break;

  case XML_READER_TYPE_END_ELEMENT:
    srcml_write_end_element(unit);
    break;

  case XML_READER_TYPE_COMMENT:
    //xmlTextWriterWriteComment(unit, (const xmlChar *)node.content);
    break;

  case XML_READER_TYPE_TEXT:
  case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:

    // output the UTF-8 buffer escaping the characters.  Note that the output encoding
    // is handled by libxml
    srcml_write_string(unit, node.content);
    /*for (unsigned char* p = (unsigned char*) node.content; *p != 0; ++p) {
      if (*p == '&')
        xmlTextWriterWriteRawLen(unit, BAD_CAST (unsigned char*) "&amp;", 5);
      else if (*p == '<')
        xmlTextWriterWriteRawLen(unit, BAD_CAST (unsigned char*) "&lt;", 4);
      else if (*p == '>')
        xmlTextWriterWriteRawLen(unit, BAD_CAST (unsigned char*) "&gt;", 4);
      else
        xmlTextWriterWriteRawLen(unit, BAD_CAST (unsigned char*) p, 1);
    }*/
    break;

  default:
    break;
  }

}

bool is_white_space(const srcml_nodePtr node) {

  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]);

}

bool is_new_line(const srcml_nodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content[0] == '\n';

}

bool is_text(const srcml_nodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT;
}
