// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script.cpp
 *
 * @copyright Copyright (C) 2023-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <shortest_edit_script_utility.hpp>

#include <shortest_edit_script_t.hpp>

namespace srcdiff {

// all 8 of the const void * context parameters in this file are unused as of yet
ses::edit_list shortest_edit_script_utility::compute(const std::shared_ptr<construct>& original, const std::shared_ptr<construct>& modified) {

  ses::shortest_edit_script edit_script_computer(node_compare, construct_node_index, nullptr);
  return edit_script_computer.compute((const void *)&original, original->size(), (const void *)&modified, modified->size());
}

ses::edit_list shortest_edit_script_utility::compute(const construct::construct_list_view original, const construct::construct_list_view modified) {

  ses::shortest_edit_script edit_script_computer(construct_compare, construct_list_index, nullptr);
  return edit_script_computer.compute((const void *)&original, original.size(), (const void *)&modified, modified.size());
}

ses::edit_list shortest_edit_script_utility::compute(const std::vector<std::string> & original, const std::vector<std::string> & modified) {

  ses::shortest_edit_script edit_script_computer(string_compare, string_index, nullptr);
  return edit_script_computer.compute((const void *)&original, original.size(), (const void *)&modified, modified.size());
}

ses::edit_list shortest_edit_script_utility::compute(const std::string& original, const std::string& modified) {

  ses::shortest_edit_script edit_script_computer(char_compare, char_index, nullptr);
  return edit_script_computer.compute((const void *)&original, original.size(), (const void *)&modified, modified.size());
}


/** Internal comparison functions **/

// diff node accessor function
const void * shortest_edit_script_utility::construct_node_index(int index, const void* data, const void* context [[maybe_unused]]) {
  const std::shared_ptr<construct>& element = *(const std::shared_ptr<construct> *)data;
  return &element->term(index);
}

int shortest_edit_script_utility::node_compare(const void* node_one, const void* node_two, const void* context [[maybe_unused]]) {

  const std::shared_ptr<srcML::node> original_node = *(const std::shared_ptr<srcML::node>*)node_one;
  const std::shared_ptr<srcML::node> modified_node = *(const std::shared_ptr<srcML::node>*)node_two;

  if(*original_node == *modified_node) return 0;
  return 1;
}

const void* shortest_edit_script_utility::construct_list_index(int index, const void* data, const void* context [[maybe_unused]]) {

  construct::construct_list_view elements = *(construct::construct_list_view *)data;
  return &elements[index];
}

int shortest_edit_script_utility::construct_compare(const void* e1, const void* e2, const void* context [[maybe_unused]]) {

  const std::shared_ptr<construct>& element_1 = *(const std::shared_ptr<construct>*)e1;
  const std::shared_ptr<construct>& element_2 = *(const std::shared_ptr<construct>*)e2;
  
  if(*element_1 == *element_2) return 0;
  return 1;
}

int shortest_edit_script_utility::string_compare(const void * s1, const void * s2, const void * context [[maybe_unused]]) {

  const std::string& string1 = *(const std::string*)s1;
  const std::string& string2 = *(const std::string*)s2;

  return string1 != string2;
}

const void * shortest_edit_script_utility::string_index(int index, const void * s, const void * context [[maybe_unused]]) {

  const std::vector<std::string>& string_list = *(const std::vector<std::string>*)s;
  return &string_list[index];
}

int shortest_edit_script_utility::char_compare(const void* c1, const void* c2, const void* context [[maybe_unused]]) {

  char ch1 = *(char*)c1;
  char ch2 = *(char*)c2;
  return ch1 != ch2;
}

const void* shortest_edit_script_utility::char_index(int index, const void* s, const void* context [[maybe_unused]]) {
  const std::string& str = *(const std::string*)s;
  return &str[index];
}

}
