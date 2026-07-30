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

namespace srcML {

attribute::attribute(xmlAttrPtr attribute)
    : name((const char *)attribute->name),
      value(attribute->children && attribute->children->content ? std::string((const char *)attribute->children->content) : std::optional<std::string>()),
      ns(name_spaces::namespace_registry.get_namespace(attribute->ns)) {}

attribute::attribute(const std::string& name, std::shared_ptr<name_space> ns, const std::optional<std::string>& value)
    : name(name), value(value), ns(ns) {}

void attribute::merge(const attribute& that) {
    assert(this->get_name() == that.get_name());
    assert(this->get_ns() == that.get_ns());
    if(*this->get_value() != *that.get_value()) {
        this->set_value(std::optional<std::string>(*this->get_value() + "|" + *that.get_value()));
    }
}

void attribute::set_value(const std::optional<std::string>& input) {
    value = input;
}

std::string attribute::full_name() const {
    if(ns && ns->get_prefix()) return *ns->get_prefix() + ":" + name;
    return name;
}

std::ostream& operator<<(std::ostream& out, const attribute& that) {
    out << that.full_name();
    if(that.get_value().has_value()) out << "=" << that.get_value().value();
    return out;
}

bool attribute::operator==(const attribute& that) const {
    return ns == that.ns && name == that.name && value == that.value;
}

bool attribute::operator!=(const attribute& that) const {
    return !this->operator==(that);
}

const std::string& attribute::get_name() const {
    return name;
}

std::shared_ptr<name_space> attribute::get_ns() const {
    return ns;
}

const std::optional<std::string>& attribute::get_value() const {
    return value;
}

}
