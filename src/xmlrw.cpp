/*
  xmlrw.cpp

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

#include "xmlrw.hpp"
#include <iostream>
#include <cstring>
#include <map>
#include <string>

typedef std::map<std::string, xmlNode*> NodeMap;

NodeMap starttags;
NodeMap endtags;

Node createInternalNode(xmlNode & node) {

  Node xnode;
  xnode.type = node.type;
  xnode.name = (const char *)node.name;
  if(node.content)
    xnode.content = (const char *)node.content;

  xnode.ns.href = (const char *)node.ns->href;
  xnode.ns.prefix = (const char *)node.ns->prefix;

  xmlAttrPtr attribute = node.properties;
  while (attribute) {

    xAttr attr;
    attr.name = (const char *)attribute->name;
    attr.value = (const char *)attribute->children->content;

    attr.ns.href = (const char *)attribute->ns->href;
    attr.ns.prefix = (const char *)attribute->ns->prefix;    

    xnode.properties.push_back(attr);
    attribute = attribute->next;
  }

  xnode.is_empty = node.extra;

  return xnode;
}

bool operator==(const xmlNode& n1, const xmlNode& n2) {

  return n1.type == n2.type &&
    (strcmp((char*) n1.name, (char*) n2.name) == 0) && (
                                                        ((xmlReaderTypes)n1.type != XML_READER_TYPE_TEXT && (xmlReaderTypes)n1.type != XML_READER_TYPE_SIGNIFICANT_WHITESPACE) ||
                                                        (strcmp((char*) n1.content, (char*) n2.content) == 0)
                                                        );
}

xmlNode* getRealCurrentNode(xmlTextReaderPtr reader) {

  xmlNode* pnode = getCurrentNode(reader);

  //  pnode->extra = xmlTextReaderIsEmptyElement(reader);

  return pnode;
}

Node getCurrentXNode(xmlTextReaderPtr reader) {

  xmlNode* curnode = xmlTextReaderCurrentNode(reader);

  curnode->extra = xmlTextReaderIsEmptyElement(reader);

  return createInternalNode(*curnode);
}

xmlNode* getCurrentNode(xmlTextReaderPtr reader) {

  xmlNode* curnode = xmlTextReaderCurrentNode(reader);

  xmlNode* node = 0;
  if (!xmlTextReaderIsEmptyElement(reader) && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT && curnode->properties == 0) {

    NodeMap::iterator lb = starttags.lower_bound((const char*) curnode->name);
    if (lb != starttags.end() && !(starttags.key_comp()((const char*) curnode->name, lb->first))) {

      node = lb->second;
    } else {

      node = xmlCopyNode(curnode, 2);
      node->extra = 0;
      starttags.insert(lb, NodeMap::value_type((const char*) curnode->name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {

    NodeMap::iterator lb = endtags.lower_bound((const char*) curnode->name);
    if (lb != endtags.end() && !(endtags.key_comp()((const char*) curnode->name, lb->first))) {

      node = lb->second;
    } else {

      node = xmlCopyNode(curnode, 2);
      node->extra = 0;
      endtags.insert(lb, NodeMap::value_type((const char*) curnode->name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    node = xmlCopyNode(curnode, 2);
    node->extra = xmlTextReaderIsEmptyElement(reader) + 4;
  } else {
    node = xmlCopyNode(curnode, 2);
    node->extra = 4;
  }

  node->type = (xmlElementType) xmlTextReaderNodeType(reader);

  return node;
}

xNode * split_text(const char * characters_start, const char * characters_end) {

  xNode * text = new xNode;
  text->type = (xmlElementType)XML_READER_TYPE_TEXT;
  text->name = (const xmlChar *)"text";
  const char * content = strndup((const char *)characters_start, characters_end  - characters_start);
  text->content = (xmlChar *)content;

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
void outputXML(xmlTextReaderPtr reader, xmlTextWriterPtr writer) {

  bool isemptyelement = false;

  switch (xmlTextReaderNodeType(reader)) {
  case XML_READER_TYPE_ELEMENT:

    // record if this is an empty element since it will be erased by the attribute copying
    isemptyelement = xmlTextReaderIsEmptyElement(reader) > 0;

    // start the element
    xmlTextWriterStartElement(writer, xmlTextReaderConstName(reader));

    // copy all the attributes
    while (xmlTextReaderMoveToNextAttribute(reader)) {
      xmlTextWriterWriteAttribute(writer, xmlTextReaderConstName(reader), xmlTextReaderConstValue(reader));
    }

    // end now if this is an empty element
    if (isemptyelement) {
      xmlTextWriterEndElement(writer);
    }

    break;

  case XML_READER_TYPE_END_ELEMENT:
    xmlTextWriterEndElement(writer);
    break;

  case XML_READER_TYPE_COMMENT:
    xmlTextWriterWriteComment(writer, xmlTextReaderConstValue(reader));
    break;

  case XML_READER_TYPE_TEXT:
  case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:

    // output the UTF-8 buffer escaping the characters.  Note that the output encoding
    // is handled by libxml
    for (unsigned char* p = (unsigned char*) xmlTextReaderConstValue(reader); *p != 0; ++p) {
      if (*p == '&')
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&amp;", 5);
      else if (*p == '<')
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&lt;", 4);
      else if (*p == '>')
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&gt;", 4);
      else
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) p, 1);
    }
    break;

  default:
    break;
  }
}

// output current XML node in reader
void outputXML(xmlTextReaderPtr reader, xmlTextWriterPtr writer, const char* name) {

  bool isemptyelement = false;

  switch (xmlTextReaderNodeType(reader)) {
  case XML_READER_TYPE_ELEMENT:

    // record if this is an empty element since it will be erased by the attribute copying
    isemptyelement = xmlTextReaderIsEmptyElement(reader) > 0;

    // start the element
    xmlTextWriterStartElement(writer, BAD_CAST name);

    // copy all the attributes
    while (xmlTextReaderMoveToNextAttribute(reader)) {
      xmlTextWriterWriteAttribute(writer, xmlTextReaderConstName(reader), xmlTextReaderConstValue(reader));
    }

    // end now if this is an empty element
    if (isemptyelement) {
      xmlTextWriterEndElement(writer);
    }

    break;

  case XML_READER_TYPE_END_ELEMENT:
    xmlTextWriterEndElement(writer);
    break;

  case XML_READER_TYPE_COMMENT:
    xmlTextWriterWriteComment(writer, xmlTextReaderConstValue(reader));
    break;

  case XML_READER_TYPE_TEXT:
  case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:

    // output the UTF-8 buffer escaping the characters.  Note that the output encoding
    // is handled by libxml
    for (unsigned char* p = (unsigned char*) xmlTextReaderConstValue(reader); *p != 0; ++p) {
      if (*p == '&')
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&amp;", 5);
      else if (*p == '<')
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&lt;", 4);
      else if (*p == '>')
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&gt;", 4);
     else
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) p, 1);
    }
    break;

  default:
    break;
  }
}

// output current XML node in reader
void outputNode(xmlNode& node, xmlTextWriterPtr writer) {

  bool isemptyelement = false;

  switch (node.type) {
  case XML_READER_TYPE_ELEMENT:

    // record if this is an empty element since it will be erased by the attribute copying
    isemptyelement = node.extra & 0x1;

    // start the element
    if (node.ns && node.ns->prefix) {

      std::string s = ((char*) node.ns->prefix);
      s += ":";
      s += (char*) node.name;

      xmlTextWriterStartElement(writer, BAD_CAST s.c_str());

    } else

      xmlTextWriterStartElement(writer, node.name);

    // copy all the namespaces
    if(strcmp((const char *)node.name, "unit") == 0) {
      xmlNsPtr ns = node.nsDef;
      while (ns) {

        if(ns->type == XML_LOCAL_NAMESPACE) {
          std::string name = "xmlns";

          if(ns->prefix) {

            name += ":";
            name += (char *)ns->prefix;
          }

          xmlTextWriterWriteAttribute(writer, (xmlChar *)name.c_str(), ns->href);
        }

        ns = ns->next;
      }
    }

    // copy all the attributes
    {
      xmlAttrPtr attribute = node.properties;
      while (attribute) {

        xmlTextWriterWriteAttribute(writer, attribute->name, attribute->children->content);
        attribute = attribute->next;
      }
    }

    // end now if this is an empty element
    if (isemptyelement) {

      xmlTextWriterEndElement(writer);
    }

    break;

  case XML_READER_TYPE_END_ELEMENT:
    xmlTextWriterEndElement(writer);
    break;

  case XML_READER_TYPE_COMMENT:
    xmlTextWriterWriteComment(writer, node.content);
    break;

  case XML_READER_TYPE_TEXT:
  case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:

    // output the UTF-8 buffer escaping the characters.  Note that the output encoding
    // is handled by libxml
    for (unsigned char* p = (unsigned char*) node.content; *p != 0; ++p) {
      if (*p == '&')
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&amp;", 5);
      else if (*p == '<')
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&lt;", 4);
      else if (*p == '>')
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&gt;", 4);
      else
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) p, 1);
    }
    break;

  default:
    break;
  }
}
