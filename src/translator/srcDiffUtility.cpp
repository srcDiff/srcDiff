#include <srcDiffUtility.hpp>

#include <srcdiff_diff.hpp>
#include <srcdiff_nested.hpp>

#include <srcDiffMeasure.hpp>
#include <ShortestEditScript.hpp>

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
  node_sets & sets = *(node_sets *)s;
  return sets[idx];
}

const void * node_index(int idx, const void *s, const void * context) {
  node_set & set = *(node_set *)s;
  return &set[idx];
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

  node_set * node_set1 = (node_set *)e1;
  node_set * node_set2 = (node_set *)e2;

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

int string_compare(const void * s1, const void * s2, const void * context) {

  std::string & string1 = *(std::string *)s1;
  std::string & string2 = *(std::string *)s2;

  return strcmp(string1.c_str(), string2.c_str());

}

const void * string_index(int idx, const void * s, const void * context) {

  std::vector<std::string> & string_list = *(std::vector<std::string> *)s;

  return &string_list[idx];

}

const char * find_attribute(const xNodePtr node, const char * attr_name) {

  xAttr * attr = node->properties;

  for(; attr && strcmp((const char *)attr->name, attr_name) != 0; attr = attr->next)
    ;

  if(attr)
    return attr->value;

  return 0;

}

/*
  Begin internal heuristic functions for reject_match
*/


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
   && strcmp((const char *)nodes.at(start_pos)->name, "parameter") == 0)
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

std::vector<std::string> get_call_name(std::vector<xNodePtr> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || strcmp((const char *)nodes.at(start_pos)->name, "call") != 0) return std::vector<std::string>();
  if(nodes.at(start_pos)->extra & 0x1) return std::vector<std::string>();

  int name_start_pos = start_pos + 1;

  while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || (strcmp((const char *)nodes.at(name_start_pos)->name, "name") != 0 && strcmp((const char *)nodes.at(name_start_pos)->name, "argument_list") != 0))
    ++name_start_pos;

  if(strcmp((const char *)nodes.at(name_start_pos)->name, "argument_list") == 0) return std::vector<std::string>();

  std::vector<std::string> name_list;

  int open_name_count = nodes.at(name_start_pos)->extra & 0x1 ? 0 : 1;
  int name_pos = name_start_pos + 1;
  std::string name = "";

  while(open_name_count) {

    if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && strcmp((const char *)nodes.at(name_pos)->name, "argument_list") == 0) return name_list;

    if(strcmp((const char *)nodes.at(name_pos)->name, "name") == 0) {

      if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(name_pos)->extra & 0x1) == 0) {

        ++open_name_count;
        name = "";

      } else if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)

        --open_name_count;
        if(name != "")
          name_list.push_back(name);
        name = "";

    } else if(is_text(nodes.at(name_pos)) && !is_white_space(nodes.at(name_pos))) {

      name += (const char *)nodes.at(name_pos)->content;

    }

    ++name_pos;

  }

  return name_list;

}

