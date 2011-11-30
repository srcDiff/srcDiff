/*
  diffe2diff.cpp

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

*/

#ifndef INCLUDED_XMLRW_HPP
#define INCLUDED_XMLRW_HPP

#include <string>
#include <vector>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

struct xNs {

  //xNs * next;
  std::string href;
  std::string prefix;

};

struct xAttr {

  std::string name;
  std::string value;
  xNs ns;
};

struct xNode {

  xmlElementType type;
  std::string name;
  xNs ns;
  std::string content;
  //xNs * nsDef;
  std::vector<xmlAttr> properties;
  
};

bool operator==(const xmlNode& n1, const xmlNode& n2);

xmlNode* getRealCurrentNode(xmlTextReaderPtr reader);

inline bool iselement(const xmlTextReaderPtr& reader, const xmlChar* element_name) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT &&
    xmlStrEqual(xmlTextReaderConstName(reader), element_name);
}

inline bool isendelement(const xmlTextReaderPtr& reader, const xmlChar* element_name) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
    xmlStrEqual(xmlTextReaderConstName(reader), element_name);
}

inline bool iselement(const xmlTextReaderPtr& reader) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT;
}

inline bool isendelement(const xmlTextReaderPtr& reader) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT;
}

xmlNode* getCurrentNode(xmlTextReaderPtr reader);

void eat_element(xmlTextReaderPtr& reader);

void outputXML(xmlTextReaderPtr reader, xmlTextWriterPtr writer);

void outputNode(xmlNode& node, xmlTextWriterPtr writer);

#endif
