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

  srcml_ns(const std::string & href = std::string(), const boost::optional<std::string> & prefix = boost::optional<std::string>())
    : href(href), prefix(prefix) {}

  srcml_ns(const srcml_ns & ns);

  std::string href;
  boost::optional<std::string> prefix;

};

class srcml_attr {

public:

  srcml_attr(srcml_attr * next = 0, const std::string & name = std::string(),
    const boost::optional<std::string> & value = boost::optional<std::string>())
    : next(next), name(name), value(value) {}

  srcml_attr * next;
  std::string name;
  boost::optional<std::string> value;

  static void free_srcml_attr(srcml_attr * properties);


};

class srcml_node {

public:

  xmlElementType type;
  std::string name;
  srcml_ns * ns;
  boost::optional<std::string> content;
  srcml_attr * properties;
  unsigned short extra;
  boost::optional<std::string> parent;

  bool is_empty;
  bool free;

  int move;

public:

  srcml_node(const xmlNode & node, bool is_archive);

  srcml_node(xmlElementType type = XML_ELEMENT_NODE, const std::string & name = std::string(), srcml_ns * ns = nullptr,
    const boost::optional<std::string> & content = boost::optional<std::string>(), srcml_attr * properties = nullptr, unsigned short extra = 0,
    const boost::optional<std::string> & parent = boost::optional<std::string>(), bool is_empty = false, bool free = false,
    int move = 0);

  srcml_node(xmlElementType type, const std::string & name, const srcml_ns & ns);

  srcml_node(const srcml_node & node);

  ~srcml_node();

  bool operator==(const srcml_node & node) const;

  bool is_white_space() const;

  bool is_new_line() const;

  bool is_text() const;

};

#endif
