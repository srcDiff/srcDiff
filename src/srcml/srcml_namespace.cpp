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

#include <srcml_namespace.hpp>

#include <srcml.h>

srcml_namespace::srcml_namespace(const std::string & uri, const std::optional<std::string> & prefix) : uri(uri), prefix(prefix) {}

srcml_namespace::srcml_namespace(xmlNsPtr ns) 
  : uri(), prefix() {

    if(!ns) return;

    if(ns->href)   uri = std::string((const char *)ns->href);
    if(ns->prefix) prefix = std::string((const char *)ns->prefix);
}

srcml_namespace::srcml_namespace(const srcml_namespace & ns) 
  : uri(ns.uri), prefix(ns.prefix) {}

std::string srcml_namespace::get_uri() const {
  return uri;
}

std::optional<std::string> srcml_namespace::get_prefix() const {
  return prefix;
}

void srcml_namespace::set_uri(std::string input) {
  uri = input;
}

void srcml_namespace::set_prefix(std::optional<std::string> input) {
  prefix = input;
}

std::shared_ptr<srcml_namespace> srcml_namespace::get_namespace(xmlNsPtr ns) {

  static bool init_namespace = true;

  if(init_namespace) {
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcML/src", SRC_NAMESPACE));
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcML/cpp", CPP_NAMESPACE));
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcDiff",   DIFF_NAMESPACE));
      init_namespace = false;
  }

  if(!ns) return SRC_NAMESPACE;

  typedef std::unordered_map<std::string, std::shared_ptr<srcml_namespace>>::const_iterator namespaces_citr;
  namespaces_citr citr = namespaces.find((const char *)ns->href);
  if(citr != namespaces.end()) return citr->second;

  namespaces_citr added_citr = namespaces.emplace(std::make_pair((const char *)ns->href, std::make_shared<srcml_namespace>(ns))).first;
  return added_citr->second;
}
