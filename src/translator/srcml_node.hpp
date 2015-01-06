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

#ifndef INCLUDED_SRCML_NODE_HPP
#define INCLUDED_SRCML_NODE_HPP

#include <string>
#include <vector>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include <srcdiff_options.hpp>

#include <srcml.h>

class srcml_ns {

public:

  srcml_ns(const char * href = 0, const char * prefix = 0) : href(href), prefix(prefix) {}

  const char * href;
  const char * prefix;

};

class srcml_attr {

public:

  srcml_attr(srcml_attr * next = 0, const char * name = 0, const char * value = 0) : next(next), name(name), value(value) {}

  srcml_attr * next;
  const char * name;
  const char * value;

};

class srcml_node {

public:

  xmlElementType type;
  const char * name;
  srcml_ns * ns;
  const char * content;
  srcml_attr * properties;
  unsigned short extra;
  const char * parent;

  bool is_empty;
  bool free;

  int move;
  int nest;

private:

void free_srcml_attr(srcml_attr * properties);

public:

  srcml_node(const xmlNode & node, bool is_archive);

  srcml_node(xmlElementType type = XML_ELEMENT_NODE, const char * name = 0, srcml_ns * ns = 0, const char * content = 0, srcml_attr * properties = 0, unsigned short extra = 0,
    const char * parent = 0, bool is_empty = false, bool free = false, int move = 0, int nest = 0);

  srcml_node(const srcml_node & node);

  ~srcml_node();

  bool operator==(const srcml_node & node);

  bool is_white_space();

  bool is_new_line();

  bool is_text();

};

#endif
