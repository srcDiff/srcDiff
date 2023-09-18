/*
  node.hpp

  Copyright (C) 2018 srcML, LLC. (www.srcML.org)

  This file is part of a translator from source code to srcReader

  srcReader is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  srcReader is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcML translator; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef INCLUDED_NODE_HPP
#define INCLUDED_NODE_HPP

#include <srcml.h>
#include <namespace.hpp>
#include <srcml_attribute.hpp>
#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <memory>
#include <optional>
#include <any>
#include <functional>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

namespace srcML {

  class node {

  public:

    enum node_type : unsigned int  { OTHER = 0, START = 1, END = 2, TEXT = 3, WS = 4 };

    typedef std::map<std::string, srcml_attribute, std::greater<std::string>> srcml_attribute_map;
    typedef std::pair<std::string, srcml_attribute> srcml_attribute_map_pair;
    typedef std::map<std::string, srcml_attribute, std::greater<std::string>>::const_iterator srcml_attribute_map_citr;
    typedef std::map<std::string, srcml_attribute, std::greater<std::string>>::iterator srcml_attribute_map_itr;

    node();
    node(const xmlNode & node, xmlElementType xml_type);
    node(const std::string & text);
    node(const node & node);
    node(node_type type, const std::string & name, const std::shared_ptr<srcML::name_space> & ns = SRC_NAMESPACE);

    ~node();

    void clear_attributes();

    void set_attributes(const srcml_attribute_map & input);
    void emplace_attribute(const std::string & type, const srcml_attribute & attr);
    void set_type(node_type input);
    void set_empty(bool input);
    void set_temporary(bool input);
    void set_parent(std::shared_ptr<node> input);
    void set_simple(bool input);
    void set_move(int input);
    void set_content(std::optional<std::string> input);
    void set_name(std::string input);

    const srcml_attribute_map & get_attributes() const;
    node_type get_type() const;
    const std::string & get_name() const;
    const std::optional<std::string> & get_content() const;
    int get_move() const;
    std::optional<std::shared_ptr<node>> get_parent() const;
    std::shared_ptr<srcML::name_space> get_namespace() const;

    std::string full_name() const;
    const srcml_attribute * get_attribute(const std::string & attribute) const;
    srcml_attribute * get_attribute(const std::string & attribute);
    const std::optional<std::string> & get_attribute_value(const std::string & attribute) const;

    bool operator==(const node & node) const;
    bool operator!=(const node & node) const;

    bool is_start() const;
    bool is_end() const;
    bool is_empty() const;
    bool is_text() const;
    bool is_whitespace() const;
    bool is_temporary() const;
    bool is_simple() const;
    bool is_new_line() const;
    bool is_open_tag() const;


    friend std::ostream & operator<<(std::ostream & out, const node & node);

  private: 

    node_type type;
    std::string name;
    std::shared_ptr<srcML::name_space> ns;
    std::optional<std::string> content;
    std::list<std::shared_ptr<srcML::name_space>> ns_definition;
    std::optional<std::shared_ptr<node>> parent;
    srcml_attribute_map attributes;

    bool temporary;
    bool empty;
    bool simple;

    int move;
    std::any user_data;

    unsigned short extra;

  };

}
#endif