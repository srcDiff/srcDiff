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

bool is_single_call_expr(std::vector<xNodePtr> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (strcmp((const char *)nodes.at(start_pos)->name, "expr_stmt") != 0 && strcmp((const char *)nodes.at(start_pos)->name, "expr") != 0)) return false;

  if(nodes.at(start_pos)->extra & 0x1) return false;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && strcmp((const char *)nodes.at(start_pos)->name, "expr_stmt") == 0)
    ++start_pos;

  if(nodes.at(start_pos)->extra & 0x1) return false;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && strcmp((const char *)nodes.at(start_pos)->name, "expr") == 0)
    ++start_pos;

  if(strcmp((const char *)nodes.at(start_pos)->name, "call") != 0) return false;

  int open_call_count = (nodes.at(start_pos)->extra & 0x1) ? 0 : 1;
  ++start_pos;

  while(open_call_count) {

    if(strcmp((const char *)nodes.at(start_pos)->name, "call") == 0) {

      if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
        ++open_call_count;
      else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_call_count;

    }

    ++start_pos;

  }

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && strcmp((const char *)nodes.at(start_pos)->name, "expr") == 0)
    return true;

  return false;
 
}

std::string get_name(std::vector<xNodePtr> & nodes, int name_start_pos) {

  int open_name_count = nodes.at(name_start_pos)->extra & 0x1 ? 0 : 1;
  int name_pos = name_start_pos + 1;
  std::string name = "";

  while(open_name_count) {

    if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && strcmp((const char *)nodes.at(name_pos)->name, "argument_list") == 0) return name;

    if(strcmp((const char *)nodes.at(name_pos)->name, "name") == 0) {

      if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(name_pos)->extra & 0x1) == 0)
        ++open_name_count;
      else if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_name_count;

    } else if(is_text(nodes.at(name_pos)) && !is_white_space(nodes.at(name_pos))) {

      name += (const char *)nodes.at(name_pos)->content;

    }

    ++name_pos;

  }

  return name;

}

void skip_type(std::vector<xNodePtr> & nodes, int & start_pos) {

  while(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || strcmp((const char *)nodes.at(start_pos)->name, "type") != 0)
    ++start_pos;

  int open_type_count = nodes.at(start_pos)->extra & 0x1 ? 0 : 1;
  ++start_pos;

  while(open_type_count) {

    if(strcmp((const char *)nodes.at(start_pos)->name, "type") == 0) {

      if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
        ++open_type_count;
      else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_type_count;

    }

    ++start_pos;

  }

}

void skip_specifiers(std::vector<xNodePtr> & nodes, int & start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || strcmp((const char *)nodes.at(start_pos)->name, "specifier") != 0)
      return;

  while(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
    && strcmp((const char *)nodes.at(start_pos)->name, "specifier") == 0) {

    int open_specifier_count = nodes.at(start_pos)->extra & 0x1 ? 0 : 1;
    ++start_pos;

    while(open_specifier_count) {

      if(strcmp((const char *)nodes.at(start_pos)->name, "specifier") == 0) {

        if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
          ++open_specifier_count;
        else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
          --open_specifier_count;

      }

      ++start_pos;

    }

  }

}

std::string get_call_name(std::vector<xNodePtr> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || strcmp((const char *)nodes.at(start_pos)->name, "call") != 0) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  int name_start_pos = start_pos + 1;

  while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || (strcmp((const char *)nodes.at(name_start_pos)->name, "name") != 0 && strcmp((const char *)nodes.at(name_start_pos)->name, "argument_list") != 0))
    ++name_start_pos;

  if(strcmp((const char *)nodes.at(name_start_pos)->name, "argument_list") != 0) return "";

  return get_name(nodes, name_start_pos);

}

std::string get_decl_name(std::vector<xNodePtr> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (strcmp((const char *)nodes.at(start_pos)->name, "decl_stmt") != 0 && strcmp((const char *)nodes.at(start_pos)->name, "decl") != 0)) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  int name_start_pos = start_pos + 1;

  skip_type(nodes, name_start_pos);

  while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || strcmp((const char *)nodes.at(name_start_pos)->name, "name") != 0)
    ++name_start_pos;


  return get_name(nodes, name_start_pos);

}

