#include <srcdiff_compare.hpp>

#include <srcdiff_diff.hpp>

#include <list>
#include <construct.hpp>

namespace srcdiff_compare {

  // diff node accessor function
  const void * construct_node_index(int index, const void* data, const void * context) {
    construct * element = (construct *)data;
    return &element->term(index);
  }

  int node_compare(const void * node_one, const void * node_two, const void * context) {
    return node_compare(*(const std::shared_ptr<srcml_node> *)node_one, *(const std::shared_ptr<srcml_node>*)node_two);
  }

  // diff node comparison function
  int node_compare(const std::shared_ptr<srcml_node> & node_one, const std::shared_ptr<srcml_node> & node_two) {
    if(*node_one == *node_two) return 0;
    return 1;
  }


  const void * construct_list_index(int index, const void* data, const void * context) {

    construct::construct_list & elements = *(construct::construct_list *)data;
    return &elements[index];
  }

  int construct_compare(const void * e1, const void * e2, const void * context) {

    const construct & element_1 = *(const construct *)e1;
    const construct & element_2 = *(const construct *)e2;
    
    if(element_1 == element_2) return 0;
    return 1;
  }

  int string_compare(const void * s1, const void * s2, const void * context) {

    std::string & string1 = *(std::string *)s1;
    std::string & string2 = *(std::string *)s2;

    return string1 != string2;

  }

  const void * string_index(int index, const void * s, const void * context) {

    std::vector<std::string> & string_list = *(std::vector<std::string> *)s;

    return &string_list[index];

  }

}
