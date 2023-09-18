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

#include <namespace.hpp>

#include <srcml.h>

srcML::name_space::name_space(const std::string & uri, const std::optional<std::string> & prefix) : uri(uri), prefix(prefix) {}

srcML::name_space::name_space(xmlNsPtr ns) 
  : uri(), prefix() {

    if(!ns) return;

    if(ns->href)   uri = std::string((const char *)ns->href);
    if(ns->prefix) prefix = std::string((const char *)ns->prefix);
}

srcML::name_space::name_space(const name_space & ns) 
  : uri(ns.uri), prefix(ns.prefix) {}

std::string srcML::name_space::get_uri() const {
  return uri;
}

std::optional<std::string> srcML::name_space::get_prefix() const {
  return prefix;
}

void srcML::name_space::set_uri(std::string input) {
  uri = input;
}

void srcML::name_space::set_prefix(std::optional<std::string> input) {
  prefix = input;
}

std::shared_ptr<srcML::name_space> srcML::name_space::get_namespace(xmlNsPtr ns) {

  static bool init_namespace = true;

  if(init_namespace) {
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcML/src", srcML::SRC_NAMESPACE));
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcML/cpp", srcML::CPP_NAMESPACE));
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcDiff",   srcML::DIFF_NAMESPACE));
      init_namespace = false;
  }

  if(!ns) return SRC_NAMESPACE;

  typedef std::unordered_map<std::string, std::shared_ptr<srcML::name_space>>::const_iterator namespaces_citr;
  namespaces_citr citr = namespaces.find((const char *)ns->href);
  if(citr != namespaces.end()) return citr->second;

  namespaces_citr added_citr = namespaces.emplace(std::make_pair((const char *)ns->href, std::make_shared<srcML::name_space>(ns))).first;
  return added_citr->second;
}
