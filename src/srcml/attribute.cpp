// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file attribute.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <attribute.hpp>
#include <cassert>
#include <optional>
#include <string>

srcML::attribute::attribute(xmlAttrPtr attribute)
    : name((const char *)attribute->name),
      value(attribute->children && attribute->children->content ? std::string((const char *)attribute->children->content) : std::optional<std::string>()),
      ns(srcML::name_space::get_namespace(attribute->ns)) {}

srcML::attribute::attribute(
    const std::string & name,
    std::shared_ptr<srcML::name_space> ns,
    std::optional<std::string> value) : name(name), value(value), ns(ns) {}

void srcML::attribute::merge(const srcML::attribute & that) {
    assert(this->get_name() == that.get_name());
    assert(this->get_ns() == that.get_ns());
    this->set_value(std::optional<std::string>(*this->get_value() + "|" + *that.get_value()));
}

void srcML::attribute::set_value(const std::optional<std::string> & input) {
    value = input;
}

std::string srcML::attribute::full_name() const {
    if(ns && ns->get_prefix()) return *ns->get_prefix() + ":" + name;
    return name;
}

std::ostream & srcML::operator<<(std::ostream & out, const srcML::attribute & that) {
    out << that.full_name();
    if(that.get_value().has_value()) out << "=" << that.get_value().value();
    return out;
}

bool srcML::attribute::operator==(const attribute & that) const {
    return ns == that.ns && name == that.name && value == that.value;
}

bool srcML::attribute::operator!=(const attribute & that) const {
    return !this->operator==(that);
}

const std::string & srcML::attribute::get_name() const {
    return name;
}

std::shared_ptr<srcML::name_space> srcML::attribute::get_ns() const {
    return ns;
}

const std::optional<std::string> & srcML::attribute::get_value() const {
    return value;
}
