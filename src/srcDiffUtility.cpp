#include "srcDiffUtility.hpp"
#include <string.h>
#include <vector>
#include <string>

// global structures defined in main
extern std::vector<xmlNode *> nodes_old;
extern std::vector<xmlNode *> nodes_new;

bool is_change(struct edit * edit_script) {

  return edit_script->operation == DELETE && edit_script->next != NULL && edit_script->next->operation == INSERT
    && (edit_script->offset_sequence_one + edit_script->length - 1) == edit_script->next->offset_sequence_one;

}

// diff node accessor function
const void * node_set_index(int idx, const void *s) {
  std::vector<std::vector<int> *> & node_sets = *(std::vector<std::vector<int> *> *)s;
  return node_sets[idx];
}

bool attribute_compare(xmlAttrPtr attr1, xmlAttrPtr attr2) {

  xmlAttrPtr attr_old = attr1;
  xmlAttrPtr attr_new = attr2;

  for(; attr_old && attr_new
        && strcmp((const char *)attr_old->name, (const char *)attr_new->name) == 0
        && strcmp((const char *)attr_old->children->content, (const char *)attr_new->children->content) == 0;
      attr_old = attr_old->next, attr_new = attr_new->next)
    ;

  if(attr_old || attr_new)
    return 1;

  return 0;

}

// diff node comparison function
int node_compare(xmlNode * node1, xmlNode * node2) {

  if(node1->type != node2->type || strcmp((const char *)node1->name, (const char *)node2->name) != 0)
    return 1;

  // end if text node contents differ
  if((xmlReaderTypes)node1->type == XML_READER_TYPE_TEXT)
    return strcmp((const char *)node1->content, (const char *)node2->content);

  return attribute_compare(node1->properties, node2->properties);
}

bool is_white_space(xmlNodePtr node) {

  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]);

}

bool is_new_line(xmlNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content[0] == '\n';

}

bool is_text(xmlNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT;
}

// diff node comparison function
int node_set_syntax_compare(const void * e1, const void * e2) {
  std::vector<int> * node_set1 = (std::vector<int> *)e1;
  std::vector<int> * node_set2 = (std::vector<int> *)e2;

  for(unsigned int i = 0, j = 0; i < node_set1->size() && j < node_set2->size();) {

    // string consecutive non whitespace text nodes
    // TODO:  Why create the string?  Just compare directly as you go through
    if(is_text(nodes_old.at(node_set1->at(i))) && is_text(nodes_new.at(node_set2->at(j)))) {

      std::string text1 = "";
      for(; i < node_set1->size() && is_text(nodes_old.at(node_set1->at(i))); ++i)
        text1 += (const char *)nodes_old.at(node_set1->at(i))->content;

      std::string text2 = "";
      for(; j < node_set2->size() && is_text(nodes_new.at(node_set2->at(j))); ++j)
        text2 += (const char *)nodes_new.at(node_set2->at(j))->content;

      if(text1 != text2)
        return 1;

    } else if(node_compare(nodes_old.at(node_set1->at(i)), nodes_new.at(node_set2->at(j))))
      return 1;
    else {

      ++i;
      ++j;

    }
  }

  return 0;
}


