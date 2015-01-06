#include <srcdiff_match.hpp>

#include <srcdiff_nested.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>

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

srcdiff_match::srcdiff_match(const std::vector<srcml_node *> & nodes_old, const std::vector<srcml_node *> & nodes_new, const node_sets & node_sets_old, const node_sets & node_sets_new)
  : nodes_old(nodes_old), nodes_new(nodes_new), node_sets_old(node_sets_old), node_sets_new(node_sets_new) {}

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

      match->old_offset = differences[i * olength + j].opos;
      match->new_offset = differences[i * olength + j].npos;
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

  /*
    fprintf(stderr, "HERE\n");

    for(int old_pos = 0; old_pos < edits->length; ++old_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_old.at(node_sets_old.at(edits->offset_sequence_one + old_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");

    for(int new_pos = 0; new_pos < edit_next->length; ++new_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_sets_new.at(edit_next->offset_sequence_two + new_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");
*/

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  int olength = node_sets_old.size();
  int nlength = node_sets_new.size();

  size_t mem_size = olength * nlength * sizeof(difference);

  difference * differences = (difference *)malloc(mem_size);

  // still need to figure out how to track matching on each path
  for(int i = 0; i < nlength; ++i) {

    for(int j = 0; j < olength; ++j) {

      srcdiff_measure measure(nodes_old, nodes_new, node_sets_old.at(j), node_sets_new.at(i));
      int similarity, difference, text_old_length, text_new_length;
      measure.compute_measures(similarity, difference, text_old_length, text_new_length);

      //unsigned long long max_similarity = (unsigned long long)-1;
      int max_similarity = -1;
      int unmatched = 0;

      // check if unmatched
      if(similarity == MAX_INT 
        || reject_match(similarity, difference, text_old_length, text_new_length,
          nodes_old, node_sets_old.at(j), nodes_new, node_sets_new.at(i))
        || srcdiff_nested::is_better_nested(nodes_old, node_sets_old, j, nodes_new, node_sets_new, i,
            similarity, difference, text_old_length, text_new_length)) {

        similarity = 0;
        unmatched = 1;

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
        int temp_num_unmatched = i + j + (unmatched ? 2 : 0);

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
        int temp_num_unmatched_match = i + j + (unmatched ? 2 : 0);


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
        int temp_num_unmatched = differences[(i - 1) * olength + (j - 1)].num_unmatched + (unmatched ? 2 : 0);

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity > max_similarity)) {

          matched = !unmatched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;
          direction = 3;

        }

      }

      if(direction == 1) {
        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        //differences[i * olength + (j - 1)].marked = marked_left;
        //differences[i * olength + (j - 1)].last_similarity = last_similarity_left;

      } else if(direction == 2) {
        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        //differences[(i - 1) * olength + j].marked = marked_top;
        //differences[(i - 1) * olength + j].last_similarity = last_similarity_top;

      } else {

        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

      }

      // special case starting node
      if(i == 0 && j == 0) {

        max_similarity = similarity;
        num_unmatched = unmatched;
        if(unmatched)
          ++num_unmatched;

        matched = !unmatched;
      }

      // set if marked
      if(matched) {

        differences[i * olength + j].marked = true;

      } else {

        differences[i * olength + j].marked = false;

      }

      /*
      fprintf(stderr, "HERE\n");
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, matched);
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, num_unmatched);
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, max_similarity);
      fprintf(stderr, "HERE\n");
      */

      // update structure
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


boost::optional<std::string> find_attribute(const srcml_node * node, const char * attr_name) {

  srcml_attr * attr = node->properties;

  for(; attr && attr->name && *attr->name != attr_name; attr = attr->next)
    ;

  if(attr)
    return attr->value;

  return 0;

}

/*
  Begin internal heuristic functions for reject_match
*/


