#include <srcDiffUtility.hpp>
#include <srcDiffTypes.hpp>
#include <string.h>
#include <vector>
#include <string>
#include <xmlrw.hpp>
#include <srcDiffDiff.hpp>

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

bool is_white_space(const xNodePtr node) {

  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]);

}

bool is_new_line(const xNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content[0] == '\n';

}

bool is_text(const xNodePtr node) {

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

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
   && strcmp((const char *)nodes.at(start_pos)->name, "decl_stmt") == 0)
    ++start_pos;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
   && strcmp((const char *)nodes.at(start_pos)->name, "param") == 0)
    ++start_pos;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
   && strcmp((const char *)nodes.at(start_pos)->name, "decl") == 0)
    ++start_pos;

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || strcmp((const char *)nodes.at(start_pos)->name, "type") != 0)
    return;

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

  if(strcmp((const char *)nodes.at(name_start_pos)->name, "argument_list") == 0) return "";

  return get_name(nodes, name_start_pos);

}

std::string get_decl_name(std::vector<xNodePtr> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (strcmp((const char *)nodes.at(start_pos)->name, "decl_stmt") != 0
      && strcmp((const char *)nodes.at(start_pos)->name, "param") != 0
      && strcmp((const char *)nodes.at(start_pos)->name, "decl") != 0)) return "";
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

std::string get_class_type_name(std::vector<xNodePtr> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (strcmp((const char *)nodes.at(start_pos)->name, "class") != 0 && strcmp((const char *)nodes.at(start_pos)->name, "struct") != 0
      && strcmp((const char *)nodes.at(start_pos)->name, "union") != 0 && strcmp((const char *)nodes.at(start_pos)->name, "enum") != 0)) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  int name_start_pos = start_pos + 1;

  while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || (strcmp((const char *)nodes.at(name_start_pos)->name, "name") != 0
    && strcmp((const char *)nodes.at(name_start_pos)->name, "block") != 0))
    ++name_start_pos;

  if(strcmp((const char *)nodes.at(name_start_pos)->name, "name") == 0)
    return get_name(nodes, name_start_pos);
  else
    return "";

}

