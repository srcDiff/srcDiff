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

#include <Options.hpp>
#include <srcDiffOptions.hpp>

struct xNs {

  const char * href;
  const char * prefix;

};

struct xAttr {

  xAttr * next;
  const char * name;
  const char * value;

};

struct xNode {

  xmlElementType type;
  const char * name;
  xNs * ns;
  const char * content;
  xAttr * properties;
  unsigned short extra;

  bool is_empty;
  bool free;

  int move;
  int nest;
  
};

typedef xNode * xNodePtr;
typedef xAttr * xAttrPtr;

xNode * createInternalNode(xmlNode & node);

bool operator==(const xNode& n1, const xNode& n2);

xNode* getRealCurrentNode(xmlTextReaderPtr reader, OPTION_TYPE options, int context);

xNode * copyXNode(xNode * node);

xNode * split_text(const char * characters_start, const char * characters_end);

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

xNode* getCurrentNode(xmlTextReaderPtr reader, OPTION_TYPE options, int context);

void freeXNode(xNode * node);

void eat_element(xmlTextReaderPtr& reader);

void outputXML(xmlTextReaderPtr reader, xmlTextWriterPtr writer);

void outputNode(const xNode& node, xmlTextWriterPtr writer);

#endif
