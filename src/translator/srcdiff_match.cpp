#include <srcdiff_match.hpp>

#include <srcdiff_nested.hpp>
#include <srcdiff_text_measure.hpp>
#include <srcdiff_syntax_measure.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>

#include <list>

#include <cstring>

struct difference {

  //unsigned long long similarity;
  int similarity;
  int num_unmatched;
  bool marked;
  int direction;
  unsigned int opos;
  unsigned int npos;

};

bool srcdiff_match::is_match_default(const node_sets & sets_original, int start_pos_original,
                                     const node_sets & sets_modified, int start_pos_modified,
                                     const srcdiff_measure & measure) {

  if(measure.similarity() == MAX_INT) return false;

  if(reject_similarity_match_only(sets_original.at(start_pos_original),
                                   sets_modified.at(start_pos_modified))
    && reject_match(measure,
                  sets_original.at(start_pos_original),
                  sets_modified.at(start_pos_modified)))
    return false;

  if(srcdiff_nested::is_better_nested(sets_original, start_pos_original,
                                      sets_modified, start_pos_modified,
                                      measure))
    return false;

  return true;

}


srcdiff_match::srcdiff_match(const node_sets & node_sets_original, const node_sets & node_sets_modified,
                             const is_match_func & is_match)
  : node_sets_original(node_sets_original), node_sets_modified(node_sets_modified),
    is_match(is_match) {}

static offset_pair * create_linked_list(int olength, int nlength, difference * differences) {

  // create match linked list
  offset_pair * last_match = NULL;

  bool * olist = (bool *)malloc(olength * sizeof(bool));
  memset(olist, 0, olength * sizeof(bool));

  bool * nlist = (bool *)malloc(nlength * sizeof(bool));
  memset(nlist, 0, nlength * sizeof(bool));

  for(int i = nlength - 1, j = olength - 1; i >= 0 || j >= 0;) {

    // only output marked and if has not already been output
    if(differences[i * olength + j].marked && !(olist[j] || nlist[i])) {

      offset_pair * match = new offset_pair;

      match->original_offset = differences[i * olength + j].opos;
      match->modified_offset = differences[i * olength + j].npos;
      match->similarity = differences[i * olength + j].similarity;
      match->next = last_match;

      last_match = match;

      olist[j] = true;
      nlist[i] = true;

    }

    switch(differences[i * olength + j].direction) {

    case 0:

      --i;
      --j;

      break;

    case 1:

      --j;

      break;

    case 2:

      --i;

      break;

    case 3:

      --i;
      --j;

      break;

    default:

      break;

    }

  }

  free(olist);
  free(nlist);

  return last_match;

}


offset_pair * srcdiff_match::match_differences() {

  /*

    Possible dynamic programming solution.  Sum similarities choosing left top or diagonal,
    picking smallest.  Unmatching has a high cost 1 less than a syntax mismatch.  So, it is chosen over
    a syntax mismatch.  Not sure yet, but left and diagonal probably add cost, and top might be a straight copy
    or a copy plus a unmatch.

    Errata: Now minimizing unmatched then minimizing similarity

    Errata: Now maximizing similarity

  */

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  int olength = node_sets_original.size();
  int nlength = node_sets_modified.size();

  size_t mem_size = olength * nlength * sizeof(difference);

  difference * differences = (difference *)malloc(mem_size);

  // still need to figure out how to track matching on each path
  for(int i = 0; i < nlength; ++i) {

    for(int j = 0; j < olength; ++j) {

      srcdiff_text_measure measure(node_sets_original.at(j), node_sets_modified.at(i));
      measure.compute();
      int similarity = measure.similarity();

      //unsigned long long max_similarity = (unsigned long long)-1;
      int max_similarity = -1;
      int unmatched = 0;

      // check if unmatched
      if(!is_match(node_sets_original, j, node_sets_modified, i, measure)) {

        similarity = 0;
        unmatched = 2;

      }

      int num_unmatched = MAX_INT;
      int direction = 0;

      bool matched = false;

      // check along x axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(j > 0) {

        //max_similarity = differences[i * olength + (j - 1)].similarity + MAX_INT;
        max_similarity = differences[i * olength + (j - 1)].similarity;
        num_unmatched = differences[i * olength + (j - 1)].num_unmatched + 1;

        matched = false;

        // may be wrong
        int temp_num_unmatched = i + j + unmatched;

        //unsigned long long temp_similarity = MAX_INT * num_unmatched + similarity;

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && similarity > max_similarity)) {

          matched = !unmatched;

          //max_similarity = temp_similarity;
          max_similarity = similarity;
          num_unmatched = temp_num_unmatched;

        }

        direction = 1;

      }

      // check along y axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(i > 0) {

        // may not have been initialized in j > 0
        if(direction == 0)
          direction = 2;

        //unsigned long long temp_similarity = differences[(i - 1) * olength + j].similarity + MAX_INT;
        int temp_similarity = differences[(i - 1) * olength + j].similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + j].num_unmatched + 1;

        // may be wrong
        int temp_num_unmatched_match = i + j + unmatched;

        int temp_matched = false;

        //unsigned long long temp_similarity_match = MAX_INT * num_unmatched + similarity;

        //if(temp_similarity_match < temp_similarity) {
        if(temp_num_unmatched_match < temp_num_unmatched || (temp_num_unmatched_match == temp_num_unmatched && similarity > temp_similarity)) {

          temp_matched = !unmatched;

          temp_similarity = similarity;
          temp_num_unmatched = temp_num_unmatched_match;

        }

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity > max_similarity)) {

          matched = temp_matched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;

          direction = 2;

        }

      }

      // go along diagonal just add similarity and unmatched
      if(i > 0 && j > 0) {

        //unsigned long long temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + (j - 1)].num_unmatched + unmatched;

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity > max_similarity)) {

          matched = !unmatched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;
          direction = 3;

        }

      }

      // special case starting node
      if(i == 0 && j == 0) {

        max_similarity = similarity;
        num_unmatched = unmatched;
        matched = !unmatched;

      }

      // update structure
      differences[i * olength + j].marked = matched;
      differences[i * olength + j].similarity = max_similarity;
      differences[i * olength + j].num_unmatched = num_unmatched;
      differences[i * olength + j].opos = j;
      differences[i * olength + j].npos = i;
      differences[i * olength + j].direction = direction;

    }

  }

  // create match linked list
  offset_pair * matches = create_linked_list(olength, nlength, differences);

  // free memory
  free(differences);

  return matches;

}


