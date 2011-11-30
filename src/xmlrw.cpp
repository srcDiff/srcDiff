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

typedef std::map<std::string, xNode*> NodeMap;

NodeMap starttags;
NodeMap endtags;

xNode * createInternalNode(xmlNode & node) {

  xNode * xnode = new xNode;

  xnode->type = node.type;

  xnode->name = strdup((const char *)node.name);

  xnode->content = 0;
  if(node.content)
    xnode->content = strdup((const char *)node.content);
  xnode->ns = 0;

  if(node.ns) {

    xnode->ns = new xNs;

  xnode->ns->href = 0;

  if(node.ns->href)
    xnode->ns->href = strdup((const char *)node.ns->href);

    xnode->ns->prefix = 0;
    if(node.ns->prefix)
      xnode->ns->prefix = strdup((const char *)node.ns->prefix);
  }

  xmlAttrPtr attribute = node.properties;
  xnode->properties = 0;
  if(attribute) {

   xAttr * attr;
   attr = new xAttr;
   attr->name = strdup((const char *)attribute->name);
   attr->value = strdup((const char *)attribute->children->content);
   attr->next = 0;

   xnode->properties = attr;;
    
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


  xnode->extra = node.extra;
  return xnode;
}

void freeXNode(xNode * node) {

  if(node->ns)
    delete node->ns;

  while(node->properties) {

    xAttr * attr = node->properties;
    node->properties = node->properties->next;
    delete attr;
    
  }
  
  delete node;
  
}

bool operator==(const xNode& n1, const xNode& n2) {

  return n1.type == n2.type &&
    (strcmp((char*) n1.name, (char*) n2.name) == 0) && (
                                                        ((xmlReaderTypes)n1.type != XML_READER_TYPE_TEXT && (xmlReaderTypes)n1.type != XML_READER_TYPE_SIGNIFICANT_WHITESPACE) ||
                                                        (strcmp((char*) n1.content, (char*) n2.content) == 0)
                                                        );
}

xNode* getRealCurrentNode(xmlTextReaderPtr reader) {

  xNode* pnode = getCurrentNode(reader);

  //  pnode->extra = xmlTextReaderIsEmptyElement(reader);

  return pnode;
}

xNode * getCurrentXNode(xmlTextReaderPtr reader) {

  xmlNode* curnode = xmlTextReaderCurrentNode(reader);

  curnode->extra = xmlTextReaderIsEmptyElement(reader);

  return createInternalNode(*curnode);
}

xNode* getCurrentNode(xmlTextReaderPtr reader) {

  xmlNode* curnode = xmlTextReaderCurrentNode(reader);

  xNode * node = 0;
  if (!xmlTextReaderIsEmptyElement(reader) && xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT && curnode->properties == 0) {

    NodeMap::iterator lb = starttags.lower_bound((const char*) curnode->name);
    if (lb != starttags.end() && !(starttags.key_comp()((const char*) curnode->name, lb->first))) {

      node = lb->second;
    } else {

      node = createInternalNode(*curnode);
      node->extra = 0;
      starttags.insert(lb, NodeMap::value_type((const char*) curnode->name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {

    NodeMap::iterator lb = endtags.lower_bound((const char*) curnode->name);
    if (lb != endtags.end() && !(endtags.key_comp()((const char*) curnode->name, lb->first))) {

      node = lb->second;
    } else {

      node = createInternalNode(*curnode);
      node->extra = 0;
      endtags.insert(lb, NodeMap::value_type((const char*) curnode->name, node));
    }

  } else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
    node = createInternalNode(*curnode);
    node->extra = xmlTextReaderIsEmptyElement(reader) + 4;
  } else {
    node = createInternalNode(*curnode);
    node->extra = 4;
  }

  node->type = (xmlElementType) xmlTextReaderNodeType(reader);

  return node;
}

xNode * split_text(const char * characters_start, const char * characters_end) {

  xNode * text = new xNode;
  text->type = (xmlElementType)XML_READER_TYPE_TEXT;
  text->name = "text";
  const char * content = strndup((const char *)characters_start, characters_end  - characters_start);
  text->content = content;

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
void outputNode(xNode& node, xmlTextWriterPtr writer) {

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

      xmlTextWriterStartElement(writer, (xmlChar *)node.name);

    // copy all the namespaces
    /*
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
    */

    // copy all the attributes
    {
      xAttr * attribute = node.properties;
      while (attribute) {

        xmlTextWriterWriteAttribute(writer, (const xmlChar *)attribute->name, (const xmlChar *)attribute->value);
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
    xmlTextWriterWriteComment(writer, (const xmlChar *)node.content);
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