int name_list_similarity(std::vector<std::string> name_list_old, std::vector<std::string> name_list_new) {

  ShortestEditScript ses(string_compare, string_index, 0);

  ses.compute(&name_list_old, name_list_old.size(), &name_list_new, name_list_new.size());

  edit * edits = ses.get_script();

  int similarity = 0;

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    switch(edits->operation) {

      case SESDELETE :

        delete_similarity += edits->length;
        break;

      case SESINSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = name_list_old.size() - delete_similarity;
  insert_similarity = name_list_new.size() - insert_similarity;

  similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(similarity < 0)
    similarity = 0;

  return similarity;

}

std::string get_decl_name(std::vector<xNodePtr> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (strcmp((const char *)nodes.at(start_pos)->name, "decl_stmt") != 0
      && strcmp((const char *)nodes.at(start_pos)->name, "parameter") != 0
      && strcmp((const char *)nodes.at(start_pos)->name, "param") != 0
      && strcmp((const char *)nodes.at(start_pos)->name, "decl") != 0)) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  int name_start_pos = start_pos + 1;

  skip_type(nodes, name_start_pos);

  while(!(nodes.at(name_start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
      && strcmp((const char *)nodes.at(name_start_pos)->name, "name") == 0)
    && !(nodes.at(name_start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
      && strcmp((const char *)nodes.at(name_start_pos)->name, "decl") == 0))
    ++name_start_pos;

  if(nodes.at(name_start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
      && strcmp((const char *)nodes.at(name_start_pos)->name, "decl") == 0)
    return "";

  return get_name(nodes, name_start_pos);

}

std::string get_for_condition(std::vector<xNodePtr> & nodes, int start_pos) {

  int control_start_pos = start_pos;

  while(nodes.at(control_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || strcmp((const char *)nodes.at(control_start_pos)->name, "control") != 0)
    ++control_start_pos;

  if(nodes.at(control_start_pos)->extra & 0x1) return "";

  int control_end_pos = control_start_pos + 1;
  int open_control_count = 1;

  while(open_control_count) {

    if(strcmp((const char *)nodes.at(control_end_pos)->name, "control") == 0) {

      if(nodes.at(control_end_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(control_end_pos)->extra & 0x1) == 0)
        ++open_control_count;
      else if(nodes.at(control_end_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_control_count;

    }

    ++control_end_pos;

  }

  node_sets control_sets = node_sets(nodes, control_start_pos + 1, control_end_pos);

  node_sets::const_iterator citr;
  for(citr = control_sets.begin(); citr != control_sets.end(); ++citr)
    if(strcmp((const char *)nodes.at((*citr)->front())->name, "condition") == 0)
      break;

  if(citr == control_sets.end()) return "";

  std::string condition = "";
  for(node_set::const_iterator node_itr = (*citr)->begin(); node_itr != (*citr)->end(); ++node_itr)
    if(is_text(nodes.at(*node_itr)))
      condition += (const char *)nodes.at(*node_itr)->content;

  if(condition.size() > 0 && *--condition.end() == ';')
    condition.erase(--condition.end());

  return condition;

}

std::string get_condition(std::vector<xNodePtr> & nodes, int start_pos) {

  if(strcmp((const char *)nodes.at(start_pos)->name, "for") == 0
    || strcmp((const char *)nodes.at(start_pos)->name, "foreach") == 0)
    return get_for_condition(nodes, start_pos);

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

  if(condition.size() > 0 && *condition.begin() == '(')
    condition.erase(condition.begin());

  if(condition.size() > 0 && *--condition.end() == ')')
    condition.erase(--condition.end());

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

bool conditional_has_block(std::vector<xNodePtr> & nodes, node_set * set) {

  node_sets sets = node_sets(nodes, set->at(1), set->back());

  for(node_sets::iterator itr = sets.begin(); itr != sets.end(); ++itr) {

    if(strcmp((const char *)nodes.at((*itr)->at(0))->name, "block") == 0 && find_attribute(nodes.at((*itr)->at(0)), "type") == 0) {

      return true;

    } else if(strcmp((const char *)nodes.at((*itr)->at(0))->name, "then") == 0) {

      int next_element_pos = (*itr)->at(0) + 1;
      while(nodes.at(next_element_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(next_element_pos)->type != (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        ++next_element_pos;

      if(nodes.at(next_element_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
        && strcmp((const char *)nodes.at(next_element_pos)->name, "block") == 0
        && find_attribute(nodes.at((next_element_pos)), "type") == 0)
        return true;
      else
        return false;

    }

  }

  return false;

}

bool if_has_else(std::vector<xNodePtr> & nodes, node_set * set) {

  node_sets sets = node_sets(nodes, set->at(1), set->back());

  for(node_sets::iterator itr = sets.begin(); itr != sets.end(); ++itr) {

    if(strcmp((const char *)nodes.at((*itr)->at(0))->name, "else") == 0 || strcmp((const char *)nodes.at((*itr)->at(0))->name, "elseif") == 0) {

      return true;

    }

  }

  return false;

}

bool if_then_equal(std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  node_sets node_sets_old = node_sets(nodes_old, set_old->at(1), set_old->back());
  node_sets node_sets_new = node_sets(nodes_new, set_new->at(1), set_new->back());

  node_sets::iterator then_old;
  for(then_old = node_sets_old.begin(); then_old != node_sets_old.end(); ++then_old) {

    if(strcmp((const char *)nodes_old.at((*then_old)->at(0))->name, "then") == 0) {

      break;

    }

  }

  node_sets::iterator then_new;
  for(then_new = node_sets_new.begin(); then_new != node_sets_new.end(); ++then_new) {

    if(strcmp((const char *)nodes_new.at((*then_new)->at(0))->name, "then") == 0) {

      break;

    }

  }

  bool then_is_equal = node_set_syntax_compare((void *)*then_old, (void *)*then_new, (void *)&dnodes) == 0;

  return then_is_equal;

}

bool for_control_matches(std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  node_sets node_sets_old = node_sets(nodes_old, set_old->at(1), set_old->back());
  node_sets node_sets_new = node_sets(nodes_new, set_new->at(1), set_new->back());

  node_sets::size_type control_pos_old;
  for(control_pos_old = 0; control_pos_old < node_sets_old.size(); ++control_pos_old)
    if(strcmp((const char *)nodes_old.at(node_sets_old.at(control_pos_old)->front())->name, "control") == 0)
      break;

  node_sets::size_type control_pos_new;
  for(control_pos_new = 0; control_pos_new < node_sets_new.size(); ++control_pos_new)
    if(strcmp((const char *)nodes_new.at(node_sets_new.at(control_pos_new)->front())->name, "control") == 0)
      break;

  bool matches = control_pos_old != node_sets_old.size() && control_pos_new != node_sets_new.size() 
    && node_set_syntax_compare((void *)node_sets_old.at(control_pos_old), (void *)node_sets_new.at(control_pos_new), (void *)&dnodes) == 0;

  return matches;

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

/*
  End internal heuristic functions for reject_match
*/

struct interchange_list {

  const char * const name;
  const char * const * list;

};

static const char * const if_interchange[]   = { "if",   "while", "for", "foreach", 0 };
static const char * const else_interchange[] = { "else", "elseif",                  0 };
static const interchange_list interchange_lists[] = {

  { "if",      if_interchange },
  { "while",   if_interchange },
  { "for",     if_interchange },
  { "foreach", if_interchange },
  
  { "else",    else_interchange },
  { "elseif",  else_interchange },

  { 0, 0 }

};

bool is_interchangeable_match(const std::string & old_tag, const std::string & new_tag) {

  for(size_t list_pos = 0; interchange_lists[list_pos].name; ++list_pos) {

    if(interchange_lists[list_pos].name == old_tag) {

      for(size_t pos = 0; interchange_lists[list_pos].list[pos]; ++pos) {

        if(interchange_lists[list_pos].list[pos] == new_tag)
          return true;

      }


    }

  }

  return false;

}

bool reject_similarity(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new) {

  int syntax_similarity, syntax_difference, children_length_old, children_length_new;
  compute_syntax_measures(nodes_old, set_old, nodes_new, set_new, syntax_similarity, syntax_difference, children_length_old, children_length_new);

  int min_child_length = children_length_old < children_length_new ? children_length_old : children_length_new;
  int max_child_length = children_length_old < children_length_new ? children_length_new : children_length_old;

  if(min_child_length > 1) { 

    if(2 * syntax_similarity >= min_child_length && syntax_difference <= min_child_length)
      return false;

  }

  node_sets child_node_sets_old = node_sets(nodes_old, set_old->at(1), set_old->back());
  node_sets child_node_sets_new = node_sets(nodes_new, set_new->at(1), set_new->back());    

  if(strcmp(nodes_old.at(child_node_sets_old.back()->at(0))->name, "then") == 0) {

    node_sets temp = node_sets(nodes_old, child_node_sets_old.back()->at(1), child_node_sets_old.back()->back());
    child_node_sets_old = temp;

  }

  if(strcmp(nodes_new.at(child_node_sets_new.back()->at(0))->name, "then") == 0) {

    node_sets temp = node_sets(nodes_new, child_node_sets_new.back()->at(1), child_node_sets_new.back()->back());
    child_node_sets_new = temp;

  }

  if(strcmp(nodes_old.at(child_node_sets_old.back()->at(0))->name, "block") == 0
    && strcmp(nodes_new.at(child_node_sets_new.back()->at(0))->name, "block") == 0) {

    compute_syntax_measures(nodes_old, child_node_sets_old.back(), nodes_new, child_node_sets_new.back(), syntax_similarity, syntax_difference, children_length_old, children_length_new);

    min_child_length = children_length_old < children_length_new ? children_length_old : children_length_new;
    max_child_length = children_length_old < children_length_new ? children_length_new : children_length_old;      

    if(min_child_length > 1) { 

      if(2 * syntax_similarity >= min_child_length && syntax_difference <= min_child_length)
        return false;

    }

  }

  int min_size = text_old_length < text_new_length ? text_old_length : text_new_length;
  int max_size = text_old_length < text_new_length ? text_new_length : text_old_length;

  if(min_size <= 2)
    return 2 * similarity < min_size || (difference > 1.25 * min_size) || difference > max_size;
  else if(min_size <= 3)
    return 3 * similarity < 2 * min_size || (difference > 1.25 * min_size) || difference > max_size;
  else if(min_size <= 30)
    return 10 * similarity < 7 * min_size || (difference > 1.25 * min_size) || difference > max_size;
  else
    return 2 * similarity < min_size || (difference > 1.25 * min_size) || difference > max_size;

}

bool reject_match_same(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new) {

  int old_pos = set_old->at(0);
  int new_pos = set_new->at(0);

  std::string old_tag = nodes_old.at(old_pos)->name;
  std::string new_tag = nodes_new.at(new_pos)->name;

  if(old_tag != new_tag) return true;

  if(old_tag == "name" || old_tag == "type" || old_tag == "then" || old_tag == "condition" || old_tag == "init"
    || old_tag == "default" || old_tag == "comment"
    || old_tag == "private" || old_tag == "protected" || old_tag == "public" || old_tag == "signals"
    || old_tag == "parameter_list" || old_tag == "krparameter_list" || old_tag == "argument_list" || old_tag == "member_list"
    || old_tag == "attribute_list" || old_tag == "association_list" || old_tag == "protocol_list"
    || old_tag == "argument"
    || old_tag == "literal" || old_tag == "operator" || old_tag == "modifier")
    return false;

  if((old_tag == "expr" || old_tag == "expr_stmt") && similarity > 0) return false;

  if(old_tag == "block") {

    bool is_pseudo_old = find_attribute(nodes_old.at(old_pos), "type") != 0;
    bool is_pseudo_new = find_attribute(nodes_new.at(new_pos), "type") != 0;

    if(is_pseudo_old == is_pseudo_new) {

      return false;

    } else if(similarity) {

      bool is_reject = true;

      if(is_pseudo_old) {

        node_sets node_sets_old = node_sets(nodes_old, set_old->at(1), set_old->back());
        node_sets node_sets_new = node_sets(nodes_new, set_new->at(0), set_new->back() + 1);

        int start_nest_old, end_nest_old, start_nest_new, end_nest_new, operation;
        srcdiff_nested::check_nestable(&node_sets_old, nodes_old, 0, node_sets_old.size(), &node_sets_new, nodes_new, 0, 1,
                      start_nest_old, end_nest_old, start_nest_new , end_nest_new, operation);


        is_reject = !(operation == SESINSERT);

      } else {

        node_sets node_sets_old = node_sets(nodes_old, set_old->at(0), set_old->back() + 1);
        node_sets node_sets_new = node_sets(nodes_new, set_new->at(1), set_new->back());

        int start_nest_old, end_nest_old, start_nest_new, end_nest_new, operation;
        srcdiff_nested::check_nestable(&node_sets_old, nodes_old, 0, 1, &node_sets_new, nodes_new, 0, node_sets_new.size(),
                      start_nest_old, end_nest_old, start_nest_new , end_nest_new, operation);


        is_reject = !(operation == SESDELETE);

      }

      return is_reject;

    }

  }

  if(is_single_call_expr(nodes_old, old_pos) && is_single_call_expr(nodes_new, new_pos)) {

    while(nodes_old.at(old_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
      || strcmp((const char *)nodes_old.at(old_pos)->name, "call") != 0)
      ++old_pos;

    while(nodes_new.at(new_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
      || strcmp((const char *)nodes_new.at(new_pos)->name, "call") != 0)
      ++new_pos;

    std::vector<std::string> old_names = get_call_name(nodes_old, old_pos);
    std::vector<std::string> new_names = get_call_name(nodes_new, new_pos);

    if(name_list_similarity(old_names, new_names)) return false;

  } else if(old_tag == "call") {

    std::vector<std::string> old_names = get_call_name(nodes_old, old_pos);
    std::vector<std::string> new_names = get_call_name(nodes_new, new_pos);

    if(name_list_similarity(old_names, new_names)) return false;

  } else if(old_tag == "decl" || old_tag == "decl_stmt" || old_tag == "parameter" || old_tag == "param") {

    std::string old_name = get_decl_name(nodes_old, old_pos);
    std::string new_name = get_decl_name(nodes_new, new_pos);

    if(old_name == new_name && old_name != "") return false;

  } else if(old_tag == "function"    || old_tag == "function_decl"
         || old_tag == "constructor" || old_tag == "constructor_decl"
         || old_tag == "destructor"  || old_tag == "destructor_decl") {

    std::string old_name = get_function_type_name(nodes_old, old_pos);
    std::string new_name = get_function_type_name(nodes_new, new_pos);

    if(old_name == new_name) return false;

  } else if(old_tag == "if") {

    std::string old_condition = get_condition(nodes_old, old_pos);
    std::string new_condition = get_condition(nodes_new, new_pos);

    bool old_has_block = conditional_has_block(nodes_old, set_old);
    bool new_has_block = conditional_has_block(nodes_new, set_new);

    bool old_has_else = if_has_else(nodes_old, set_old);
    bool new_has_else = if_has_else(nodes_new, set_new);

    if(if_then_equal(nodes_old, set_old, nodes_new, set_new) || (old_condition == new_condition
      && (old_has_block == new_has_block || old_has_else == new_has_else || ((old_has_block || !old_has_else) && (new_has_block || !new_has_else)))))
     return false;

  } else if(old_tag == "while" || old_tag == "switch") {

    std::string old_condition = get_condition(nodes_old, old_pos);
    std::string new_condition = get_condition(nodes_new, new_pos);

    if(old_condition == new_condition) return false;

  } else if(old_tag == "for" || old_tag == "foreach") {

    if(for_control_matches(nodes_old, set_old, nodes_new, set_new))
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

  // int syntax_similarity, syntax_difference, children_length_old, children_length_new;
  // compute_syntax_measures(nodes_old, set_old, nodes_new, set_new, syntax_similarity, syntax_difference, children_length_old, children_length_new);

  // int min_child_length = children_length_old < children_length_new ? children_length_old : children_length_new;
  // int max_child_length = children_length_old < children_length_new ? children_length_new : children_length_old;

  // if(min_child_length > 1) { 

  //   if(min_child_length < 3 && 2 * syntax_similarity >= min_child_length && syntax_difference <= min_child_length)
  //     return false;

  //   if(min_child_length > 2 && 3 * syntax_similarity >= 2 * min_child_length && syntax_difference <= min_child_length) 
  //     return false;

  // }

  bool is_reject = reject_similarity(similarity, difference, text_old_length, text_new_length, nodes_old, set_old, nodes_new, set_new);
  return is_reject;

}

bool reject_match_interchangeable(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new) {

  int old_pos = set_old->at(0);
  int new_pos = set_new->at(0);

  std::string old_tag = nodes_old.at(old_pos)->name;
  std::string new_tag = nodes_new.at(new_pos)->name;

  std::string old_condition = "";
  if(old_tag == "if" || old_tag == "while" || old_tag == "for" || old_tag == "foreach") {

    old_condition = get_condition(nodes_old, old_pos);

  }

  std::string new_condition = "";
  if(new_tag == "if" || new_tag == "while" || new_tag == "for" || new_tag == "foreach") {

    new_condition = get_condition(nodes_new, new_pos);

  }

  if(old_condition != "" && old_condition == new_condition) return false;

  bool is_reject = reject_similarity(similarity, difference, text_old_length, text_new_length, nodes_old, set_old, nodes_new, set_new);
  return is_reject;

}

bool reject_match(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new) {

  /** if different prefix should not reach here, however, may want to add that here */
  int old_pos = set_old->at(0);
  int new_pos = set_new->at(0);

  std::string old_tag = nodes_old.at(old_pos)->name;
  std::string new_tag = nodes_new.at(new_pos)->name;

  if(old_tag == new_tag)
    return reject_match_same(similarity, difference, text_old_length, text_new_length, nodes_old, set_old, nodes_new, set_new);
  else if(is_interchangeable_match(old_tag, new_tag)) 
    return reject_match_interchangeable(similarity, difference, text_old_length, text_new_length, nodes_old, set_old, nodes_new, set_new);
  else
    return true;

}