std::string get_condition(std::vector<xNodePtr> & nodes, int start_pos) {

  int condition_start_pos = start_pos;

  while(nodes.at(condition_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || strcmp((const char *)nodes.at(condition_start_pos)->name, "condition") != 0)
    ++condition_start_pos;

  std::string condition = "";
  int open_condition_count = nodes.at(condition_start_pos)->extra & 0x1 ? 0 : 1;
  int condition_pos = condition_start_pos + 1;

  while(open_condition_count) {

    if(strcmp((const char *)nodes.at(condition_pos)->name, "condition") == 0) {

      if(nodes.at(condition_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(condition_pos)->extra & 0x1) == 0)
        ++open_condition_count;
      else if(nodes.at(condition_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_condition_count;

    } else if(is_text(nodes.at(condition_pos)) && !is_white_space(nodes.at(condition_pos))) {

      condition += (const char *)nodes.at(condition_pos)->content;

    }

    ++condition_pos;

  }

  return condition;

}

std::string get_function_type_name(std::vector<xNodePtr> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (strcmp((const char *)nodes.at(start_pos)->name, "function") != 0 && strcmp((const char *)nodes.at(start_pos)->name, "function_decl") != 0
      && strcmp((const char *)nodes.at(start_pos)->name, "constructor") != 0 && strcmp((const char *)nodes.at(start_pos)->name, "constructor_decl") != 0
      && strcmp((const char *)nodes.at(start_pos)->name, "destructor") != 0 && strcmp((const char *)nodes.at(start_pos)->name, "destructor_decl")) != 0) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  int name_start_pos = start_pos + 1;

  if(strcmp((const char *)nodes.at(start_pos)->name, "function") != 0 || strcmp((const char *)nodes.at(start_pos)->name, "function_decl") != 0)
    skip_type(nodes, name_start_pos);
  else
    skip_specifiers(nodes, ++name_start_pos);

  while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || strcmp((const char *)nodes.at(name_start_pos)->name, "name") != 0)
    ++name_start_pos;

  return get_name(nodes, name_start_pos);

}

bool has_block(std::vector<xNodePtr> & nodes, int & start_pos) {

  xNodePtr & start_node = nodes.at(start_pos);

  if(start_node->type != XML_READER_TYPE_ELEMENT || start_node->extra & 0x1) return false;

  start_pos += 1;
  int open_structure_count = 1;

  while(open_structure_count) {

    if(strcmp((const char *)nodes.at(start_pos)->name, (const char *)start_node->name) == 0) {

      if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
        ++open_structure_count;
      else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_structure_count;

    } else if(nodes.at(start_pos)->type == XML_READER_TYPE_ELEMENT
              && strcmp((const char *)nodes.at(start_pos)->name, "block") == 0)
      return true;

    ++start_pos;

  }

  return false;
}

bool conditional_has_block(std::vector<xNodePtr> & nodes, int start_pos) {

  xNodePtr & start_node = nodes.at(start_pos);

  int block_pos = start_pos;
  bool is_block = has_block(nodes, block_pos);

  if(!is_block) return false;

  int previous_element_pos = block_pos - 1;

  while(previous_element_pos > start_pos
    && (nodes.at(previous_element_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
    && nodes.at(previous_element_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT))
    --previous_element_pos;

  if(strcmp((const char *)nodes.at(previous_element_pos)->name, "then") == 0)
    return true;

  if(strcmp((const char *)nodes.at(previous_element_pos)->name, "condition") == 0)
    return true;

  if(strcmp((const char *)nodes.at(previous_element_pos)->name, "incr") == 0)
    return true;

  if(strcmp((const char *)nodes.at(previous_element_pos)->name, "init") == 0)
    return true;

  return false;

}

bool reject_match(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, int old_pos, std::vector<xNodePtr> & nodes_new, int new_pos) {

  std::string old_tag = nodes_old.at(old_pos)->name;
  std::string new_tag = nodes_new.at(new_pos)->name;

  if(old_tag != new_tag) return true;

  if(old_tag == "name" || old_tag == "type" || old_tag == "then" || old_tag == "block" || old_tag == "condition" || old_tag == "expr"
    // change to class name
    || old_tag == "class" || old_tag == "struct" || old_tag == "union"
    || old_tag == "private" || old_tag == "protected" || old_tag == "public"
    || old_tag == "parameter_list" || old_tag == "krparameter_list" || old_tag == "argument_list" || old_tag == "member_list"
    || old_tag == "attribute_list" || old_tag == "association_list" || old_tag == "protocol_list"
    || old_tag == "lit:literal" || old_tag == "op:operator" || old_tag == "type:modifier")
    return false;

  if(is_single_call_expr(nodes_old, old_pos) && is_single_call_expr(nodes_new, new_pos)) {

    while(nodes_old.at(old_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
      || strcmp((const char *)nodes_old.at(old_pos)->name, "call") != 0)
      ++old_pos;

    while(nodes_new.at(new_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
      || strcmp((const char *)nodes_new.at(new_pos)->name, "call") != 0)
      ++new_pos;

    std::string old_name = get_call_name(nodes_old, old_pos);
    std::string new_name = get_call_name(nodes_new, new_pos);

    if(old_name == new_name) return false;

  } else if(old_tag == "call") {

    std::string old_name = get_call_name(nodes_old, old_pos);
    std::string new_name = get_call_name(nodes_new, new_pos);

    if(old_name == new_name) return false;

  } else if(old_tag == "decl" || old_tag == "decl_stmt") {

    std::string old_name = get_decl_name(nodes_old, old_pos);
    std::string new_name = get_decl_name(nodes_new, new_pos);

    if(old_name == new_name) return false;


  } else if(old_tag == "function" || old_tag == "function_decl"
    || old_tag == "constructor" || old_tag == "constructor_decl"
    || old_tag == "destructor" || old_tag == "destructor_decl") {

    std::string old_name = get_decl_name(nodes_old, old_pos);
    std::string new_name = get_decl_name(nodes_new, new_pos);

    if(old_name == new_name) return false;

  } else if(old_tag == "if") {

    std::string old_condition = get_condition(nodes_old, old_pos);
    std::string new_condition = get_condition(nodes_new, new_pos);

    bool old_has_block = conditional_has_block(nodes_old, old_pos);
    bool new_has_block = conditional_has_block(nodes_new, new_pos);

    if(old_condition == new_condition && old_has_block == new_has_block) return false;

  }

  int min_size = text_old_length < text_new_length ? text_old_length : text_new_length;
  int max_size = text_old_length < text_new_length ? text_new_length : text_old_length;

  if(min_size <= 2)
    return 2 * similarity < min_size || difference > min_size;
  else if(min_size <= 3)
    return 3 * similarity < 2 * min_size || difference > min_size;
  else if(min_size <= 30)
    return 10 * similarity < 7 * min_size || difference > min_size;
  else
    return 2 * similarity < min_size || difference > min_size;

}