bool is_single_call_expr(const std::vector<srcml_node *> & nodes, int start_pos) {

  if(!nodes.at(start_pos)->name) return false;

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || !nodes.at(start_pos)->name
    || (*nodes.at(start_pos)->name != "expr_stmt" && *nodes.at(start_pos)->name != "expr")) return false;

  if(nodes.at(start_pos)->extra & 0x1) return false;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "expr_stmt")
    ++start_pos;

  if(nodes.at(start_pos)->extra & 0x1) return false;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "expr")
    ++start_pos;

  if(!nodes.at(start_pos)->name || *nodes.at(start_pos)->name != "call") return false;

  int open_call_count = (nodes.at(start_pos)->extra & 0x1) ? 0 : 1;
  ++start_pos;

  while(open_call_count) {

    if(nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "call") {

      if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
        ++open_call_count;
      else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_call_count;

    }

    ++start_pos;

  }

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "expr")
    return true;

  return false;
 
}

std::string get_name(const std::vector<srcml_node *> & nodes, int name_start_pos) {

  int open_name_count = nodes.at(name_start_pos)->extra & 0x1 ? 0 : 1;
  int name_pos = name_start_pos + 1;
  std::string name = "";

  while(open_name_count) {

    if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(name_pos)->name && *nodes.at(name_pos)->name == "argument_list") return name;

    if(nodes.at(name_pos)->name && *nodes.at(name_pos)->name == "name") {

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

void skip_type(const std::vector<srcml_node *> & nodes, int & start_pos) {

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
   && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "decl_stmt")
    ++start_pos;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
   && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "parameter")
    ++start_pos;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
   && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "param")
    ++start_pos;

  if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
   && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "decl")
    ++start_pos;

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || !nodes.at(start_pos)->name || *nodes.at(start_pos)->name != "type")
    return;

  int open_type_count = nodes.at(start_pos)->extra & 0x1 ? 0 : 1;
  ++start_pos;

  while(open_type_count) {

    if(nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "type") {

      if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
        ++open_type_count;
      else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        --open_type_count;

    }

    ++start_pos;

  }

}

void skip_specifiers(const std::vector<srcml_node *> & nodes, int & start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || !nodes.at(start_pos)->name || *nodes.at(start_pos)->name != "specifier")
      return;

  while(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
    && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "specifier") {

    int open_specifier_count = nodes.at(start_pos)->extra & 0x1 ? 0 : 1;
    ++start_pos;

    while(open_specifier_count) {

      if(nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "specifier") {

        if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && (nodes.at(start_pos)->extra & 0x1) == 0)
          ++open_specifier_count;
        else if(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT)
          --open_specifier_count;

      }

      ++start_pos;

    }

  }

}