boost::optional<std::string> find_attribute(const std::shared_ptr<srcml_node> & node, const char * attr_name) {

  const std::list<srcml_node::srcml_attr> & attributes = node->properties;

  std::list<srcml_node::srcml_attr>::size_type pos = 0;
  for(const srcml_node::srcml_attr & attr : attributes) {

    if(attr.name == attr_name)
      return attr.value;

  }

  return boost::optional<std::string>();

}

/*
  Begin internal heuristic functions for reject_match
*/


bool is_single_call_expr(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "expr_stmt" && nodes.at(start_pos)->name != "expr")) return false;

  if(nodes.at(start_pos)->extra & 0x1) return false;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name == "expr_stmt")
    ++start_pos;

  if(nodes.at(start_pos)->extra & 0x1) return false;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name == "expr")
    ++start_pos;

  if(nodes.at(start_pos)->name != "call") return false;

  int open_call_count = (nodes.at(start_pos)->extra & 0x1) ? 0 : 1;
  ++start_pos;

  while(open_call_count) {

    if(nodes.at(start_pos)->name == "call") {

      if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
        ++open_call_count;
      else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_call_count;

    }

    ++start_pos;

  }

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(start_pos)->name == "expr")
    return true;

  return false;
 
}

void skip_tag(const srcml_nodes & nodes, int & start_pos) {

  std::string & start_tag = nodes.at(start_pos)->name;
  int open_type_count = nodes.at(start_pos)->extra & 0x1 ? 0 : 1;
  ++start_pos;

  while(open_type_count) {

    if(nodes.at(start_pos)->name == start_tag) {

      if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
        ++open_type_count;
      else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_type_count;

    }

    ++start_pos;

  }

}

void top_level_name_seek(const srcml_nodes & nodes, int & start_pos) {

    if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(start_pos)->name == "name")
      return;

    std::string & start_tag = nodes.at(start_pos)->name;
    int name_start_pos = start_pos + 1;

    while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || nodes.at(name_start_pos)->name != "name") {

      if(nodes.at(name_start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(name_start_pos)->name == start_tag)
        return;

      if(nodes.at(name_start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT)
        skip_tag(nodes, name_start_pos);
      else
        ++name_start_pos;

    }

    start_pos = name_start_pos;

}

std::string get_name(const srcml_nodes & nodes, int name_start_pos) {

  int open_name_count = nodes.at(name_start_pos)->extra & 0x1 ? 0 : 1;
  int name_pos = name_start_pos + 1;
  std::string name = "";

  while(open_name_count) {

    if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(name_pos)->name == "argument_list") return name;

    if(nodes.at(name_pos)->name == "name") {

      if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(name_pos)->extra & 0x1) == 0)
        ++open_name_count;
      else if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_name_count;

    } else if(nodes.at(name_pos)->is_text() && !nodes.at(name_pos)->is_white_space()) {

      name += nodes.at(name_pos)->content ? *nodes.at(name_pos)->content : "";

    }

    ++name_pos;

  }

  return name;

}

std::string extract_name(const srcml_nodes & nodes, int start_pos) {

  int name_start_pos = start_pos;
  top_level_name_seek(nodes, name_start_pos);

  if(name_start_pos == start_pos) return "";

  return get_name(nodes, name_start_pos);  

}