int find_end(std::vector<xNodePtr> & nodes, int start_pos) {

  xNodePtr & start_node = nodes.at(start_pos);

  if(start_node->type != (xmlElementType)XML_READER_TYPE_ELEMENT || start_node->extra & 0x1) return -1;

  int end_pos = start_pos + 1;
  int open_structure_count = 1;

  while(open_structure_count) {

    if(strcmp((const char *)nodes.at(end_pos)->name, (const char *)start_node->name) == 0) {

      if(nodes.at(end_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(end_pos)->extra & 0x1) == 0)
        ++open_structure_count;
      else if(nodes.at(end_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_structure_count;

    }

    if(open_structure_count == 0) return end_pos;

    ++end_pos;

  }

  return -1;

}

bool conditional_has_block(std::vector<xNodePtr> & nodes, int start_pos) {

  xNodePtr & start_node = nodes.at(start_pos);

  int end_pos = find_end(nodes, start_pos);

  if(end_pos == -1) return false;

  NodeSets node_sets = create_node_set(nodes, start_pos + 1, end_pos);

  for(NodeSets::iterator itr = node_sets.begin(); itr != node_sets.end(); ++itr) {

    if(strcmp((const char *)nodes.at((*itr)->at(0))->name, "block") == 0) {

      free_node_sets(node_sets);

      return true;

    } else if(strcmp((const char *)nodes.at((*itr)->at(0))->name, "then") == 0) {

      int next_element_pos = (*itr)->at(0) + 1;
      while(nodes.at(next_element_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(next_element_pos)->type != (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        ++next_element_pos;

      free_node_sets(node_sets);

      if(nodes.at(next_element_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
        && strcmp((const char *)nodes.at(next_element_pos)->name, "block") == 0)
        return true;
      else
        return false;

    }

  }

  free_node_sets(node_sets);

  return false;

}

bool if_has_else(std::vector<xNodePtr> & nodes, int start_pos) {

  xNodePtr & start_node = nodes.at(start_pos);

  int end_pos = find_end(nodes, start_pos);

  if(end_pos == -1) return false;

  NodeSets node_sets = create_node_set(nodes, start_pos + 1, end_pos);

  for(NodeSets::iterator itr = node_sets.begin(); itr != node_sets.end(); ++itr) {

    if(strcmp((const char *)nodes.at((*itr)->at(0))->name, "else") == 0 || strcmp((const char *)nodes.at((*itr)->at(0))->name, "elseif") == 0) {

      free_node_sets(node_sets);

      return true;

    }

  }

  free_node_sets(node_sets);

  return false;

}

bool for_group_matches(std::vector<xNodePtr> & nodes_old, int start_pos_old, std::vector<xNodePtr> & nodes_new, int start_pos_new) {

  int end_pos_old = find_end(nodes_old, start_pos_old);
  int end_pos_new = find_end(nodes_new, start_pos_new);

  diff_nodes dnodes = { nodes_old, nodes_new };

  NodeSets node_sets_old = create_node_set(nodes_old, start_pos_old + 1, end_pos_old);
  NodeSets node_sets_new = create_node_set(nodes_new, start_pos_new + 1, end_pos_new);

  for(int i = 0; i < node_sets_old.size() && i < node_sets_new.size(); ++i) {

    /** @todo possible has group tag or whatever although does not currently exist (may have at some point though) */
    if(is_text(nodes_old.at(node_sets_old.at(i)->at(0))) && is_text(nodes_new.at(node_sets_new.at(i)->at(0))) 
      && strcmp((const char *)nodes_old.at(node_sets_old.at(i)->at(0))->content, ")") == 0
      && strcmp((const char *)nodes_new.at(node_sets_new.at(i)->at(0))->content, ")") == 0) {

      free_node_sets(node_sets_old);
      free_node_sets(node_sets_new);

      return true;

    }

    if(node_set_syntax_compare((void *)node_sets_old.at(i), (void *)node_sets_new.at(i), (void *)&dnodes) != 0) {

      free_node_sets(node_sets_old);
      free_node_sets(node_sets_new);

      return false;

    }

  }

  free_node_sets(node_sets_old);
  free_node_sets(node_sets_new);

  return true;

}

std::string get_case_expr(std::vector<xNodePtr> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || strcmp((const char *)nodes.at(start_pos)->name, "case") != 0 || (nodes.at(start_pos)->extra & 0x1)) return "";

  // skip case tag and case text
  int expr_pos = start_pos + 1;

  if((is_text(nodes.at(expr_pos)) && index(nodes.at(expr_pos)->content, ':'))
     || (nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && strcmp((const char *)nodes.at(start_pos)->name, "case") == 0)) return "";

  while((nodes.at(expr_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || strcmp((const char *)nodes.at(expr_pos)->name, "expr") != 0) 
    && !(is_text(nodes.at(expr_pos)) && index(nodes.at(expr_pos)->content, ':'))
    && !(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && strcmp((const char *)nodes.at(start_pos)->name, "case") == 0))
    ++expr_pos;

  if((is_text(nodes.at(expr_pos)) && index(nodes.at(expr_pos)->content, ':'))
    || (nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && strcmp((const char *)nodes.at(start_pos)->name, "case") == 0)) return "";

  std::string case_expr = "";

  int open_expr_count = nodes.at(expr_pos)->extra & 0x1 ? 0 : 1;
  ++expr_pos;

  while(open_expr_count) {

    if(strcmp((const char *)nodes.at(expr_pos)->name, "expr") == 0) {

      if(nodes.at(expr_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(expr_pos)->extra & 0x1) == 0)
        ++open_expr_count;
      else if(nodes.at(expr_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_expr_count;

    } else if(is_text(nodes.at(expr_pos)) && !is_white_space(nodes.at(expr_pos))) {

      case_expr += (const char *)nodes.at(expr_pos)->content;

    }

    ++expr_pos;

  }

  return case_expr;

}

bool reject_match(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, int old_pos, std::vector<xNodePtr> & nodes_new, int new_pos) {

  std::string old_tag = nodes_old.at(old_pos)->name;
  std::string new_tag = nodes_new.at(new_pos)->name;

  if(old_tag != new_tag) return true;

  if(old_tag == "name" || old_tag == "type" || old_tag == "then" || old_tag == "block" || old_tag == "condition"
    || old_tag == "default" || old_tag == "comment"
    || old_tag == "private" || old_tag == "protected" || old_tag == "public" || old_tag == "signals"
    || old_tag == "parameter_list" || old_tag == "krparameter_list" || old_tag == "argument_list" || old_tag == "member_list"
    || old_tag == "attribute_list" || old_tag == "association_list" || old_tag == "protocol_list"
    || old_tag == "argument"
    || old_tag == "lit:literal" || old_tag == "op:operator" || old_tag == "type:modifier")
    return false;

  if(old_tag == "expr" && similarity > 0) return false;

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

  } else if(old_tag == "decl" || old_tag == "decl_stmt" || old_tag == "param") {

    std::string old_name = get_decl_name(nodes_old, old_pos);
    std::string new_name = get_decl_name(nodes_new, new_pos);

    if(old_name == new_name) return false;

  } else if(old_tag == "function"    || old_tag == "function_decl"
         || old_tag == "constructor" || old_tag == "constructor_decl"
         || old_tag == "destructor"  || old_tag == "destructor_decl") {

    std::string old_name = get_function_type_name(nodes_old, old_pos);
    std::string new_name = get_function_type_name(nodes_new, new_pos);

    if(old_name == new_name) return false;

  } else if(old_tag == "if") {

    std::string old_condition = get_condition(nodes_old, old_pos);
    std::string new_condition = get_condition(nodes_new, new_pos);

    bool old_has_block = conditional_has_block(nodes_old, old_pos);
    bool new_has_block = conditional_has_block(nodes_new, new_pos);

    bool old_has_else = if_has_else(nodes_old, old_pos);
    bool new_has_else = if_has_else(nodes_new, new_pos);

    if(old_condition == new_condition 
      && (old_has_block == new_has_block || ((old_has_block || !old_has_else) && (new_has_block || !new_has_else))))
     return false;

  } else if(old_tag == "while" || old_tag == "switch") {

    std::string old_condition = get_condition(nodes_old, old_pos);
    std::string new_condition = get_condition(nodes_new, new_pos);

    if(old_condition == new_condition) return false;

  } else if(old_tag == "for" || old_tag == "foreach") {

    if(for_group_matches(nodes_old, old_pos, nodes_new, new_pos))
      return false;
    
  } else if(old_tag == "case") { 

    std::string old_expr = get_case_expr(nodes_old, old_pos);
    std::string new_expr = get_case_expr(nodes_new, new_pos);

    if(old_expr == new_expr) return false;

  } else if(old_tag == "class" || old_tag == "struct" || old_tag == "union" || old_tag == "enum") {

    std::string old_name = get_class_type_name(nodes_old, old_pos);
    std::string new_name = get_class_type_name(nodes_new, new_pos);

    if(old_name == new_name && old_name != "") return false;

  } else if(old_tag == "comment") {

    if(node_compare(nodes_old.at(old_pos), nodes_new.at(new_pos)) == 0) return false;

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

