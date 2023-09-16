/**
 * @file srcml_namespace.cpp
 *
 * @copyright Copyright (C) 2023-2023 srcML, LLC. (www.srcML.org)
 *
 * srcDiff is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * srcDiff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <srcml_attribute.hpp>

srcml_attribute::srcml_attribute(xmlAttrPtr attribute)
  : name((const char *)attribute->name),
    value(attribute->children && attribute->children->content ? 
          std::string((const char *)attribute->children->content) : std::optional<std::string>()),
    ns(srcml_namespace::get_namespace(attribute->ns)) {}

srcml_attribute::srcml_attribute(
    const std::string & name,
    std::shared_ptr<srcml_namespace> ns,
    std::optional<std::string> value) : name(name), ns(ns), value(value) {}

std::string srcml_attribute::full_name() const {
  if(ns && ns->get_prefix()) return *ns->get_prefix() + ":" + name;
  return name;
}

std::ostream & operator<<(std::ostream & out, const srcml_attribute & that) {
  out << that.full_name();
  if(that.value) out << "=" << *that.value;
  return out;
}

bool srcml_attribute::operator==(const srcml_attribute & that) const {
  return ns == that.ns && name == that.name && value == that.value;
}

bool srcml_attribute::operator!=(const srcml_attribute & that) const {
  return !this->operator==(that);
}

const std::string & srcml_attribute::get_name() const {
  return name;
}

std::shared_ptr<srcml_namespace> srcml_attribute::get_ns() const {
  return ns;
}

const std::optional<std::string> & srcml_attribute::get_value() const {
  return value;
}
