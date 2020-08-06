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

#include <srcdiff_options.hpp>

#include <srcml.h>

#include <string>
#include <list>
#include <memory>

#include <boost/optional.hpp>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

class srcml_node {

public:

  class srcml_ns {

  public:

    std::string href;
    boost::optional<std::string> prefix;

    srcml_ns(const std::string & href = std::string(), const boost::optional<std::string> & prefix = boost::optional<std::string>())
      : href(href), prefix(prefix) {}

    srcml_ns(const srcml_ns & ns);

  };

  class srcml_attr {

  public:

    std::string name;
    boost::optional<std::string> value;

    srcml_attr(const std::string & name = std::string(), const boost::optional<std::string> & value = boost::optional<std::string>())
      : name(name), value(value) {}

    bool operator==(const srcml_attr & attr) const;
    bool operator!=(const srcml_attr & attr) const;

  };

  xmlElementType type;
  std::string name;
  srcml_ns ns;
  boost::optional<std::string> content;
  std::list<srcml_attr> properties;
  unsigned short extra;
  boost::optional<std::shared_ptr<srcml_node>> parent;

  bool is_empty;
  bool free;

  int move;

  bool is_simple;
  bool is_temporary;

public:

  srcml_node(const xmlNode & node, bool is_archive);

  srcml_node(xmlElementType type = XML_ELEMENT_NODE, const std::string & name = std::string(), const srcml_ns & ns = srcml_ns(),
    const boost::optional<std::string> & content = boost::optional<std::string>(), const std::list<srcml_attr> & properties = std::list<srcml_attr>(),
    unsigned short extra = 0, const boost::optional<std::shared_ptr<srcml_node>> & parent = boost::optional<std::shared_ptr<srcml_node>>(), bool is_empty = false);

  srcml_node(const srcml_node & node);

  ~srcml_node();

  bool operator==(const srcml_node & node) const;

  friend std::ostream & operator<<(std::ostream & out, const srcml_node & that);

  bool is_open_tag() const;
  bool is_close_tag() const;

  bool is_empty_tag() const;

  bool is_text() const;
  bool is_white_space() const;
  bool is_new_line() const;

};

#endif