std::vector<std::string> get_call_name(const std::vector<srcml_node *> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || !nodes.at(start_pos)->name || *nodes.at(start_pos)->name != "call")
    return std::vector<std::string>();

  if(nodes.at(start_pos)->extra & 0x1) return std::vector<std::string>();

  int name_start_pos = start_pos + 1;

  while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || !nodes.at(name_start_pos)->name || (*nodes.at(name_start_pos)->name != "name" && *nodes.at(name_start_pos)->name != "argument_list"))
    ++name_start_pos;

  if(nodes.at(name_start_pos)->name && *nodes.at(name_start_pos)->name == "argument_list") return std::vector<std::string>();

  std::vector<std::string> name_list;

  int open_name_count = nodes.at(name_start_pos)->extra & 0x1 ? 0 : 1;
  int name_pos = name_start_pos + 1;
  std::string name = "";

  while(open_name_count) {

    if(nodes.at(name_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(name_pos)->name && *nodes.at(name_pos)->name == "argument_list") return name_list;

    if(nodes.at(name_pos)->name && *nodes.at(name_pos)->name == "name") {

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

int name_list_similarity(std::vector<std::string> name_list_old, std::vector<std::string> name_list_new) {

  class shortest_edit_script ses(srcdiff_compare::string_compare, srcdiff_compare::string_index, 0);

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

std::string get_decl_name(const std::vector<srcml_node *> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || !nodes.at(start_pos)->name
    || (*nodes.at(start_pos)->name != "decl_stmt"
      && *nodes.at(start_pos)->name != "parameter"
      && *nodes.at(start_pos)->name != "param"
      && *nodes.at(start_pos)->name != "decl")) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  int name_start_pos = start_pos + 1;

  skip_type(nodes, name_start_pos);

  while(!(nodes.at(name_start_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
      && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "name")
    && !(nodes.at(name_start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
      && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "decl"))
    ++name_start_pos;

  if(nodes.at(name_start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
      && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "decl")
    return "";

  return get_name(nodes, name_start_pos);

}

std::string get_for_condition(const std::vector<srcml_node *> & nodes, int start_pos) {

  int control_start_pos = start_pos;

  while(nodes.at(control_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || !nodes.at(start_pos)->name || *nodes.at(start_pos)->name != "control")
    ++control_start_pos;

  if(nodes.at(control_start_pos)->extra & 0x1) return "";

  int control_end_pos = control_start_pos + 1;
  int open_control_count = 1;

  while(open_control_count) {

    if(nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "control") {

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
    if(nodes.at(citr->front())->name && *nodes.at(citr->front())->name == "condition")
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

std::string get_condition(const std::vector<srcml_node *> & nodes, int start_pos) {

  if(nodes.at(start_pos)->name && (*nodes.at(start_pos)->name == "for" || *nodes.at(start_pos)->name == "foreach"))
    return get_for_condition(nodes, start_pos);

  int condition_start_pos = start_pos;

  while(nodes.at(condition_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || !nodes.at(condition_start_pos)->name || *nodes.at(condition_start_pos)->name != "condition")
    ++condition_start_pos;

  std::string condition = "";
  int open_condition_count = nodes.at(condition_start_pos)->extra & 0x1 ? 0 : 1;
  int condition_pos = condition_start_pos + 1;

  while(open_condition_count) {

    if(nodes.at(condition_pos)->name && *nodes.at(condition_pos)->name == "condition") {

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

std::string get_function_type_name(const std::vector<srcml_node *> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || !nodes.at(start_pos)->name || (*nodes.at(start_pos)->name != "function" && *nodes.at(start_pos)->name != "function_decl"
      && *nodes.at(start_pos)->name != "constructor" && *nodes.at(start_pos)->name != "constructor_decl"
      && *nodes.at(start_pos)->name != "destructor" && *nodes.at(start_pos)->name != "destructor_decl")) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  int name_start_pos = start_pos + 1;

  /** @todo this is wrong */
  if(!nodes.at(start_pos)->name || (*nodes.at(start_pos)->name != "function" || *nodes.at(start_pos)->name != "function_decl"))
    skip_type(nodes, name_start_pos);
  else
    skip_specifiers(nodes, ++name_start_pos);

  while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || !nodes.at(name_start_pos)->name || *nodes.at(name_start_pos)->name != "name")
    ++name_start_pos;

  return get_name(nodes, name_start_pos);

}

std::string get_class_type_name(const std::vector<srcml_node *> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || nodes.at(start_pos)->name || (*nodes.at(start_pos)->name != "class" && *nodes.at(start_pos)->name != "struct"
      && *nodes.at(start_pos)->name != "union" && *nodes.at(start_pos)->name == "enum")) return "";
  if(nodes.at(start_pos)->extra & 0x1) return "";

  int name_start_pos = start_pos + 1;

  while(nodes.at(name_start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
   || !nodes.at(name_start_pos)->name || (*nodes.at(name_start_pos)->name !=  "name"
    && *nodes.at(name_start_pos)->name !=  "block"))
    ++name_start_pos;

  if(nodes.at(name_start_pos)->name && *nodes.at(name_start_pos)->name == "name")
    return get_name(nodes, name_start_pos);
  else
    return "";

}

bool conditional_has_block(const std::vector<srcml_node *> & nodes, const node_set & set) {

  node_sets sets = node_sets(nodes, set.at(1), set.back());

  for(node_sets::iterator itr = sets.begin(); itr != sets.end(); ++itr) {

    if(nodes.at(itr->at(0))->name && *nodes.at(itr->at(0))->name == "block" && find_attribute(nodes.at(itr->at(0)), "type") == 0) {

      return true;

    } else if(nodes.at(itr->at(0))->name && *nodes.at(itr->at(0))->name == "then") {

      int next_element_pos = itr->at(0) + 1;
      while(nodes.at(next_element_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT && nodes.at(next_element_pos)->type != (xmlElementType)XML_READER_TYPE_END_ELEMENT)
        ++next_element_pos;

      if(nodes.at(next_element_pos)->type == (xmlElementType)XML_READER_TYPE_ELEMENT
        && nodes.at(next_element_pos)->name && *nodes.at(next_element_pos)->name == "block"
        && find_attribute(nodes.at((next_element_pos)), "type") == 0)
        return true;
      else
        return false;

    }

  }

  return false;

}

bool if_has_else(const std::vector<srcml_node *> & nodes, const node_set & set) {

  node_sets sets = node_sets(nodes, set.at(1), set.back());

  for(node_sets::iterator itr = sets.begin(); itr != sets.end(); ++itr) {

    if(nodes.at(itr->at(0))->name && (*nodes.at(itr->at(0))->name == "else" || *nodes.at(itr->at(0))->name == "elseif")) {

      return true;

    }

  }

  return false;

}

bool if_then_equal(const std::vector<srcml_node *> & nodes_old, const node_set & set_old, const std::vector<srcml_node *> & nodes_new, const node_set & set_new) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  node_sets node_sets_old = node_sets(nodes_old, set_old.at(1), set_old.back());
  node_sets node_sets_new = node_sets(nodes_new, set_new.at(1), set_new.back());

  node_sets::iterator then_old;
  for(then_old = node_sets_old.begin(); then_old != node_sets_old.end(); ++then_old) {

    if(nodes_old.at(then_old->at(0))->name && *nodes_old.at(then_old->at(0))->name == "then") {

      break;

    }

  }

  node_sets::iterator then_new;
  for(then_new = node_sets_new.begin(); then_new != node_sets_new.end(); ++then_new) {

    if(nodes_new.at(then_new->at(0))->name && *nodes_new.at(then_new->at(0))->name == "then") {

      break;

    }

  }

  bool then_is_equal = srcdiff_compare::node_set_syntax_compare((void *)&*then_old, (void *)&*then_new, (void *)&dnodes) == 0;

  return then_is_equal;

}

bool for_control_matches(const std::vector<srcml_node *> & nodes_old, const node_set & set_old, const std::vector<srcml_node *> & nodes_new, const node_set & set_new) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  node_sets node_sets_old = node_sets(nodes_old, set_old.at(1), set_old.back());
  node_sets node_sets_new = node_sets(nodes_new, set_new.at(1), set_new.back());

  node_sets::size_type control_pos_old;
  for(control_pos_old = 0; control_pos_old < node_sets_old.size(); ++control_pos_old)
    if(nodes_old.at(node_sets_old.at(control_pos_old).front())->name && *nodes_old.at(node_sets_old.at(control_pos_old).front())->name == "control")
      break;

  node_sets::size_type control_pos_new;
  for(control_pos_new = 0; control_pos_new < node_sets_new.size(); ++control_pos_new)
    if(nodes_new.at(node_sets_new.at(control_pos_new).front())->name && *nodes_new.at(node_sets_new.at(control_pos_new).front())->name == "control")
      break;

  bool matches = control_pos_old != node_sets_old.size() && control_pos_new != node_sets_new.size() 
    && srcdiff_compare::node_set_syntax_compare((void *)&node_sets_old.at(control_pos_old), (void *)&node_sets_new.at(control_pos_new), (void *)&dnodes) == 0;

  return matches;

}

std::string get_case_expr(const std::vector<srcml_node *> & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
    || !nodes.at(start_pos)->name || *nodes.at(start_pos)->name == "case" || (nodes.at(start_pos)->extra & 0x1)) return "";

  // skip case tag and case text
  int expr_pos = start_pos + 1;

  if((nodes.at(expr_pos)->is_text() && nodes.at(expr_pos)->content && nodes.at(expr_pos)->content->find(':') != std::string::npos)
     || (nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "case")) return "";

  while((nodes.at(expr_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT || !nodes.at(expr_pos)->name || * nodes.at(expr_pos)->name != "expr")
    && !(nodes.at(expr_pos)->is_text() && nodes.at(expr_pos)->content && nodes.at(expr_pos)->content->find(':') != std::string::npos)
    && !(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "case"))
    ++expr_pos;

  if((nodes.at(expr_pos)->is_text() && nodes.at(expr_pos)->content && nodes.at(expr_pos)->content->find(':') != std::string::npos)
    || (nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT && nodes.at(start_pos)->name && *nodes.at(start_pos)->name == "case")) return "";

  std::string case_expr = "";

  int open_expr_count = nodes.at(expr_pos)->extra & 0x1 ? 0 : 1;
  ++expr_pos;

  while(open_expr_count) {

    if(nodes.at(expr_pos)->name && *nodes.at(expr_pos)->name == "expr") {

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

bool srcdiff_match::is_interchangeable_match(const boost::optional<std::string> & old_tag, const boost::optional<std::string> & new_tag) {

  if(!old_tag || !new_tag) return false;

  for(size_t list_pos = 0; interchange_lists[list_pos].name; ++list_pos) {

    if(interchange_lists[list_pos].name == *old_tag) {

      for(size_t pos = 0; interchange_lists[list_pos].list[pos]; ++pos) {

        if(interchange_lists[list_pos].list[pos] == *new_tag)
          return true;

      }


    }

  }

  return false;

}

bool reject_match_same(int similarity, int difference, int text_old_length, int text_new_length,
  const std::vector<srcml_node *> & nodes_old, const node_set & set_old, const std::vector<srcml_node *> & nodes_new, const node_set & set_new) {

  int old_pos = set_old.at(0);
  int new_pos = set_new.at(0);

  std::string old_tag = nodes_old.at(old_pos)->name ? *nodes_old.at(old_pos)->name : "";
  std::string new_tag = nodes_new.at(new_pos)->name ? *nodes_new.at(new_pos)->name : "";

  if(old_tag != new_tag) return true;

  if(old_tag == "name" || old_tag == "type" || old_tag == "then" || old_tag == "condition" || old_tag == "init"
    || old_tag == "default" || old_tag == "comment"
    || old_tag == "private" || old_tag == "protected" || old_tag == "public" || old_tag == "signals"
    || old_tag == "parameter_list" || old_tag == "krparameter_list" || old_tag == "argument_list" || old_tag == "member_init_list"
    || old_tag == "attribute_list" || old_tag == "association_list" || old_tag == "protocol_list"
    || old_tag == "argument"
    || old_tag == "literal" || old_tag == "operator" || old_tag == "modifier" || old_tag == "member_list")
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

        node_sets node_sets_old = node_sets(nodes_old, set_old.at(1), set_old.back());
        node_sets node_sets_new = node_sets(nodes_new, set_new.at(0), set_new.back() + 1);

        int start_nest_old, end_nest_old, start_nest_new, end_nest_new, operation;
        srcdiff_nested::check_nestable(node_sets_old, nodes_old, 0, node_sets_old.size(), node_sets_new, nodes_new, 0, 1,
                      start_nest_old, end_nest_old, start_nest_new , end_nest_new, operation);


        is_reject = !(operation == SESINSERT);

      } else {

        node_sets node_sets_old = node_sets(nodes_old, set_old.at(0), set_old.back() + 1);
        node_sets node_sets_new = node_sets(nodes_new, set_new.at(1), set_new.back());

        int start_nest_old, end_nest_old, start_nest_new, end_nest_new, operation;
        srcdiff_nested::check_nestable(node_sets_old, nodes_old, 0, 1, node_sets_new, nodes_new, 0, node_sets_new.size(),
                      start_nest_old, end_nest_old, start_nest_new , end_nest_new, operation);


        is_reject = !(operation == SESDELETE);

      }

      return is_reject;

    }

  }

  if(is_single_call_expr(nodes_old, old_pos) && is_single_call_expr(nodes_new, new_pos)) {

    while(nodes_old.at(old_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
      || !nodes_old.at(old_pos)->name || *nodes_old.at(old_pos)->name != "call")
      ++old_pos;

    while(nodes_new.at(new_pos)->type != (xmlElementType)XML_READER_TYPE_ELEMENT
      || !nodes_new.at(new_pos)->name || *nodes_new.at(new_pos)->name != "call")
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

    if(srcdiff_compare::node_compare(nodes_old.at(old_pos), nodes_new.at(new_pos)) == 0) return false;

  }

  bool is_reject = srcdiff_match::reject_similarity(similarity, difference, text_old_length, text_new_length, nodes_old, set_old, nodes_new, set_new);
  return is_reject;

}

bool reject_match_interchangeable(int similarity, int difference, int text_old_length, int text_new_length,
  const std::vector<srcml_node *> & nodes_old, const node_set & set_old, const std::vector<srcml_node *> & nodes_new, const node_set & set_new) {

  int old_pos = set_old.at(0);
  int new_pos = set_new.at(0);

  std::string old_tag = nodes_old.at(old_pos)->name ? *nodes_old.at(old_pos)->name : "";
  std::string new_tag = nodes_new.at(new_pos)->name ? *nodes_new.at(new_pos)->name : "";

  std::string old_condition = "";
  if(old_tag == "if" || old_tag == "while" || old_tag == "for" || old_tag == "foreach") {

    old_condition = get_condition(nodes_old, old_pos);

  }

  std::string new_condition = "";
  if(new_tag == "if" || new_tag == "while" || new_tag == "for" || new_tag == "foreach") {

    new_condition = get_condition(nodes_new, new_pos);

  }

  if(old_condition != "" && old_condition == new_condition) return false;

  bool is_reject = srcdiff_match::reject_similarity(similarity, difference, text_old_length, text_new_length, nodes_old, set_old, nodes_new, set_new);
  return is_reject;

}

bool srcdiff_match::reject_match(int similarity, int difference, int text_old_length, int text_new_length,
  const std::vector<srcml_node *> & nodes_old, const node_set & set_old, const std::vector<srcml_node *> & nodes_new, const node_set & set_new) {

  /** if different prefix should not reach here, however, may want to add that here */
  int old_pos = set_old.at(0);
  int new_pos = set_new.at(0);

  std::string old_tag = nodes_old.at(old_pos)->name ? *nodes_old.at(old_pos)->name : "";
  std::string new_tag = nodes_new.at(new_pos)->name ? *nodes_new.at(new_pos)->name : "";

  if(old_tag == new_tag)
    return reject_match_same(similarity, difference, text_old_length, text_new_length, nodes_old, set_old, nodes_new, set_new);
  else if(is_interchangeable_match(old_tag, new_tag)) 
    return reject_match_interchangeable(similarity, difference, text_old_length, text_new_length, nodes_old, set_old, nodes_new, set_new);
  else
    return true;

}

bool srcdiff_match::reject_similarity(int similarity, int difference, int text_old_length, int text_new_length,
  const std::vector<srcml_node *> & nodes_old, const node_set & set_old, const std::vector<srcml_node *> & nodes_new, const node_set & set_new) {

  srcdiff_measure measure(nodes_old, nodes_new, set_old, set_new);
  int syntax_similarity, syntax_difference, children_length_old, children_length_new;
  measure.compute_syntax_measures(syntax_similarity, syntax_difference, children_length_old, children_length_new);

  int min_child_length = children_length_old < children_length_new ? children_length_old : children_length_new;
  int max_child_length = children_length_old < children_length_new ? children_length_new : children_length_old;

  if(min_child_length > 1) { 

    if(2 * syntax_similarity >= min_child_length && syntax_difference <= min_child_length)
      return false;

  }

  node_sets child_node_sets_old = node_sets(nodes_old, set_old.at(1), set_old.back());
  node_sets child_node_sets_new = node_sets(nodes_new, set_new.at(1), set_new.back());    

  if(nodes_old.at(child_node_sets_old.back().at(0))->name && *nodes_old.at(child_node_sets_old.back().at(0))->name == "then") {

    node_sets temp = node_sets(nodes_old, child_node_sets_old.back().at(1), child_node_sets_old.back().back());
    child_node_sets_old = temp;

  }

  if(nodes_new.at(child_node_sets_new.back().at(0))->name && *nodes_new.at(child_node_sets_new.back().at(0))->name == "then") {

    node_sets temp = node_sets(nodes_new, child_node_sets_new.back().at(1), child_node_sets_new.back().back());
    child_node_sets_new = temp;

  }

  if(nodes_old.at(child_node_sets_old.back().at(0))->name && *nodes_old.at(child_node_sets_old.back().at(0))->name == "block"
    && nodes_new.at(child_node_sets_new.back().at(0))->name && *nodes_new.at(child_node_sets_new.back().at(0))->name == "block") {

    srcdiff_measure measure(nodes_old, nodes_new, child_node_sets_old.back(), child_node_sets_new.back());
    measure.compute_syntax_measures(syntax_similarity, syntax_difference, children_length_old, children_length_new);

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
