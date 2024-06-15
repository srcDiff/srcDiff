// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file node.cpp
 *
 * @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
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

srcML::node_type xml_type2srcml_type(xmlElementType type) {
    static std::unordered_map<unsigned int, srcML::node_type> type_map = {

        { XML_READER_TYPE_ELEMENT, srcML::node_type::START },
        { XML_READER_TYPE_END_ELEMENT, srcML::node_type::END },
        { XML_READER_TYPE_TEXT, srcML::node_type::TEXT },
        { XML_READER_TYPE_SIGNIFICANT_WHITESPACE, srcML::node_type::WS },

    };

    try {
        return type_map.at((unsigned int)type);
    } catch(const std::out_of_range & error) {
        return srcML::node_type::OTHER;
    }

}

srcML::node::node()
    : type(srcML::node_type::OTHER), name(), ns(srcML::name_space::SRC_NAMESPACE), content(),
      ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(true), move(0), user_data(), extra(0) {}

srcML::node::node(const xmlNode & node, xmlElementType xml_type) 
    : type(xml_type2srcml_type(xml_type)), name(), ns(), content(),
      ns_definition(), parent(), attributes(), temporary(false), empty(node.extra), simple(true), move(0), user_data(), extra(node.extra) {

    name = std::string((const char *)node.name);

    if(node.content)
        content = std::string((const char *)node.content);

    ns = srcML::name_space::get_namespace(node.ns);

    if(type != srcML::node_type::START) return;

    xmlNsPtr node_ns = node.nsDef;
    while(node_ns) {
        ns_definition.emplace_back(srcML::name_space::get_namespace(node_ns));
        node_ns = node_ns->next;
    }

    xmlAttrPtr attribute = node.properties;
    while (attribute) {
        srcML::attribute new_attribute = srcML::attribute(attribute);
        attributes.emplace(std::make_pair(new_attribute.full_name(), new_attribute));
        attribute = attribute->next;
    }

}

srcML::node::node(const std::string & text) : type(srcML::node_type::TEXT), name("text"), 
    ns(srcML::name_space::SRC_NAMESPACE), content(text), ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(true), move(0), user_data(), extra(0) {}

srcML::node::node(const srcML::node & node) : type(node.type), name(node.name), ns(node.ns),
    content(node.content), ns_definition(node.ns_definition), parent(node.parent), attributes(node.attributes), temporary(node.temporary), empty(node.empty), simple(node.simple), 
    move(node.move), user_data(node.user_data) {}

srcML::node::node(node_type type, const std::string & name, const std::shared_ptr<srcML::name_space> & ns) 
    : type(type), name(name), ns(ns), content(), ns_definition(), parent(), attributes(), temporary(false), empty(false), simple(false), move(0), user_data(), 
      extra(0) {}

srcML::node::~node() {}

std::string srcML::node::full_name() const {

    if(ns->get_prefix()) return *ns->get_prefix() + ":" + name;

    return name;
} 

const srcML::attribute * srcML::node::get_attribute(const std::string & attribute) const {

    srcML::attribute_map_citr attribute_itr = attributes.find(attribute);
    if(attribute_itr == attributes.end()) return nullptr;
    return &attribute_itr->second;

}

srcML::attribute * srcML::node::get_attribute(const std::string & attribute) {

    srcML::attribute_map_itr attribute_itr = attributes.find(attribute);
    if(attribute_itr == attributes.end()) return nullptr;
    return &attribute_itr->second;

}

const std::optional<std::string> & srcML::node::get_attribute_value(const std::string & attribute) const {

    srcML::attribute_map_citr attribute_itr = attributes.find(attribute);
    assert(attribute_itr != attributes.end());

    return attribute_itr->second.get_value();

}

void srcML::node::merge(const srcML::node & that) {
    assert(this->get_name() == that.get_name());
    assert(this->get_namespace() == that.get_namespace());
    this->set_empty(this->is_empty() && that.is_empty());
    merge_attributes(that.get_attributes());
}

void srcML::node::merge_attributes(const srcML::attribute_map & that) {

    srcML::attribute_map same_attributes;
    srcML::attribute_map original_attributes;
    srcML::attribute_map modified_attributes;

    auto key_compare = [](
        const srcML::attribute_map_pair & a,
        const srcML::attribute_map_pair & b
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

bool srcML::node::operator==(const srcML::node & node) const {
    return type == node.type && name == node.name && content == node.content && attributes == node.attributes;
}

bool srcML::node::operator!=(const srcML::node & node) const {
    return !operator==(node);
}

bool srcML::node::is_start() const {
    return type == srcML::node_type::START;
}

bool srcML::node::is_end() const {
    return type == srcML::node_type::END;
}

bool srcML::node::is_empty() const {
    return empty;
}

bool srcML::node::is_text() const {
    return type == srcML::node_type::TEXT;
}

bool srcML::node::is_whitespace() const {
    return is_text() && std::isspace((*content)[0]);
}

bool srcML::node::is_temporary() const {
    return temporary;
}

bool srcML::node::is_simple() const {
    return simple;
}

bool srcML::node::is_new_line() const {
    return is_text() && (*content)[0] == '\n';
}

bool srcML::node::is_open_tag() const {
    return type == srcML::node_type::START;
}

void srcML::node::clear_attributes() {
    attributes.clear();
}

void srcML::node::set_attributes(const srcML::attribute_map & input) {
    attributes = input;
}

void srcML::node::emplace_attribute(const std::string & name, const srcML::attribute & attr) {
    attributes.emplace(name, attr);
}

void srcML::node::set_type(srcML::node_type input) {
    type = input;
}

void srcML::node::set_empty(bool input) {
    empty = input;
}

void srcML::node::set_temporary(bool input) {
    temporary = input;
}

void srcML::node::set_parent(std::shared_ptr<srcML::node> input) {
    parent = input;
}

void srcML::node::set_simple(bool input) {
    simple = input;
}

void srcML::node::set_content(std::optional<std::string> input) {
    content = input;
}

void srcML::node::set_name(std::string input) {
    name = input;
}

void srcML::node::set_move(int input) {
    move = input;
}

const srcML::attribute_map & srcML::node::get_attributes() const {
    return attributes;
}

srcML::node_type srcML::node::get_type() const {
    return type;
}

const std::string & srcML::node::get_name() const {
    return name;
}

const std::optional<std::string> & srcML::node::get_content() const {
    return content;
}

int srcML::node::get_move() const {
    return move;
}

std::shared_ptr<srcML::node> srcML::node::get_parent() const {
    return parent;
}

std::shared_ptr<srcML::name_space> srcML::node::get_namespace() const {
    return ns;
}

std::ostream & srcML::operator<<(std::ostream & out, const srcML::node & node) {
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