std::vector<std::string> get_call_name(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || nodes.at(start_pos)->name != "call")
    return std::vector<std::string>();

  if(nodes.at(start_pos)->extra & 0x1) return std::vector<std::string>();

  int name_start_pos = start_pos + 1;

  while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || (nodes.at(name_start_pos)->name != "name" && nodes.at(name_start_pos)->name != "argument_list"))
    ++name_start_pos;

  if(nodes.at(name_start_pos)->name == "argument_list") return std::vector<std::string>();

  std::vector<std::string> name_list;

  int open_name_count = nodes.at(name_start_pos)->extra & 0x1 ? 0 : 1;
  int name_pos = name_start_pos + 1;
  std::string name = "";

  while(open_name_count) {

    if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(name_pos)->name == "argument_list") return name_list;

    if(nodes.at(name_pos)->name == "name") {

      if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(name_pos)->extra & 0x1) == 0) {

        ++open_name_count;
        name = "";

      } else if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)

        --open_name_count;
        if(name != "")
          name_list.push_back(name);
        name = "";

    } else if(nodes.at(name_pos)->is_text() && !nodes.at(name_pos)->is_white_space()) {

      name += nodes.at(name_pos)->content ? *nodes.at(name_pos)->content : 0;

    }

    ++name_pos;

  }

  return name_list;

}

int name_list_similarity(std::vector<std::string> name_list_original, std::vector<std::string> name_list_modified) {

  shortest_edit_script_t ses(srcdiff_compare::string_compare, srcdiff_compare::string_index, 0);

  ses.compute(&name_list_original, name_list_original.size(), &name_list_modified, name_list_modified.size());

  edit_t * edits = ses.script();

  int similarity = 0;

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    switch(edits->operation) {

      case SES_DELETE :

        delete_similarity += edits->length;
        break;

      case SES_INSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = name_list_original.size() - delete_similarity;
  insert_similarity = name_list_modified.size() - insert_similarity;

  similarity = std::min(delete_similarity, insert_similarity);

  if(similarity < 0) similarity = 0;

  return similarity;

}

std::string get_decl_name(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "decl_stmt"
      && nodes.at(start_pos)->name != "parameter"
      && nodes.at(start_pos)->name != "param"
      && nodes.at(start_pos)->name != "decl")) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  if(nodes.at(start_pos)->name != "decl")
    ++start_pos;
  if(nodes.at(start_pos)->extra & 0x1) return "";

  return extract_name(nodes, start_pos);

}

