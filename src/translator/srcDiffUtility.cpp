#include <srcDiffUtility.hpp>
#include <srcDiffTypes.hpp>
#include <string.h>
#include <vector>
#include <string>
#include <xmlrw.hpp>

bool is_change(edit * edit_script) {

  return edit_script->operation == SESDELETE && edit_script->next != NULL && edit_script->next->operation == SESINSERT
    && (edit_script->offset_sequence_one + edit_script->length) == edit_script->next->offset_sequence_one;

}

// diff node accessor function
const void * node_set_index(int idx, const void *s, const void * context) {
  NodeSets & node_sets = *(NodeSets *)s;
  return node_sets[idx];
}

const void * node_index(int idx, const void *s, const void * context) {
  NodeSet & node_set = *(NodeSet *)s;
  return &node_set[idx];
}

int node_index_compare(const void * node1, const void * node2, const void * context) {

  diff_nodes & dnodes = *(diff_nodes *)context;

  xNodePtr node_old = dnodes.nodes_old.at(*(int *)node1);
  xNodePtr node_new = dnodes.nodes_new.at(*(int *)node2);

  return node_compare(node_old, node_new);
}


bool attribute_compare(xAttr * attr1, xAttr * attr2) {

  xAttr * attr_old = attr1;
  xAttr * attr_new = attr2;

  for(; attr_old && attr_new
        && strcmp((const char *)attr_old->name, (const char *)attr_new->name) == 0
        && strcmp((const char *)attr_old->value, (const char *)attr_new->value) == 0;
      attr_old = attr_old->next, attr_new = attr_new->next)
    ;

  if(attr_old || attr_new)
    return 1;

  return 0;

}

// diff node comparison function
int node_compare(xNode * node1, xNode * node2) {

  if (node1 == node2)
    return 0;

  if(node1->type != node2->type || strcmp((const char *)node1->name, (const char *)node2->name) != 0)
    return 1;

  // end if text node contents differ
  if((xmlReaderTypes)node1->type == XML_READER_TYPE_TEXT)
    return strcmp((const char *)node1->content, (const char *)node2->content);

  if(node1->is_empty != node2->is_empty)
    return 1;

  if(!(node1->ns->prefix == 0 && node2->ns->prefix == 0)) {

    if(node1->ns->prefix == 0)
      return 1;
    else if(node2->ns->prefix == 0)
      return 1;
    else if(strcmp((const char *)node1->ns->prefix, (const char *)node2->ns->prefix) != 0)
    return 1;

  }

  return attribute_compare(node1->properties, node2->properties);
}

bool is_white_space(xNodePtr node) {

  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]);

}

bool is_new_line(xNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content[0] == '\n';

}

bool is_text(xNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT;
}

// diff node comparison function
int node_set_syntax_compare(const void * e1, const void * e2, const void * context) {

  diff_nodes & dnodes = *(diff_nodes *)context;

  NodeSet * node_set1 = (NodeSet *)e1;
  NodeSet * node_set2 = (NodeSet *)e2;

  for(unsigned int i = 0, j = 0; i < node_set1->size() && j < node_set2->size();) {

    // string consecutive non whitespace text nodes
    // TODO:  Why create the string?  Just compare directly as you go through
    if(is_text(dnodes.nodes_old.at(node_set1->at(i))) && is_text(dnodes.nodes_new.at(node_set2->at(j)))) {

      std::string text1 = "";
      for(; i < node_set1->size() && is_text(dnodes.nodes_old.at(node_set1->at(i))); ++i)
        text1 += (const char *)dnodes.nodes_old.at(node_set1->at(i))->content;

      std::string text2 = "";
      for(; j < node_set2->size() && is_text(dnodes.nodes_new.at(node_set2->at(j))); ++j)
        text2 += (const char *)dnodes.nodes_new.at(node_set2->at(j))->content;

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


