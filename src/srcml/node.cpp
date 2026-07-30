// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file node.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <node.hpp>

#include <srcml.h>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <unordered_map>

#ifdef __MINGW32__
#include <mingw32.hpp>
#endif

namespace srcML {

node_type xml_type2srcml_type(xmlElementType type) {
    static std::unordered_map<unsigned int, node_type> type_map = {

        { XML_READER_TYPE_ELEMENT, node_type::START },
        { XML_READER_TYPE_END_ELEMENT, node_type::END },
        { XML_READER_TYPE_TEXT, node_type::TEXT },
        { XML_READER_TYPE_SIGNIFICANT_WHITESPACE, node_type::WS },

    };

    try {
        return type_map.at((unsigned int)type);
    } catch(const std::out_of_range & error) {
        return node_type::OTHER;
    }

}

node::node()
    : type(node_type::OTHER), name(), ns(name_spaces::SRC_NAMESPACE), content(),
      ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(true), move(0), user_data(), extra(0) {}

node::node(const xmlNode & node, xmlElementType xml_type) 
    : type(xml_type2srcml_type(xml_type)), name(), ns(), content(),
      ns_definition(), parent(), attributes(), temporary(false), empty(node.extra), simple(true), move(0), user_data(), extra(node.extra) {

    name = std::string((const char *)node.name);

    if(node.content)
        content = std::string((const char *)node.content);

    ns = name_spaces::namespace_registry.get_namespace(node.ns);

    if(type != node_type::START) return;

    xmlNsPtr node_ns = node.nsDef;
    while(node_ns) {
        ns_definition.emplace_back(name_spaces::namespace_registry.get_namespace(node_ns));
        node_ns = node_ns->next;
    }

    xmlAttrPtr attr = node.properties;
    while (attr) {
        attribute new_attribute = attribute(attr);
        attributes.emplace(std::make_pair(new_attribute.full_name(), new_attribute));
        attr= attr->next;
    }

}

node::node(const std::string & text) : type(node_type::TEXT), name("text"), 
    ns(name_spaces::SRC_NAMESPACE), content(text), ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(true), move(0), user_data(), extra(0) {}

node::node(const node & node) : type(node.type), name(node.name), ns(node.ns),
    content(node.content), ns_definition(node.ns_definition), parent(node.parent), attributes(node.attributes), temporary(node.temporary), empty(node.empty), simple(node.simple), 
    move(node.move), user_data(node.user_data) {}

node::node(node_type type, const std::string & name, const std::shared_ptr<name_space> & ns) 
    : type(type), name(name), ns(ns), content(), ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(false), move(0), user_data(), 
      extra(0) {}

node::~node() {}

std::string node::full_name() const {

    if(ns->get_prefix()) return *ns->get_prefix() + ":" + name;

    return name;
} 

const attribute * node::get_attribute(const std::string & attribute) const {

    attribute_map_citr attribute_itr = attributes.find(attribute);
    if(attribute_itr == attributes.end()) return nullptr;
    return &attribute_itr->second;

}

attribute * node::get_attribute(const std::string & attribute) {

    attribute_map_itr attribute_itr = attributes.find(attribute);
    if(attribute_itr == attributes.end()) return nullptr;
    return &attribute_itr->second;

}

const std::optional<std::string> & node::get_attribute_value(const std::string & attribute) const {

    attribute_map_citr attribute_itr = attributes.find(attribute);
    assert(attribute_itr != attributes.end());

    return attribute_itr->second.get_value();

}

void node::merge(const node & that) {
    assert(this->get_name() == that.get_name());
    assert(this->get_namespace() == that.get_namespace());
    this->set_empty(this->is_empty() && that.is_empty());
    merge_attributes(that.get_attributes());
}

void node::merge_attributes(const attribute_map & that) {

    attribute_map same_attributes;
    attribute_map original_attributes;
    attribute_map modified_attributes;

    auto key_compare = [](
        const attribute_map_pair & a,
        const attribute_map_pair & b
    ) {
        // std::set_intersection and similar require their input elements to be
        // ordered according to this comparator function, so > must be used to
        // match how std::greater is used to order elements within the
        // attribute_maps that are passed in
        return a.first > b.first;
    };

    std::set_intersection(this->get_attributes().begin(), this->get_attributes().end(),
                          that.begin(), that.end(),
                          std::inserter(same_attributes, same_attributes.end()), key_compare);
    std::set_difference(this->get_attributes().begin(), this->get_attributes().end(),
                        that.begin(), that.end(),
                        std::inserter(original_attributes, original_attributes.end()), key_compare);
    std::set_difference(that.begin(), that.end(), 
                        this->get_attributes().begin(), this->get_attributes().end(),
                        std::inserter(modified_attributes, modified_attributes.end()), key_compare);

    for (attribute_map_cpair pair : same_attributes) {
        attributes.at(pair.first).merge(that.at(pair.first));
    }

    for (attribute_map_cpair pair : original_attributes) {
        attributes.at(pair.first).set_value(std::optional<std::string>(*this->get_attributes().at(pair.first).get_value() + "|"));
    }

    for (attribute_map_cpair pair : modified_attributes) {
        attributes.emplace(pair.first, attribute(pair.first, pair.second.get_ns(), std::optional<std::string>("|" + *that.at(pair.first).get_value())));
    }
}

bool node::is_equal(const node & node, bool ignore_pos_attr) const {

    std::function<bool (const attribute_map_pair&, const attribute_map_pair&)> attr_compare =
     [ignore_pos_attr](const attribute_map_pair& lhs, const attribute_map_pair& rhs) {
        if(lhs.first != rhs.first) return lhs.first > rhs.first;
        if(ignore_pos_attr && lhs.first.substr(0, 3) == "pos") return false;
        if(lhs.second == rhs.second) return false;
        if(!lhs.second.get_value())  return true;
        if(!rhs.second.get_value())  return false;
        return *lhs.second.get_value() > *rhs.second.get_value();
    };

    attribute_map attribute_diff;
    std::set_symmetric_difference(attributes.begin(), attributes.end(), node.attributes.begin(), node.attributes.end(),
                                  std::inserter(attribute_diff, attribute_diff.end()), attr_compare);
    return type == node.type && name == node.name && content == node.content && attribute_diff.empty();
}


bool node::operator==(const node & node) const {
    return is_equal(node);
}

bool node::operator!=(const node & node) const {
    return !operator==(node);
}

bool node::is_start() const {
    return type == node_type::START;
}

bool node::is_end() const {
    return type == node_type::END;
}

bool node::is_empty() const {
    return empty;
}

bool node::is_text() const {
    return type == node_type::TEXT;
}

bool node::is_whitespace() const {
    return is_text() && std::isspace((*content)[0]);
}

bool node::is_temporary() const {
    return temporary;
}

bool node::is_simple() const {
    return simple;
}

bool node::is_new_line() const {
    return is_text() && (*content)[0] == '\n';
}

bool node::is_open_tag() const {
    return type == node_type::START;
}

void node::clear_attributes() {
    attributes.clear();
}

void node::set_attributes(const attribute_map & input) {
    attributes = input;
}

void node::emplace_attribute(const std::string & name, const attribute & attr) {
    attributes.emplace(name, attr);
}

void node::set_type(node_type input) {
    type = input;
}

void node::set_empty(bool input) {
    empty = input;
}

void node::set_temporary(bool input) {
    temporary = input;
}

void node::set_parent(std::shared_ptr<node> input) {
    parent = input;
}

void node::set_simple(bool input) {
    simple = input;
}

void node::set_content(std::optional<std::string> input) {
    content = input;
}

void node::set_name(std::string input) {
    name = input;
}

void node::set_move(int input) {
    move = input;
}

const attribute_map & node::get_attributes() const {
    return attributes;
}

node_type node::get_type() const {
    return type;
}

const std::string & node::get_name() const {
    return name;
}

const std::optional<std::string> & node::get_content() const {
    return content;
}

int node::get_move() const {
    return move;
}

std::shared_ptr<node> node::get_parent() const {
    return parent;
}

std::shared_ptr<name_space> node::get_namespace() const {
    return ns;
}

std::ostream & operator<<(std::ostream & out, const node & node) {
    if(node.is_text()) {
        out << "text: '" << (node.get_content() ? *node.get_content() : "") << '\'';
    } else {
        out << '<';
        if(node.is_end()) {
            out << '/';
        }
    out << node.get_name();
    }

    for(attribute_map_cpair attribute_pair : node.get_attributes()) {
        out << ' ' << attribute_pair.first << '=' << *attribute_pair.second.get_value();
    }

    if(!node.is_text()) {
        out << '>';
    }

    return out;
}

}