std::string get_for_condition(const srcml_nodes & nodes, int start_pos) {

  int control_start_pos = start_pos;

  while(nodes.at(control_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || nodes.at(control_start_pos)->name != "control")
    ++control_start_pos;

  if(nodes.at(control_start_pos)->extra & 0x1) return "";

  int control_end_pos = control_start_pos + 1;
  int open_control_count = 1;

  while(open_control_count) {

    if(nodes.at(control_end_pos)->name == "control") {

      if(nodes.at(control_end_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(control_end_pos)->extra & 0x1) == 0)
        ++open_control_count;
      else if(nodes.at(control_end_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_control_count;

    }

    ++control_end_pos;

  }

  --control_end_pos;

  node_sets control_sets = node_sets(nodes, control_start_pos + 1, control_end_pos);

  node_sets::const_iterator citr;
  for(citr = control_sets.begin(); citr != control_sets.end(); ++citr)
    if(nodes.at(citr->front())->name == "condition")
      break;

  if(citr == control_sets.end()) return "";

  std::string condition = "";
  for(node_set::const_iterator node_itr = citr->begin(); node_itr != citr->end(); ++node_itr)
    if(nodes.at(*node_itr)->is_text())
      condition += nodes.at(*node_itr)->content ? *nodes.at(*node_itr)->content : "";

  if(condition.size() > 0 && *--condition.end() == ';')
    condition.erase(--condition.end());

  return condition;

}

std::string get_condition(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->name == "for" || nodes.at(start_pos)->name == "foreach")
    return get_for_condition(nodes, start_pos);

  int condition_start_pos = start_pos;

  while(nodes.at(condition_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || nodes.at(condition_start_pos)->name != "condition")
    ++condition_start_pos;

  std::string condition = "";
  int open_condition_count = nodes.at(condition_start_pos)->extra & 0x1 ? 0 : 1;
  int condition_pos = condition_start_pos + 1;

  while(open_condition_count) {

    if(nodes.at(condition_pos)->name == "condition") {

      if(nodes.at(condition_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(condition_pos)->extra & 0x1) == 0)
        ++open_condition_count;
      else if(nodes.at(condition_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_condition_count;

    } else if(nodes.at(condition_pos)->is_text() && !nodes.at(condition_pos)->is_white_space()) {

      condition += nodes.at(condition_pos)->content ? *nodes.at(condition_pos)->content : "";

    }

    ++condition_pos;

  }

  if(condition.size() > 0 && *condition.begin() == '(')
    condition.erase(condition.begin());

  if(condition.size() > 0 && *--condition.end() == ')')
    condition.erase(--condition.end());

  return condition;

}

std::string get_function_type_name(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "function" && nodes.at(start_pos)->name != "function_decl"
      && nodes.at(start_pos)->name != "constructor" && nodes.at(start_pos)->name != "constructor_decl"
      && nodes.at(start_pos)->name != "destructor" && nodes.at(start_pos)->name != "destructor_decl")) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";


  return extract_name(nodes, start_pos);

}

std::string get_class_type_name(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "class" && nodes.at(start_pos)->name != "struct"
      && nodes.at(start_pos)->name != "union" && nodes.at(start_pos)->name == "enum")) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  return extract_name(nodes, start_pos);

}

bool conditional_has_block(const node_set & set) {

  node_sets sets = node_sets(set.nodes(), set.at(1), set.back());
  if(set.nodes().at(set.at(0))->name == "elseif") {

    for(node_sets::iterator itr = sets.begin(); itr != sets.end(); ++itr) {

      if(set.nodes().at(itr->at(0))->name == "if") {

        sets = node_sets(sets.nodes(), itr->at(1), itr->back());
        break;

      }

    }

  }

  for(node_sets::iterator itr = sets.begin(); itr != sets.end(); ++itr) {

    if(set.nodes().at(itr->at(0))->name == "block" && !bool(find_attribute(set.nodes().at(itr->at(0)), "type"))) {

      return true;

    } else if(set.nodes().at(itr->at(0))->name == "then") {

      int next_element_pos = itr->at(0) + 1;
      while(set.nodes().at(next_element_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT && set.nodes().at(next_element_pos)->type != (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        ++next_element_pos;

      if(set.nodes().at(next_element_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
        && set.nodes().at(next_element_pos)->name == "block"
        && !bool(find_attribute(set.nodes().at((next_element_pos)), "type")))
        return true;
      else
        return false;

    }

  }

  return false;

}

bool if_has_else(const node_set & set) {

  if(set.nodes().at(set.at(0))->name == "elseif") return false;

  node_sets sets = node_sets(set.nodes(), set.at(1), set.back());
  for(node_sets::iterator itr = sets.begin(); itr != sets.end(); ++itr) {

    if(set.nodes().at(itr->at(0))->name == "else" || set.nodes().at(itr->at(0))->name == "elseif") {

      return true;

    }

  }

  return false;

}

bool if_then_equal(const node_set & set_original, const node_set & set_modified) {

  diff_nodes dnodes = { set_original.nodes(), set_modified.nodes() };

  node_sets node_sets_original = node_sets(set_original.nodes(), set_original.at(1), set_original.back());
  node_sets node_sets_modified = node_sets(set_modified.nodes(), set_modified.at(1), set_modified.back());

  node_sets::iterator then_original;
  for(then_original = node_sets_original.begin(); then_original != node_sets_original.end(); ++then_original) {

    if(set_original.nodes().at(then_original->at(0))->name == "then") {

      break;

    }

  }

  if(then_original == node_sets_original.end()) return false;

  node_sets::iterator then_modified;
  for(then_modified = node_sets_modified.begin(); then_modified != node_sets_modified.end(); ++then_modified) {

    if(set_modified.nodes().at(then_modified->at(0))->name == "then") {

      break;

    }

  }

  if(then_modified == node_sets_modified.end()) return false;

  bool then_is_equal = srcdiff_compare::node_set_syntax_compare((void *)&*then_original, (void *)&*then_modified, (void *)&dnodes) == 0;

  return then_is_equal;

}

bool for_control_matches(const node_set & set_original, const node_set & set_modified) {

  diff_nodes dnodes = { set_original.nodes(), set_modified.nodes() };

  node_sets node_sets_original = node_sets(set_original.nodes(), set_original.at(1), set_original.back());
  node_sets node_sets_modified = node_sets(set_modified.nodes(), set_modified.at(1), set_modified.back());

  node_sets::size_type control_pos_original;
  for(control_pos_original = 0; control_pos_original < node_sets_original.size(); ++control_pos_original)
    if(set_original.nodes().at(node_sets_original.at(control_pos_original).front())->name == "control")
      break;

  node_sets::size_type control_pos_modified;
  for(control_pos_modified = 0; control_pos_modified < node_sets_modified.size(); ++control_pos_modified)
    if(set_modified.nodes().at(node_sets_modified.at(control_pos_modified).front())->name == "control")
      break;

  bool matches = control_pos_original != node_sets_original.size() && control_pos_modified != node_sets_modified.size() 
    && srcdiff_compare::node_set_syntax_compare((void *)&node_sets_original.at(control_pos_original), (void *)&node_sets_modified.at(control_pos_modified), (void *)&dnodes) == 0;

  return matches;

}

std::string get_case_expr(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || nodes.at(start_pos)->name != "case" || (nodes.at(start_pos)->extra & 0x1)) return "";

  // skip case tag and case text
  int expr_pos = start_pos + 1;

  if((nodes.at(expr_pos)->is_text() && nodes.at(expr_pos)->content && nodes.at(expr_pos)->content->find(':') != std::string::npos)
     || (nodes.at(expr_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == "case")) return "";

  while((nodes.at(expr_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || nodes.at(expr_pos)->name != "expr")
    && !(nodes.at(expr_pos)->is_text() && nodes.at(expr_pos)->content && nodes.at(expr_pos)->content->find(':') != std::string::npos)
    && !(nodes.at(expr_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == "case"))
    ++expr_pos;

  if((nodes.at(expr_pos)->is_text() && nodes.at(expr_pos)->content && nodes.at(expr_pos)->content->find(':') != std::string::npos)
    || (nodes.at(expr_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == "case")) return "";

  std::string case_expr = "";

  int open_expr_count = nodes.at(expr_pos)->extra & 0x1 ? 0 : 1;
  ++expr_pos;

  while(open_expr_count) {

    if(nodes.at(expr_pos)->name == "expr") {

      if(nodes.at(expr_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(expr_pos)->extra & 0x1) == 0)
        ++open_expr_count;
      else if(nodes.at(expr_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_expr_count;

    } else if(nodes.at(expr_pos)->is_text() && !nodes.at(expr_pos)->is_white_space()) {

      case_expr += nodes.at(expr_pos)->content ? *nodes.at(expr_pos)->content : "";

    }

    ++expr_pos;

  }

  return case_expr;

}

bool is_single_name_expr(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "expr_stmt" && nodes.at(start_pos)->name != "expr")) return false;

  if(nodes.at(start_pos)->extra & 0x1) return false;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name == "expr_stmt")
    ++start_pos;

  if(nodes.at(start_pos)->extra & 0x1) return false;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name == "expr")
    ++start_pos;

  if(nodes.at(start_pos)->name != "name") return false;

  int open_name_count = (nodes.at(start_pos)->extra & 0x1) ? 0 : 1;
  ++start_pos;

  while(open_name_count) {

    if(nodes.at(start_pos)->name == "name") {

      if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
        ++open_name_count;
      else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_name_count;

    }

    ++start_pos;

  }

  return nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(start_pos)->name == "expr";

}


node_set get_first_expr_child(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->extra & 0x1) return node_set(nodes);

  int expr_pos = start_pos;

  while((nodes.at(expr_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || nodes.at(expr_pos)->name != "expr")
    && !(nodes.at(expr_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == nodes.at(start_pos)->name))
    ++expr_pos;

  if(nodes.at(expr_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == nodes.at(start_pos)->name)
    return node_set(nodes);

  return node_set(nodes, expr_pos);

}

/*
  End internal heuristic functions for reject_match
*/

struct interchange_list {

  const char * const name;
  const char * const * list;

};

static const char * const class_interchange[]     = { "class", "struct", "union", "enum", 0 };
static const char * const access_interchange[]    = { "public", "protected", "private",   0 };
static const char * const if_interchange[]        = { "if", "while", "for", "foreach",    0 };
static const char * const else_interchange[]      = { "else", "elseif",                   0 };
static const char * const expr_stmt_interchange[] = { "expr_stmt", "decl_stmt", "return", 0 };
static const char * const cast_interchange[]      = { "cast", 0 };
static const interchange_list interchange_lists[] = {

  { "class",     class_interchange },
  { "struct",    class_interchange },
  { "union",     class_interchange },
  { "enum",      class_interchange },

  { "public",    access_interchange },
  { "protected", access_interchange },
  { "private",   access_interchange },

  { "if",        if_interchange },
  { "while",     if_interchange },
  { "for",       if_interchange },
  { "foreach",   if_interchange },
  
  { "else",      else_interchange },
  { "elseif",    else_interchange },

  {"expr_stmt", expr_stmt_interchange },
  {"decl_stmt", expr_stmt_interchange },
  {"return"   , expr_stmt_interchange },

  {"cast", cast_interchange },

  { 0, 0 }

};

bool srcdiff_match::is_interchangeable_match(const std::string & original_tag, const std::string & original_uri,
                                             const std::string & modified_tag, const std::string & modified_uri) {

  if(original_uri != modified_uri) return false;

  if(original_tag == "if" && original_uri != SRCML_SRC_NAMESPACE_HREF) return false;
  if(modified_tag == "if" && modified_uri != SRCML_SRC_NAMESPACE_HREF) return false;

  for(size_t list_pos = 0; interchange_lists[list_pos].name; ++list_pos) {

    if(interchange_lists[list_pos].name == original_tag) {

      for(size_t pos = 0; interchange_lists[list_pos].list[pos]; ++pos) {

        if(interchange_lists[list_pos].list[pos] == modified_tag)
          return true;

      }

    }

  }

  return false;

}

bool reject_match_same(const srcdiff_measure & measure,
                       const node_set & set_original,
                       const node_set & set_modified) {

  int original_pos = set_original.at(0);
  int modified_pos = set_modified.at(0);

  const std::string & original_tag = set_original.nodes().at(original_pos)->name;
  const std::string & modified_tag = set_modified.nodes().at(modified_pos)->name;

  const std::string & original_uri = set_original.nodes().at(original_pos)->ns.href;
  const std::string & modified_uri = set_modified.nodes().at(modified_pos)->ns.href;

  if(original_tag != modified_tag) return true;

  if(original_tag == "type" || original_tag == "then" || original_tag == "condition" || original_tag == "control" || original_tag == "init"
    || original_tag == "default" || original_tag == "comment"
    || original_tag == "private" || original_tag == "protected" || original_tag == "public" || original_tag == "signals"
    || original_tag == "parameter_list" || original_tag == "krparameter_list" || original_tag == "argument_list"
    || original_tag == "attribute_list" || original_tag == "association_list" || original_tag == "protocol_list"
    || original_tag == "member_init_list" || original_tag == "member_list"
    || original_tag == "argument"
    || original_tag == "range"
    || original_tag == "literal" || original_tag == "operator" || original_tag == "modifier"
    || original_tag == "number" || original_tag == "file")
    return false;

  if(original_tag == "name" && set_original.nodes().at(original_pos)->is_simple && set_modified.nodes().at(modified_pos)->is_simple) return false;
  if(original_tag == "name" && set_original.nodes().at(original_pos)->is_simple != set_modified.nodes().at(modified_pos)->is_simple) return true;

  if((original_tag == "expr" || original_tag == "expr_stmt") && measure.similarity() > 0 && measure.difference() <= measure.max_length()) return false;

  // may need to refine to if child only single name
  if(original_tag == "expr" && set_original.nodes().at(original_pos)->parent && (*set_original.nodes().at(original_pos)->parent)->name == "argument") return false;
  if(original_tag == "expr" && is_single_name_expr(set_original.nodes(), original_pos) && is_single_name_expr(set_modified.nodes(), modified_pos)) return false;

  if(original_tag == "block") {

    bool is_pseudo_original = bool(find_attribute(set_original.nodes().at(original_pos), "type"));
    bool is_pseudo_modified = bool(find_attribute(set_modified.nodes().at(modified_pos), "type"));

    if(is_pseudo_original == is_pseudo_modified) {

      return false;

    } else if(measure.similarity()) {

      bool is_reject = true;

      if(is_pseudo_original) {

        node_sets node_sets_original = node_sets(set_original.nodes(), set_original.at(1), set_original.back());
        node_sets node_sets_modified = node_sets(set_modified.nodes(), set_modified.at(0), set_modified.back() + 1);

        int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
        srcdiff_nested::check_nestable(node_sets_original, 0, node_sets_original.size(), node_sets_modified, 0, 1,
                      start_nest_original, end_nest_original, start_nest_modified , end_nest_modified, operation);

        is_reject = !(operation == SES_INSERT);

      } else {

        node_sets node_sets_original = node_sets(set_original.nodes(), set_original.at(0), set_original.back() + 1);
        node_sets node_sets_modified = node_sets(set_modified.nodes(), set_modified.at(1), set_modified.back());

        int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
        srcdiff_nested::check_nestable(node_sets_original, 0, 1, node_sets_modified, 0, node_sets_modified.size(),
                      start_nest_original, end_nest_original, start_nest_modified , end_nest_modified, operation);

        is_reject = !(operation == SES_DELETE);

      }

      return is_reject;

    }

  }

  if(original_tag == "call") {

    std::vector<std::string> original_names = get_call_name(set_original.nodes(), original_pos);
    std::vector<std::string> modified_names = get_call_name(set_modified.nodes(), modified_pos);

    if(name_list_similarity(original_names, modified_names)) return false;

  } else if(original_tag == "decl" || original_tag == "decl_stmt" || original_tag == "parameter" || original_tag == "param") {

    std::string original_name = get_decl_name(set_original.nodes(), original_pos);
    std::string modified_name = get_decl_name(set_modified.nodes(), modified_pos);

    if(original_name == modified_name && original_name != "") return false;

  } else if(original_tag == "function"    || original_tag == "function_decl"
         || original_tag == "constructor" || original_tag == "constructor_decl"
         || original_tag == "destructor"  || original_tag == "destructor_decl") {

    std::string original_name = get_function_type_name(set_original.nodes(), original_pos);
    std::string modified_name = get_function_type_name(set_modified.nodes(), modified_pos);

    if(original_name == modified_name) return false;

  } else if((original_tag == "if" || original_tag == "elseif") && original_uri == SRCML_SRC_NAMESPACE_HREF) {

    std::string original_condition = get_condition(set_original.nodes(), original_pos);
    std::string modified_condition = get_condition(set_modified.nodes(), modified_pos);

    bool original_has_block = conditional_has_block(set_original);
    bool modified_has_block = conditional_has_block(set_modified);

    bool original_has_else = if_has_else(set_original);
    bool modified_has_else = if_has_else(set_modified);

    if(if_then_equal(set_original, set_modified) || (original_condition == modified_condition
      && (original_has_block == modified_has_block || original_has_else == modified_has_else || ((original_has_block || !original_has_else) && (modified_has_block || !modified_has_else)))))
     return false;

  } else if(original_tag == "while" || original_tag == "switch" || original_tag == "do") {

    std::string original_condition = get_condition(set_original.nodes(), original_pos);
    std::string modified_condition = get_condition(set_modified.nodes(), modified_pos);

    if(original_condition == modified_condition) return false;

  } else if(original_tag == "for" || original_tag == "foreach") {

    if(for_control_matches(set_original, set_modified))
      return false;

  } else if(original_tag == "case") { 

    std::string original_expr = get_case_expr(set_original.nodes(), original_pos);
    std::string modified_expr = get_case_expr(set_modified.nodes(), modified_pos);

    if(original_expr == modified_expr) return false;

  } else if(original_tag == "class" || original_tag == "struct" || original_tag == "union" || original_tag == "enum") {

    std::string original_name = get_class_type_name(set_original.nodes(), original_pos);
    std::string modified_name = get_class_type_name(set_modified.nodes(), modified_pos);

    if(original_name == modified_name && original_name != "") return false;

  }

  bool is_reject = srcdiff_match::reject_similarity(measure, set_original, set_modified);
  return is_reject;

}

bool reject_match_interchangeable(const srcdiff_measure & measure,
                                  const node_set & set_original,
                                  const node_set & set_modified) {

  int original_pos = set_original.at(0);
  int modified_pos = set_modified.at(0);

  const std::string & original_tag = set_original.nodes().at(original_pos)->name;
  const std::string & modified_tag = set_modified.nodes().at(modified_pos)->name;

  const std::string & original_uri = set_original.nodes().at(original_pos)->ns.href;
  const std::string & modified_uri = set_modified.nodes().at(modified_pos)->ns.href;

  std::string original_name;
  if(original_tag == "class" || original_tag == "struct" || original_tag == "union" || original_tag == "enum") {

      original_name = get_class_type_name(set_original.nodes(), original_pos);

  }

  std::string modified_name;
  if(modified_tag == "class" || modified_tag == "struct" || modified_tag == "union" || modified_tag == "enum") {

      modified_name = get_class_type_name(set_original.nodes(), original_pos);
    
  }

  if(original_name != "" && original_name == modified_name) return false;

  std::string original_condition;
  if(original_tag == "if" || original_tag == "while" || original_tag == "for" || original_tag == "foreach") {

    original_condition = get_condition(set_original.nodes(), original_pos);

  }

  std::string modified_condition;
  if(modified_tag == "if" || modified_tag == "while" || modified_tag == "for" || modified_tag == "foreach") {

    modified_condition = get_condition(set_modified.nodes(), modified_pos);

  }

  if(original_condition != "" && original_condition == modified_condition) return false;


  if(  (original_tag == "expr_stmt" || original_tag == "decl_stmt" || original_tag == "return")
    && (modified_tag == "expr_stmt" || modified_tag == "decl_stmt" || modified_tag == "return")) {

    node_set expr_original(set_original.nodes());
    node_set expr_modified(set_modified.nodes());
    if(original_tag == "decl_stmt" || modified_tag == "decl_stmt") {

      if(original_tag == "decl_stmt") {

        expr_modified = get_first_expr_child(set_modified.nodes(), modified_pos);

        if(!expr_modified.empty()) {

          node_sets sets = node_sets(set_original.nodes(), set_original.at(1), set_original.back(), srcdiff_nested::is_match,
                                    &set_modified.nodes().at(expr_modified.at(0)));
          int match = srcdiff_nested::best_match(sets, expr_modified);

          if(match < sets.size())
            expr_original = sets.at(match);

        }

      } else {

        expr_original = get_first_expr_child(set_original.nodes(), original_pos);

        if(!expr_original.empty()) {

          node_sets sets = node_sets(set_modified.nodes(), set_modified.at(1), set_modified.back(), srcdiff_nested::is_match,
                                    &set_original.nodes().at(expr_original.at(0)));
          int match = srcdiff_nested::best_match(sets, expr_original);

          if(match < sets.size())
            expr_modified = sets.at(match);

        }

      }

    } else {

      expr_original = get_first_expr_child(set_original.nodes(), original_pos);
      expr_modified = get_first_expr_child(set_modified.nodes(), modified_pos);

    }

    if(expr_original.size() && expr_modified.size()) {

      srcdiff_text_measure expr_measure(expr_original, expr_modified);
      expr_measure.compute();

      bool is_expr_reject = srcdiff_match::reject_similarity(expr_measure, expr_original, expr_modified);

      int min_size = expr_measure.min_length();
      int max_size = expr_measure.max_length();

      if(!is_expr_reject && 2 * expr_measure.similarity() > max_size && 2 * expr_measure.difference() < max_size) return false;

    }

  }

  bool is_reject = srcdiff_match::reject_similarity(measure, set_original, set_modified);
  return is_reject;

}

bool srcdiff_match::reject_match(const srcdiff_measure & measure,
                                 const node_set & set_original,
                                 const node_set & set_modified) {

  /** if different prefix should not reach here, however, may want to add that here */
  int original_pos = set_original.at(0);
  int modified_pos = set_modified.at(0);

  const std::string & original_tag = set_original.nodes().at(original_pos)->name;
  const std::string & modified_tag = set_modified.nodes().at(modified_pos)->name;

  const std::string & original_uri = set_original.nodes().at(original_pos)->ns.href;
  const std::string & modified_uri = set_modified.nodes().at(modified_pos)->ns.href;

  if(original_tag == modified_tag && original_uri == modified_uri)
    return reject_match_same(measure, set_original, set_modified);
  else if(is_interchangeable_match(original_tag, original_uri, modified_tag, modified_uri)) 
    return reject_match_interchangeable(measure, set_original, set_modified);
  else
    return true;

}

bool srcdiff_match::reject_similarity(const srcdiff_measure & measure,
                                      const node_set & set_original,
                                      const node_set & set_modified) {

  if(set_original.size() == 1 && set_modified.size() == 1) {

    const std::string & original_tag = set_original.nodes().at(set_original.front())->name;
    const std::string & modified_tag = set_modified.nodes().at(set_modified.front())->name;

    return original_tag != modified_tag;

  }

  if(set_original.size() == 1 || set_modified.size() == 1) {

    /** @todo need to handle this some time */
    return true;

  }

  srcdiff_syntax_measure syntax_measure(set_original, set_modified);
  syntax_measure.compute();

  int min_child_length = syntax_measure.min_length();
  int max_child_length = syntax_measure.max_length();

  if(min_child_length > 1) { 

    if(2 * syntax_measure.similarity() >= min_child_length && syntax_measure.difference() <= min_child_length)
      return false;

  }

  node_sets child_node_sets_original = node_sets(set_original.nodes(), set_original.at(1), set_original.back());
  node_sets child_node_sets_modified = node_sets(set_modified.nodes(), set_modified.at(1), set_modified.back());    

  if(!child_node_sets_original.empty() && set_original.nodes().at(child_node_sets_original.back().at(0))->name == "then") {

    node_sets temp = node_sets(set_original.nodes(), child_node_sets_original.back().at(1), child_node_sets_original.back().back());
    child_node_sets_original = temp;

  }

  if(!child_node_sets_modified.empty() && set_modified.nodes().at(child_node_sets_modified.back().at(0))->name == "then") {

    node_sets temp = node_sets(set_modified.nodes(), child_node_sets_modified.back().at(1), child_node_sets_modified.back().back());
    child_node_sets_modified = temp;

  }

  if(!child_node_sets_original.empty() && !child_node_sets_modified.empty()
    && set_original.nodes().at(child_node_sets_original.back().at(0))->name == "block" && set_modified.nodes().at(child_node_sets_modified.back().at(0))->name == "block") {

    srcdiff_syntax_measure syntax_measure(child_node_sets_original.back(), child_node_sets_modified.back());
    syntax_measure.compute();

    int min_child_length = syntax_measure.min_length();
    int max_child_length = syntax_measure.max_length();

    if(min_child_length > 1) { 

      if(2 * syntax_measure.similarity() >= min_child_length && syntax_measure.difference() <= min_child_length)
        return false;

    }

  }

  int min_size = measure.min_length();
  int max_size = measure.max_length();

#if DEBUG
  std::cerr << "Similarity: " << measure.similarity() << '\n';
  std::cerr << "Difference: " << measure.difference() << '\n';
  std::cerr << "Min Size: "   << min_size   << '\n';
  std::cerr << "Max Size: "   << max_size   << '\n';
#endif

  /** @todo consider making this configurable.  That is, allow user to specify file or have default file to read from */
  if(measure.difference() != 0 && measure.similarity() == 0) return true;

  if(min_size == measure.similarity() && measure.difference() < 2 * min_size) return false;
  if(min_size < 30 && measure.difference() > 1.25 * min_size)       return true;
  else if(min_size >= 30 && measure.difference() > 1.05 * min_size) return true;
  if(measure.difference() > max_size)                               return true;

  if(min_size <= 2)                return 2  * measure.similarity() <     min_size;
  else if(min_size <= 3)           return 3  * measure.similarity() < 2 * min_size;
  else if(min_size <= 30)          return 10 * measure.similarity() < 7 * min_size;
  else                             return 2  * measure.similarity() <     min_size;

}


bool srcdiff_match::reject_similarity_match_only(const node_set & set_original,
                                                 const node_set & set_modified) {

  int original_pos = set_original.at(0);
  int modified_pos = set_modified.at(0);

  if(srcdiff_compare::node_compare(set_original.nodes().at(original_pos),
                                  set_modified.nodes().at(modified_pos)) != 0)
    return true;

  srcdiff_text_measure complete_measure(set_original, set_modified, false);
  complete_measure.compute();
  int min_size = complete_measure.min_length();

  if(min_size == 0) return true;

  return min_size != complete_measure.similarity();

}
