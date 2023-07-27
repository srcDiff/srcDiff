#include <srcdiff_compare.hpp>

#include <srcdiff_diff.hpp>

#include <list>
#include <construct.hpp>

namespace srcdiff_compare {

  // diff node accessor function
  const void * element_index(int index, const void* data, const void * context) {
    construct_list & elements = *(construct_list *)data;
    return &elements[index];
  }

  // diff node accessor function
  const void * element_array_index(int index, const void* data, const void * context) {
    construct * element = (construct *)data;
    return &element[index];
  }

  const void * node_index(int index, const void* data, const void * context) {
    construct & element = *(construct *)data;
    return &element.get_terms()[index];
  }

  const void * node_array_index(int index, const void* data, const void * context) {
    int * element = (int *)data;
    return &element[index];
  }

  int node_index_compare(const void * node1, const void * node2, const void * context) {

    diff_nodes & dnodes = *(diff_nodes *)context;

    const std::shared_ptr<srcml_node> & node_original = dnodes.nodes_original.at(*(int *)node1);
    const std::shared_ptr<srcml_node> & node_modified = dnodes.nodes_modified.at(*(int *)node2);

    return node_compare(node_original, node_modified);
  }

  // diff node comparison function
  int node_compare(const std::shared_ptr<srcml_node> & node1, const std::shared_ptr<srcml_node> & node2) {

    if (node1 == node2)
      return 0;

    if(node1->type != node2->type)
      return 1;

    if(node1->name != node2->name)
      return 1;

    // end if text node contents differ
    if((xmlReaderTypes)node1->type == XML_READER_TYPE_TEXT)
      return node1->content == node2->content && (!node1->content || *node1->content == *node2->content) ? 0 : 1;

    if(node1->is_empty != node2->is_empty)
      return 1;

    if(node1->ns.prefix || node2->ns.prefix) {

      if(!node1->ns.prefix)
        return 1;
      else if(!node2->ns.prefix)
        return 1;
      else if(*node1->ns.prefix != *node2->ns.prefix) 
      return 1;

    }

    return node1->properties != node2->properties;
  }


  // diff node comparison function
  int element_syntax_compare(const void * e1, const void * e2, const void * context) {

    diff_nodes & dnodes = *(diff_nodes *)context;

    construct * element_1 = (construct *)e1;
    construct * element_2 = (construct *)e2;

    if(!element_1->hash()) {
      element_1->hash(std::hash<construct>()(*element_1));
    }

    if(!element_2->hash()) {
      element_2->hash(std::hash<construct>()(*element_2));
    }

    if(!(element_1->hash() == element_2->hash()))
      return 1;

    for(unsigned int i = 0, j = 0; i < element_1->size() && j < element_2->size();) {

      // string consecutive non whitespace text nodes
      // TODO:  Why create the string?  Just compare directly as you go through
      if(element_1->term(i)->is_text() && element_2->term(j)->is_text()) {

        std::string text1 = "";
        for(; i < element_1->size() && element_1->term(i)->is_text(); ++i) {
          text1 += element_1->term(i)->content ? *element_1->term(i)->content : "";
        }

        std::string text2 = "";
        for(; j < element_2->size() && element_2->term(j)->is_text(); ++j) {
          text2 += element_2->term(j)->content ? *element_2->term(j)->content : "";
        }

        if(text1 != text2)
          return 1;

      } else if(node_compare(element_1->term(i), element_2->term(j)))
        return 1;
      else {

        ++i;
        ++j;

      }
    }

    return 0;
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
