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

    if (node_one == node_two)
      return 0;

    if(node_one->type != node_two->type)
      return 1;

    if(node_one->name != node_two->name)
      return 1;

    // end if text node contents differ
    if((xmlReaderTypes)node_one->type == XML_READER_TYPE_TEXT)
      return node_one->content == node_two->content && (!node_one->content || *node_one->content == *node_two->content) ? 0 : 1;

    if(node_one->is_empty != node_two->is_empty)
      return 1;

    if(node_one->ns.prefix || node_two->ns.prefix) {

      if(!node_one->ns.prefix)
        return 1;
      else if(!node_two->ns.prefix)
        return 1;
      else if(*node_one->ns.prefix != *node_two->ns.prefix) 
      return 1;

    }

    return node_one->properties != node_two->properties;
  }


  const void * construct_list_index(int index, const void* data, const void * context) {

    construct::construct_list & elements = *(construct::construct_list *)data;
    return &elements[index];
  }

  int construct_compare(const void * e1, const void * e2, const void * context) {

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
