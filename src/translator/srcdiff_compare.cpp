#include <srcdiff_compare.hpp>

#include <srcdiff_diff.hpp>

#include <list>

namespace srcdiff_compare {

  // diff node accessor function
  const void * node_set_index(int idx, const void *s, const void * context) {
    node_sets & sets = *(node_sets *)s;
    return &sets[idx];
  }

  const void * node_index(int idx, const void *s, const void * context) {
    node_set & set = *(node_set *)s;
    return &set[idx];
  }

  int node_index_compare(const void * node1, const void * node2, const void * context) {

    diff_nodes & dnodes = *(diff_nodes *)context;

    const std::shared_ptr<srcml_node> & node_old = dnodes.nodes_old.at(*(int *)node1);
    const std::shared_ptr<srcml_node> & node_new = dnodes.nodes_new.at(*(int *)node2);

    return node_compare(node_old, node_new);
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

    if(node1->ns->prefix || node2->ns->prefix) {

      if(!node1->ns->prefix)
        return 1;
      else if(!node2->ns->prefix)
        return 1;
      else if(*node1->ns->prefix != *node2->ns->prefix) 
      return 1;

    }

    return node1->properties != node2->properties;
  }


  // diff node comparison function
  int node_set_syntax_compare(const void * e1, const void * e2, const void * context) {

    diff_nodes & dnodes = *(diff_nodes *)context;

    node_set * node_set1 = (node_set *)e1;
    node_set * node_set2 = (node_set *)e2;

    for(unsigned int i = 0, j = 0; i < node_set1->size() && j < node_set2->size();) {

      // string consecutive non whitespace text nodes
      // TODO:  Why create the string?  Just compare directly as you go through
      if(dnodes.nodes_old.at(node_set1->at(i))->is_text() && dnodes.nodes_new.at(node_set2->at(j))->is_text()) {

        std::string text1 = "";
        for(; i < node_set1->size() && dnodes.nodes_old.at(node_set1->at(i))->is_text(); ++i)
          text1 += dnodes.nodes_old.at(node_set1->at(i))->content ? *dnodes.nodes_old.at(node_set1->at(i))->content : "";

        std::string text2 = "";
        for(; j < node_set2->size() && dnodes.nodes_new.at(node_set2->at(j))->is_text(); ++j)
          text2 += dnodes.nodes_new.at(node_set2->at(j))->content ? *dnodes.nodes_new.at(node_set2->at(j))->content : "";

        if(text1 != text2)
          return 1;

      } else if(node_compare(dnodes.nodes_old.at(node_set1->at(i)), dnodes.nodes_new.at(node_set2->at(j))))
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

    return strcmp(string1.c_str(), string2.c_str());

  }

  const void * string_index(int idx, const void * s, const void * context) {

    std::vector<std::string> & string_list = *(std::vector<std::string> *)s;

    return &string_list[idx];

  }

}
