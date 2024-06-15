// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file node.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NODE_HPP
#define INCLUDED_NODE_HPP

#include <srcml.h>
#include <namespace.hpp>
#include <attribute.hpp>
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

    enum node_type : unsigned int  { OTHER = 0, START = 1, END = 2, TEXT = 3, WS = 4 };

    typedef std::map<std::string, attribute, std::greater<std::string>> attribute_map;
    typedef std::pair<std::string, attribute> attribute_map_pair;
    typedef std::pair<const std::string &, attribute> attribute_map_cpair;
    typedef std::map<std::string, attribute, std::greater<std::string>>::const_iterator attribute_map_citr;
    typedef std::map<std::string, attribute, std::greater<std::string>>::iterator attribute_map_itr;

  class node {

  public:

    node();
    node(const xmlNode & node, xmlElementType xml_type);
    node(const std::string & text);
    node(const node & node);
    node(node_type type, const std::string & name, const std::shared_ptr<name_space> & ns = name_space::SRC_NAMESPACE);

    ~node();

    void clear_attributes();

    void set_attributes(const attribute_map & input);
    void emplace_attribute(const std::string & type, const attribute & attr);
    void set_type(node_type input);
    void set_empty(bool input);
    void set_temporary(bool input);
    void set_parent(std::shared_ptr<node> input);
    void set_simple(bool input);
    void set_move(int input);
    void set_content(std::optional<std::string> input);
    void set_name(std::string input);

    const attribute_map & get_attributes() const;
    node_type get_type() const;
    const std::string & get_name() const;
    const std::optional<std::string> & get_content() const;
    int get_move() const;
    std::shared_ptr<node> get_parent() const;
    std::shared_ptr<name_space> get_namespace() const;

    std::string full_name() const;
    const attribute * get_attribute(const std::string & attribute) const;
    attribute * get_attribute(const std::string & attribute);
    const std::optional<std::string> & get_attribute_value(const std::string & attribute) const;

    void merge(const node & that);
    void merge_attributes(const attribute_map & that);

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
    std::shared_ptr<name_space> ns;
    std::optional<std::string> content;
    std::list<std::shared_ptr<name_space>> ns_definition;
    std::shared_ptr<node> parent;
    attribute_map attributes;

    bool temporary;
    bool empty;
    bool simple;

    int move;
    std::any user_data;

    unsigned short extra;

  };

  std::ostream & operator<<(std::ostream & out, const node & node);
  
}
#endif
