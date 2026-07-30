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

namespace srcML {

name_space::name_space(const std::string & uri, const std::optional<std::string> & prefix) : uri(uri), prefix(prefix) {}

name_space::name_space(xmlNsPtr ns) 
  : uri(), prefix() {

    if(!ns) return;

    if(ns->href)   uri = std::string((const char *)ns->href);
    if(ns->prefix) prefix = std::string((const char *)ns->prefix);
}

name_space::name_space(const name_space & ns) 
  : uri(ns.uri), prefix(ns.prefix) {}

std::string name_space::get_uri() const {
  return uri;
}

std::optional<std::string> name_space::get_prefix() const {
  return prefix;
}

void name_space::set_uri(std::string input) {
  uri = input;
}

void name_space::set_prefix(std::optional<std::string> input) {
  prefix = input;
}

std::string name_spaces::SRC_URI = "http://www.srcML.org/srcML/src";
std::string name_spaces::DIFF_URI = "http://www.srcML.org/srcDiff";

std::shared_ptr<name_space> name_spaces::SRC_NAMESPACE = std::make_shared<name_space>(SRC_URI);
name_spaces name_spaces::namespace_registry;

name_spaces::name_spaces() : namespaces() {
  namespaces.emplace(std::make_pair(SRC_URI,  SRC_NAMESPACE));
  namespaces.emplace(std::make_pair(DIFF_URI, std::make_shared<name_space>(DIFF_URI, "diff")));
}

void name_spaces::init(const srcml_archive* archive) {

}

std::shared_ptr<name_space> name_spaces::get_src_ns() {
  return namespaces.at(SRC_URI);
}

std::shared_ptr<name_space> name_spaces::get_diff_ns() {
  return namespaces.at(DIFF_URI);
}

std::shared_ptr<name_space> name_spaces::get_namespace(xmlNsPtr ns) {

  if(!ns) return get_src_ns();

  typedef std::unordered_map<std::string, std::shared_ptr<name_space>>::const_iterator namespaces_citr;
  namespaces_citr citr = namespaces.find((const char *)ns->href);
  if(citr != namespaces.end()) return citr->second;

  namespaces_citr added_citr = namespaces.emplace(std::make_pair((const char *)ns->href, std::make_shared<name_space>(ns))).first;
  return added_citr->second;
}

}
