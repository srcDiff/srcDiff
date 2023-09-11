/*
  srcml_node.hpp

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

#ifndef INCLUDED_SRCML_NODE_HPP
#define INCLUDED_SRCML_NODE_HPP

#include <srcml.h>

#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <memory>

#include <boost/any.hpp>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

class srcml_node {

public:

  /*
  @todo: extract srcml_namespace and srcml_attribute into separate files
  introduce private members and accessors
  */
  class srcml_namespace {



  public:

    std::string uri;
    std::optional<std::string> prefix;

    srcml_namespace(const std::string & uri = std::string(),
                    const std::optional<std::string> & prefix = std::optional<std::string>());
    srcml_namespace(const srcml_namespace & ns);
    srcml_namespace(xmlNsPtr ns);

    std::string get_uri() const;
    std::optional<std::string> get_prefix() const;

    void set_uri(std::string input);
    void set_prefix(std::optional<std::string> input);

  };

  static std::shared_ptr<srcml_namespace> SRC_NAMESPACE;
  static std::shared_ptr<srcml_namespace> CPP_NAMESPACE;
  static std::shared_ptr<srcml_namespace> DIFF_NAMESPACE;
  static std::unordered_map<std::string, std::shared_ptr<srcml_namespace>> namespaces;
  
  class srcml_attribute {


  public:
    
    std::string name;
    std::optional<std::string> value;
    std::shared_ptr<srcml_namespace> ns;

    srcml_attribute(xmlAttrPtr attribute);
    srcml_attribute(const std::string & name = std::string(),
                    std::shared_ptr<srcml_namespace> ns = SRC_NAMESPACE,
                    std::optional<std::string> value = std::optional<std::string>());

    std::string full_name() const;
    void set_name(std::string input);
    void set_value(std::optional<std::string> input);
    const std::string & get_name() const;
    std::optional<std::string> get_value() const;


    friend std::ostream & operator<<(std::ostream & out, const srcml_attribute & that);
    bool operator==(const srcml_attribute & that) const;
    bool operator!=(const srcml_attribute & that) const;

  };

  enum srcml_node_type : unsigned int  { OTHER = 0, START = 1, END = 2, TEXT = 3, WS = 4 };

  typedef std::map<std::string, srcml_attribute> srcml_attribute_map;
  typedef std::pair<std::string, srcml_attribute> srcml_attribute_map_pair;
  typedef std::map<std::string, srcml_attribute>::const_iterator srcml_attribute_map_citr;
  typedef std::map<std::string, srcml_attribute>::iterator srcml_attribute_map_itr;

  srcml_node_type type;
  std::string name;
  std::shared_ptr<srcml_namespace> ns;
  std::optional<std::string> content;
  std::list<std::shared_ptr<srcml_namespace>> ns_definition;
  std::optional<std::shared_ptr<srcml_node>> parent;
  srcml_attribute_map attributes;

  bool temporary;
  bool empty;
  bool simple;

  int move;
  boost::any user_data;

  unsigned short extra;

  static std::shared_ptr<srcml_namespace> get_namespace(xmlNsPtr ns);

public:

  srcml_node();
  srcml_node(const xmlNode & node, xmlElementType xml_type);
  srcml_node(const std::string & text);
  srcml_node(const srcml_node & node);
  srcml_node(srcml_node_type type, const std::string & name, const std::shared_ptr<srcml_namespace> & ns = SRC_NAMESPACE);

  ~srcml_node();

  void clear_attributes();

  void set_attributes(const srcml_attribute_map & input);
  void set_type(srcml_node_type input);
  void set_empty(bool input);
  void set_temporary(bool input);
  void set_parent(std::shared_ptr<srcml_node> input);
  void set_simple(bool input);
  void set_move(int input);
  void set_content(std::optional<std::string> input);
  void set_name(std::string input);

  const srcml_attribute_map & get_attributes() const;
  srcml_node_type get_type() const;
  std::string get_name() const;
  std::optional<std::string> get_content() const;
  int get_move() const;

  std::string full_name() const;
  const srcml_node::srcml_attribute * get_attribute(const std::string & attribute) const;
  srcml_node::srcml_attribute * get_attribute(const std::string & attribute);
  const std::string * get_attribute_value(const std::string & attribute) const;
  std::string * get_attribute_value(const std::string & attribute);

  bool operator==(const srcml_node & node) const;
  bool operator!=(const srcml_node & node) const;

  bool is_start() const;
  bool is_end() const;
  bool is_empty() const;
  bool is_text() const;
  bool is_whitespace() const;
  bool is_temporary() const;
  bool is_simple() const;
  bool is_new_line() const;
  bool is_open_tag() const;


  friend std::ostream & operator<<(std::ostream & out, const srcml_node & node);

};

#endif