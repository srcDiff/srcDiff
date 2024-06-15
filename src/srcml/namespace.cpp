// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file namespace.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <namespace.hpp>

#include <string>
#include <optional>
#include <memory>
#include <map>
#include <unordered_map>


#include <srcml.h>

std::shared_ptr<srcML::name_space> srcML::name_space::SRC_NAMESPACE = std::make_shared<srcML::name_space>("http://www.srcML.org/srcML/src");

std::shared_ptr<srcML::name_space> srcML::name_space::CPP_NAMESPACE = std::make_shared<srcML::name_space>("http://www.srcML.org/srcML/cpp", "cpp");

std::shared_ptr<srcML::name_space> srcML::name_space::DIFF_NAMESPACE = std::make_shared<srcML::name_space>("http://www.srcML.org/srcDiff", "diff");

std::unordered_map<std::string, std::shared_ptr<srcML::name_space>> srcML::name_space::namespaces = {};

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
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcML/src", srcML::name_space::SRC_NAMESPACE));
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcML/cpp", srcML::name_space::CPP_NAMESPACE));
      namespaces.emplace(std::make_pair("http://www.srcML.org/srcDiff",   srcML::name_space::DIFF_NAMESPACE));
      init_namespace = false;
  }

  if(!ns) return SRC_NAMESPACE;

  typedef std::unordered_map<std::string, std::shared_ptr<srcML::name_space>>::const_iterator namespaces_citr;
  namespaces_citr citr = namespaces.find((const char *)ns->href);
  if(citr != namespaces.end()) return citr->second;

  namespaces_citr added_citr = namespaces.emplace(std::make_pair((const char *)ns->href, std::make_shared<srcML::name_space>(ns))).first;
  return added_citr->second;
}
