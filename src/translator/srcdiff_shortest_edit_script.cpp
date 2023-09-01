#include <srcdiff_shortest_edit_script.hpp>

int srcdiff_shortest_edit_script::compute_edit_script(const std::shared_ptr<construct> & original, const std::shared_ptr<construct> & modified) {
  compare = node_compare;
  accessor = construct_node_index;

  return compute((const void *)&original, original->size(), (const void *)&modified, modified->size());
}

int srcdiff_shortest_edit_script::compute_edit_script(const construct::construct_list & original, const construct::construct_list & modified) {
  compare = construct_compare;
  accessor = construct_list_index;

  return compute((const void *)&original, original.size(), (const void *)&modified, modified.size());
}

int srcdiff_shortest_edit_script::compute_edit_script(const std::string & original, const std::string & modified) {
  compare = char_compare;
  accessor = char_index;

  return compute((const void *)&original, original.size(), (const void *)&modified, modified.size());
}

int srcdiff_shortest_edit_script::compute_edit_script(const std::vector<std::string> & original, const std::vector<std::string> & modified) {
  compare = string_compare;
  accessor = string_index;

  return compute((const void *)&original, original.size(), (const void *)&modified, modified.size());
}

/** Internal comparison functions **/

// diff node accessor function
const void * srcdiff_shortest_edit_script::construct_node_index(int index, const void* data, const void * context) {
  const std::shared_ptr<construct>& element = *(const std::shared_ptr<construct> *)data;
  return &element->term(index);
}

int srcdiff_shortest_edit_script::node_compare(const void * node_one, const void * node_two, const void * context) {
  return node_compare(*(const std::shared_ptr<srcml_node> *)node_one, *(const std::shared_ptr<srcml_node>*)node_two);
}

// srcdiff_shortest_edit_script::diff node comparison function
int srcdiff_shortest_edit_script::node_compare(const std::shared_ptr<srcml_node> & node_one, const std::shared_ptr<srcml_node> & node_two) {
  if(*node_one == *node_two) return 0;
  return 1;
}


const void * srcdiff_shortest_edit_script::construct_list_index(int index, const void* data, const void * context) {

  construct::construct_list & elements = *(construct::construct_list *)data;
  return &elements[index];
}

int srcdiff_shortest_edit_script::construct_compare(const void * e1, const void * e2, const void * context) {

  const std::shared_ptr<construct> & element_1 = *(const std::shared_ptr<construct> *)e1;
  const std::shared_ptr<construct> & element_2 = *(const std::shared_ptr<construct> *)e2;
  
  if(*element_1 == *element_2) return 0;
  return 1;
}

int srcdiff_shortest_edit_script::char_compare(const void * c1, const void * c2, const void * context) {

  char ch1 = *(char *)c1;
  char ch2 = *(char *)c2;

  return ch1 != ch2;

}

const void * srcdiff_shortest_edit_script::char_index(int index, const void * s, const void * context) {

  const std::string & str = *(const std::string *)s;

  return &str[index];

}

int srcdiff_shortest_edit_script::string_compare(const void * s1, const void * s2, const void * context) {

  const std::string & string1 = *(const std::string *)s1;
  const std::string & string2 = *(const std::string *)s2;

  return string1 != string2;

}

const void * srcdiff_shortest_edit_script::string_index(int index, const void * s, const void * context) {

  const std::vector<std::string> & string_list = *(const std::vector<std::string> *)s;

  return &string_list[index];

}

