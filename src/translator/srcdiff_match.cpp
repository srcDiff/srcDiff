#include <srcdiff_match.hpp>

#include <srcdiff_nested.hpp>
#include <srcdiff_text_measure.hpp>
#include <srcdiff_syntax_measure.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>
#include <srcdiff_match_internal.hpp>


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

bool srcdiff_match::is_match_default(const construct_list & sets_original, int start_pos_original,
                                     const construct_list & sets_modified, int start_pos_modified,
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


srcdiff_match::srcdiff_match(const construct_list & construct_list_original, const construct_list & construct_list_modified,
                             const is_match_func & is_match)
  : construct_list_original(construct_list_original), construct_list_modified(construct_list_modified),
    is_match(is_match) {}

/** loop O(D) */
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

/** loop O(RD^2) */
offset_pair * srcdiff_match::match_differences() {

  /*

    Possible dynamic programming solution.  Sum similarities choosing left top or diagonal,
    picking smallest.  Unmatching has a high cost 1 less than a syntax mismatch.  So, it is chosen over
    a syntax mismatch.  Not sure yet, but left and diagonal probably add cost, and top might be a straight copy
    or a copy plus a unmatch.

    Errata: Now minimizing unmatched then minimizing similarity

    Errata: Now maximizing similarity

  */

  int olength = construct_list_original.size();
  int nlength = construct_list_modified.size();

  size_t mem_size = olength * nlength * sizeof(difference);

  difference * differences = (difference *)malloc(mem_size);

  // still need to figure out how to track matching on each path
  for(int i = 0; i < nlength; ++i) {

    for(int j = 0; j < olength; ++j) {

      /** loop O(nd) */
      srcdiff_text_measure measure(construct_list_original.at(j), construct_list_modified.at(i));
      measure.compute();
      int similarity = measure.similarity();

      //unsigned long long max_similarity = (unsigned long long)-1;
      int max_similarity = -1;
      int unmatched = 0;

      // check if unmatched
      /** loop text O(nd) + syntax O(nd) + best match is O(nd) times number of matches */
      if(!is_match(construct_list_original, j, construct_list_modified, i, measure)) {

        similarity = 0;
        unmatched = 2;

      }

      int num_unmatched = MAX_INT;
      int direction = 0;

      bool matched = false;

      // check along x axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(j > 0) {

        max_similarity = differences[i * olength + (j - 1)].similarity;
        num_unmatched = differences[i * olength + (j - 1)].num_unmatched + 1;

        matched = false;

        // may be wrong
        int temp_num_unmatched = i + j + unmatched;

        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && similarity > max_similarity)) {

          matched = !unmatched;

          max_similarity = similarity;
          num_unmatched = temp_num_unmatched;

        }

        direction = 1;

      }

      // check along y axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(i > 0) {

        // may not have been initialized in j > 0
        if(direction == 0) {
          direction = 2;
        }

        int temp_similarity = differences[(i - 1) * olength + j].similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + j].num_unmatched + 1;

        // may be wrong
        int temp_num_unmatched_match = i + j + unmatched;

        int temp_matched = false;

        if(temp_num_unmatched_match < temp_num_unmatched || (temp_num_unmatched_match == temp_num_unmatched && similarity > temp_similarity)) {

          temp_matched = !unmatched;

          temp_similarity = similarity;
          temp_num_unmatched = temp_num_unmatched_match;

        }

        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity > max_similarity)) {

          matched = temp_matched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;

          direction = 2;

        }

      }

      // go along diagonal just add similarity and unmatched
      if(i > 0 && j > 0) {

        int temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + (j - 1)].num_unmatched + unmatched;

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

/**
 * is_single_call_expr
 * @param nodes List of srcml nodes
 * @param start_pos The position of a expr_stmt or expr start tag
 *
 * Returns whether or not an expression statement contains an expression
 * that is made entirely of one call
 * 
 * @returns true if a single call expression
 *          or false if not
 */
bool is_single_call_expr(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "expr_stmt" && nodes.at(start_pos)->name != "expr")) return false;


  if(nodes.at(start_pos)->type == XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name == "expr_stmt") {
    ++start_pos;
  }


  if(nodes.at(start_pos)->type == XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name == "expr") {
    ++start_pos;
  }

  if(nodes.at(start_pos)->name != "call") return false;

  int open_call_count = 1;
  ++start_pos;

  while(open_call_count) {

    if(nodes.at(start_pos)->name == "call") {

      if(nodes.at(start_pos)->type == XML_READER_TYPE_ELEMENT) {
        ++open_call_count;
      }
      else if(nodes.at(start_pos)->type == XML_READER_TYPE_END_ELEMENT) {
        --open_call_count;
      }

    }

    ++start_pos;

  }

  if(nodes.at(start_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(start_pos)->name == "expr")
    return true;

  return false;
 
}

/**
 * skip_tag
 * @param nodes List of srcml nodes
 * @param start_pos The position of starting tag to be skipped
 *
 * This function can be used to skip a matching set of XML tags at the given
 * start position. If the position does not correspond with a starting tag 
 * an invalid argument exception is thrown.
 * 
 * loop 0(n)
 * 
 */
void skip_tag(const srcml_nodes & nodes, int & start_pos) {

  if(!nodes.at(start_pos)->is_open_tag()) throw std::invalid_argument("skip_tag: start is not open tag");

  std::string & start_tag = nodes.at(start_pos)->name;
  int open_type_count = 1;
  ++start_pos;

  while(open_type_count) {

    if(nodes.at(start_pos)->name == start_tag) {

      if(nodes.at(start_pos)->type == XML_READER_TYPE_ELEMENT) {
        ++open_type_count;
      }
      else if(nodes.at(start_pos)->type == XML_READER_TYPE_END_ELEMENT) {
        --open_type_count;
      }

    }

    ++start_pos;

  }

}

/**
 * top_level_name_seek
 * @param nodes List of srcml nodes
 * @param start_pos The position of a node to search children for name tag
 *
 * This function is used by extract_name to supply the function get_name with the
 * position of the starting name tag.
 * 
 * loop O(n)
 * 
 */
void top_level_name_seek(const srcml_nodes & nodes, int & start_pos) {

    if(nodes.at(start_pos)->type != XML_READER_TYPE_ELEMENT) return;

    std::string & start_tag = nodes.at(start_pos)->name;
    int name_start_pos = start_pos + 1;

    while(nodes.at(name_start_pos)->type != XML_READER_TYPE_ELEMENT || nodes.at(name_start_pos)->name != "name") {

      if(nodes.at(name_start_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(name_start_pos)->name == start_tag)
        return;

      if(nodes.at(name_start_pos)->type == XML_READER_TYPE_ELEMENT) {
        skip_tag(nodes, name_start_pos);
      }
      else {
        ++name_start_pos;
      }

    }

    start_pos = name_start_pos;

}


/**
 * get_name
 * @param nodes list of srcml nodes
 * @param start_pos position of starting tag to begin at
 *
 * @pre parameter name_start_pos must be the location of a node containing an open name tag
 *
 * This function can be used to get the name contents between an open XML name tag 
 * and its corresponding closing name tag 
 * 
 * loop 0(n)
 *
 * @returns string containing what was found in the name tag set 
 *          or empty string when not found
 *
 */

std::string get_name(const srcml_nodes & nodes, int name_start_pos) {

  int open_name_count = 1;
  int name_pos = name_start_pos + 1;
  std::string name = "";

  while(open_name_count) {

    if(nodes.at(name_pos)->type == XML_READER_TYPE_ELEMENT && nodes.at(name_pos)->name == "argument_list") return name;

    if(nodes.at(name_pos)->name == "name") {

      if(nodes.at(name_pos)->type == XML_READER_TYPE_ELEMENT) {
        ++open_name_count;
      }
      else if(nodes.at(name_pos)->type == XML_READER_TYPE_END_ELEMENT) {
        --open_name_count;
      }

    } else if(nodes.at(name_pos)->is_text() && !nodes.at(name_pos)->is_white_space()) {

      name += nodes.at(name_pos)->content ? *nodes.at(name_pos)->content : "";

    }

    ++name_pos;

  }

  return name;

}


/**
 * extract_name
 * @param nodes list of srcml nodes
 * @param start_pos position of starting tag to begin at
 *
 * This function can be used to extract a name from a set of name tags
 *
 * loop 0(n)
 * 
 * @returns  string containing what was found in the top-level most name tag set
 *           or empty string when nothing can be found
 *
 */

std::string extract_name(const srcml_nodes & nodes, int start_pos) {

  int name_start_pos = start_pos;
  top_level_name_seek(nodes, name_start_pos);

  if(name_start_pos == start_pos) return "";

  return get_name(nodes, name_start_pos);  

}


/**
 * get_call_name
 * @param nodes list of srcml nodes
 * @param start_pos position of starting tag to begin at
 *
 * @pre start_pos must be the location of a starting call tag
 *
 * Extracts the name from a function call
 *
 * loop 0(n)
 * 
 * @returns vector of strings containing the name of the function call
 *          or empty vector when not found
 *
 */

std::vector<std::string> get_call_name(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != XML_READER_TYPE_ELEMENT || nodes.at(start_pos)->name != "call")
    return std::vector<std::string>();

  int name_start_pos = start_pos + 1;

  while(nodes.at(name_start_pos)->type != XML_READER_TYPE_ELEMENT
   || (nodes.at(name_start_pos)->name != "name" && nodes.at(name_start_pos)->name != "argument_list")) {
    ++name_start_pos;
   }

  if(nodes.at(name_start_pos)->name == "argument_list") return std::vector<std::string>();

  std::vector<std::string> name_list;

  int open_name_count = 1;
  int name_pos = name_start_pos + 1;
  std::string name = "";

  while(open_name_count) {

    if(nodes.at(name_pos)->type == XML_READER_TYPE_ELEMENT && nodes.at(name_pos)->name == "argument_list") return name_list;

    if(nodes.at(name_pos)->name == "name") {

      if(nodes.at(name_pos)->type == XML_READER_TYPE_ELEMENT) {

        ++open_name_count;
        name = "";

      } else if(nodes.at(name_pos)->type == XML_READER_TYPE_END_ELEMENT)

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

/** loop O(name_size*edits) */
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


/**
 * get_decl_name
 * @param nodes list of srcml nodes
 * @param start_pos position of starting tag to begin at
 *
 * @pre start_pos must be the location of a starting decl_stmt,
 *      parameter, param, or decl tag
 *
 * Extracts the name from a declaration
 *
 * loop O(n)
 * 
 * @returns declaration's name
 *          or empty string when not found
 *
 */

std::string get_decl_name(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "decl_stmt"
      && nodes.at(start_pos)->name != "parameter"
      && nodes.at(start_pos)->name != "param"
      && nodes.at(start_pos)->name != "decl")) return "";

  if(nodes.at(start_pos)->name != "decl") {
    ++start_pos;
  }

  return extract_name(nodes, start_pos);

}

/**
 * get_for_condition
 * @param nodes List of srcml nodes
 * @param start_pos Position of a for loop starting tag
 *
 * Extracts the condition from a for loop
 *
 * loop O(n)
 * 
 * @returns for loop's condition
 * 
 */
std::string get_for_condition(const srcml_nodes & nodes, int start_pos) {

  int control_start_pos = start_pos;

  while(nodes.at(control_start_pos)->type != XML_READER_TYPE_ELEMENT
   || nodes.at(control_start_pos)->name != "control") {
    ++control_start_pos;
   }


  int control_end_pos = control_start_pos + 1;
  int open_control_count = 1;

  while(open_control_count) {

    if(nodes.at(control_end_pos)->name == "control") {

      if(nodes.at(control_end_pos)->type == XML_READER_TYPE_ELEMENT) {
        ++open_control_count;
      }
      else if(nodes.at(control_end_pos)->type == XML_READER_TYPE_END_ELEMENT) {
        --open_control_count;
      }

    }

    ++control_end_pos;

  }

  --control_end_pos;

  construct_list control_sets = construct_list(nodes, control_start_pos + 1, control_end_pos);

  construct_list::const_iterator citr;
  for(citr = control_sets.begin(); citr != control_sets.end(); ++citr) {
    if(citr->term(0)->name == "condition") {
      break;
    }
  }

  if(citr == control_sets.end()) return "";

  std::string condition = "";
  for(std::size_t pos = 0; pos < citr->size(); ++pos) {
    if(citr->term(pos)->is_text()) {
      condition += citr->term(pos)->content ? *citr->term(pos)->content : "";
    }
  }

  if(condition.size() > 0 && *--condition.end() == ';') {
    condition.erase(--condition.end());
  }

  return condition;

}

/**
 * get_condition
 * @param nodes List of srcml nodes
 * @param start_pos Position of starting while, switch, do, if, or for tag
 *
 * Extracts the condition from while, switch, do, if, or for
 * 
 * loop O(n)
 * 
 * @returns condition as a string
 */
std::string get_condition(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->name == "for" || nodes.at(start_pos)->name == "foreach") {
    return get_for_condition(nodes, start_pos);
  }

  int condition_start_pos = start_pos;

  while(nodes.at(condition_start_pos)->type != XML_READER_TYPE_ELEMENT
    || nodes.at(condition_start_pos)->name != "condition") {
    ++condition_start_pos;
    }

  std::string condition = "";
  int open_condition_count = 1;
  int condition_pos = condition_start_pos + 1;

  while(open_condition_count) {

    if(nodes.at(condition_pos)->name == "condition") {

      if(nodes.at(condition_pos)->type == XML_READER_TYPE_ELEMENT) {
        ++open_condition_count;
      }
      else if(nodes.at(condition_pos)->type == XML_READER_TYPE_END_ELEMENT) {
        --open_condition_count;
      }

    } else if(nodes.at(condition_pos)->is_text() && !nodes.at(condition_pos)->is_white_space()) {

      condition += nodes.at(condition_pos)->content ? *nodes.at(condition_pos)->content : "";

    }

    ++condition_pos;

  }

  if(condition.size() > 0 && *condition.begin() == '(') {
    condition.erase(condition.begin());
  }

  if(condition.size() > 0 && *--condition.end() == ')') {
    condition.erase(--condition.end());
  }

  return condition;

}

/**
 * get_function_type_name
 * @param nodes List of srcml nodes
 * @param start_pos Position of starting function, constructor, destructor tag
 *
 * Extracts the name from a function, constructor, or destructor
 * 
 * loop O(n)
 * 
 * @returns function's name
 *          or empty string if not found
 * 
 */
std::string get_function_type_name(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "function" && nodes.at(start_pos)->name != "function_decl"
      && nodes.at(start_pos)->name != "constructor" && nodes.at(start_pos)->name != "constructor_decl"
      && nodes.at(start_pos)->name != "destructor" && nodes.at(start_pos)->name != "destructor_decl")) return "";


  return extract_name(nodes, start_pos);

}


/**
 * get_class_type_name
 * @param nodes list of srcml nodes
 * @param start_pos position of starting tag to begin at
 *
 * @pre start_pos must be the location of a starting class,
 *      struct, or union tag.
 *
 * Extracts the name of a class, struct, or union
 *
 * loop 0(n)
 * 
 * @returns class, struct, or union name
 *          or empty string if name cannot be found
 *
 */

std::string get_class_type_name(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "class" && nodes.at(start_pos)->name != "class_decl"
      && nodes.at(start_pos)->name != "struct" && nodes.at(start_pos)->name != "struct_decl"
      && nodes.at(start_pos)->name != "union" && nodes.at(start_pos)->name != "union_decl"
      && nodes.at(start_pos)->name != "enum" && nodes.at(start_pos)->name != "enum_decl")) return "";

  return extract_name(nodes, start_pos);

}

/**
 * conditional_has_block
 * @param node_set
 *
 * Checks if a conditional has a block
 * 
 * loop O(n)
 * 
 * @returns true if conditional has a block
 *          or false if does not have a block
 * 
 */
bool conditional_has_block(const construct & set) {
  
  // Todo: Add assertions that set is a conditional
  construct_list sets = construct_list(set.nodes(), set.get_terms().at(1), set.end_position());

  for(construct_list::iterator itr = sets.begin(); itr != sets.end(); ++itr) {

    if(itr->term(0)->name == "block" && !bool(find_attribute(itr->term(0), "type"))) {
      return true;
    } 

  }

  return false;

}


/**
 * get_first_child
 * @param set set of srcml nodes
 *
 * @pre The first node of the given node set should contain an opening if node tag
 *
 * Extracts the node set of the first child of the given parent node set
 *
 * @returns child node set
 *          
 */

construct get_first_child(const construct & set) {

  construct_list sets = construct_list(set.nodes(), set.get_terms().at(1), set.end_position());
  return sets.at(0);
}

bool is_child_if(const construct & child) {

  if(child.term(0)->name == "if") {
      return true;
  }

  return false;

}


/** loop O(n) */
bool if_stmt_has_else(const construct & set) {

  construct_list sets = construct_list(set.nodes(), set.get_terms().at(1), set.end_position());
  for(construct_list::iterator itr = sets.begin(); itr != sets.end(); ++itr) {
    if(itr->term(0)->name == "else" 
      || ( itr->term(0)->name == "if" 
        && bool(find_attribute(itr->term(0), "type")))) {
      return true;

    }

  }

  return false;

}

/** loop O(n) */
bool if_block_equal(const construct & set_original, const construct & set_modified) {

  diff_nodes dnodes = { set_original.nodes(), set_modified.nodes() };

  construct_list construct_list_original = construct_list(set_original.nodes(), set_original.get_terms().at(1), set_original.end_position());
  construct_list construct_list_modified = construct_list(set_modified.nodes(), set_modified.get_terms().at(1), set_modified.end_position());

  construct_list::iterator block_original;
  for(block_original = construct_list_original.begin(); block_original != construct_list_original.end(); ++block_original) {

    if(block_original->term(0)->name == "block") {

      break;

    }

  }

  if(block_original == construct_list_original.end()) return false;

  construct_list::iterator block_modified;
  for(block_modified = construct_list_modified.begin(); block_modified != construct_list_modified.end(); ++block_modified) {

    if(block_modified->term(0)->name == "block") {

      break;

    }

  }

  if(block_modified == construct_list_modified.end()) return false;

  bool block_is_equal = srcdiff_compare::element_syntax_compare((void *)&*block_original, (void *)&*block_modified, (void *)&dnodes) == 0;

  return block_is_equal;

}

/**
 * for_control_matches
 * @param set_original Original set of srcml nodes
 * @param set_modified Modified set of srcml nodes
 *
 * Checks if two for loops have matching controls
 * 
 * loop O(n)
 * 
 * @returns true if controls match
 *          or false if not a match
 * 
 */
bool for_control_matches(const construct & set_original, const construct & set_modified) {

  diff_nodes dnodes = { set_original.nodes(), set_modified.nodes() };

  construct_list construct_list_original = construct_list(set_original.nodes(), set_original.get_terms().at(1), set_original.end_position());
  construct_list construct_list_modified = construct_list(set_modified.nodes(), set_modified.get_terms().at(1), set_modified.end_position());

  construct_list::size_type control_pos_original;
  for(control_pos_original = 0; control_pos_original < construct_list_original.size(); ++control_pos_original) {
    if(construct_list_original.at(control_pos_original).term(0)->name == "control") {
      break;
    }
  }

  construct_list::size_type control_pos_modified;
  for(control_pos_modified = 0; control_pos_modified < construct_list_modified.size(); ++control_pos_modified) {
    if(construct_list_modified.at(control_pos_modified).term(0)->name == "control") {
      break;
    }
  }

  bool matches = control_pos_original != construct_list_original.size() && control_pos_modified != construct_list_modified.size() 
    && srcdiff_compare::element_syntax_compare((void *)&construct_list_original.at(control_pos_original), (void *)&construct_list_modified.at(control_pos_modified), (void *)&dnodes) == 0;

  return matches;

}

/** loop O(n) */
std::string get_case_expr(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != XML_READER_TYPE_ELEMENT
    || nodes.at(start_pos)->name != "case") return "";

  // skip case tag and case text
  int expr_pos = start_pos + 1;

  if((nodes.at(expr_pos)->is_text() && nodes.at(expr_pos)->content && nodes.at(expr_pos)->content->find(':') != std::string::npos)
     || (nodes.at(expr_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == "case")) return "";

  while((nodes.at(expr_pos)->type != XML_READER_TYPE_ELEMENT || nodes.at(expr_pos)->name != "expr")
    && !(nodes.at(expr_pos)->is_text() && nodes.at(expr_pos)->content && nodes.at(expr_pos)->content->find(':') != std::string::npos)
    && !(nodes.at(expr_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == "case")) {
    ++expr_pos;
  }

  if((nodes.at(expr_pos)->is_text() && nodes.at(expr_pos)->content && nodes.at(expr_pos)->content->find(':') != std::string::npos)
    || (nodes.at(expr_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == "case")) return "";

  std::string case_expr = "";

  int open_expr_count = 1;
  ++expr_pos;

  while(open_expr_count) {

    if(nodes.at(expr_pos)->name == "expr") {

      if(nodes.at(expr_pos)->type == XML_READER_TYPE_ELEMENT) {
        ++open_expr_count;
      }
      else if(nodes.at(expr_pos)->type == XML_READER_TYPE_END_ELEMENT) {
        --open_expr_count;
      }

    } else if(nodes.at(expr_pos)->is_text() && !nodes.at(expr_pos)->is_white_space()) {

      case_expr += nodes.at(expr_pos)->content ? *nodes.at(expr_pos)->content : "";

    }

    ++expr_pos;

  }

  return case_expr;

}

/** loop O(n) */
bool is_single_name_expr(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != XML_READER_TYPE_ELEMENT
    || (nodes.at(start_pos)->name != "expr_stmt" && nodes.at(start_pos)->name != "expr")) return false;

  if(nodes.at(start_pos)->type == XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name == "expr_stmt") {
    ++start_pos;
  }

  if(nodes.at(start_pos)->type == XML_READER_TYPE_ELEMENT && nodes.at(start_pos)->name == "expr") {
    ++start_pos;
  }

  if(nodes.at(start_pos)->name != "name") return false;

  int open_name_count = 1;
  ++start_pos;

  while(open_name_count) {

    if(nodes.at(start_pos)->name == "name") {

      if(nodes.at(start_pos)->type == XML_READER_TYPE_ELEMENT) {
        ++open_name_count;
      }
      else if(nodes.at(start_pos)->type == XML_READER_TYPE_END_ELEMENT) {
        --open_name_count;
      }

    }

    ++start_pos;

  }

  return nodes.at(start_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(start_pos)->name == "expr";

}

/** loop O(n) */
construct get_first_expr_child(const srcml_nodes & nodes, int start_pos) {

  int expr_pos = start_pos;

  while((nodes.at(expr_pos)->type != XML_READER_TYPE_ELEMENT || nodes.at(expr_pos)->name != "expr")
    && !(nodes.at(expr_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == nodes.at(start_pos)->name)) {
    ++expr_pos;
  }

  if(nodes.at(expr_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == nodes.at(start_pos)->name) {
    return construct(nodes);
  }

  return construct(nodes, expr_pos);

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
static const char * const if_stmt_interchange[]   = { "if_stmt", "while", "for", "foreach",    0 };
static const char * const else_interchange[]      = { "else", "if",                   0 };
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

  { "if_stmt",   if_stmt_interchange },
  { "while",     if_stmt_interchange },
  { "for",       if_stmt_interchange },
  { "foreach",   if_stmt_interchange },
  
  // need to fix
  { "else",      else_interchange },
  { "if",    else_interchange },

  {"expr_stmt", expr_stmt_interchange },
  {"decl_stmt", expr_stmt_interchange },
  {"return"   , expr_stmt_interchange },

  {"cast", cast_interchange },

  { 0, 0 }

};

bool srcdiff_match::is_interchangeable_match(const construct & original_set, const construct & modified_set) {

  const std::string & original_tag = original_set.get_root_name();
  const std::string & modified_tag = modified_set.get_root_name();

  const std::string & original_uri = original_set.get_root()->ns.href;
  const std::string & modified_uri = modified_set.get_root()->ns.href;

  bool original_has_type_attribute = bool(find_attribute(original_set.get_root(), "type"));
  bool modified_has_type_attribute = bool(find_attribute(original_set.get_root(), "type"));

  if(original_uri != modified_uri) return false;

  if(original_tag == "if" && original_uri != SRCML_SRC_NAMESPACE_HREF) return false;
  if(modified_tag == "if" && modified_uri != SRCML_SRC_NAMESPACE_HREF) return false;

  if(original_tag == "if" && !original_has_type_attribute) return false;
  if(original_tag == "if" && !modified_has_type_attribute) return false;

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

/** loop O(n) + O(nd) syntax child/grandparent, O(check_nestable)  */
bool reject_match_same(const srcdiff_measure & measure,
                       const construct & set_original,
                       const construct & set_modified) {

  int original_pos = set_original.start_position();
  int modified_pos = set_modified.start_position();

  const std::string & original_tag = set_original.term(0)->name;
  const std::string & modified_tag = set_modified.term(0)->name;

  const std::string & original_uri = set_original.term(0)->ns.href;
  const std::string & modified_uri = set_modified.term(0)->ns.href;

  if(original_tag != modified_tag) return true;

  if(original_tag == "type" || original_tag == "then" || original_tag == "condition" || original_tag == "control" || original_tag == "init"
    || original_tag == "default" || original_tag == "comment"
    || original_tag == "private" || original_tag == "protected" || original_tag == "public" || original_tag == "signals"
    || original_tag == "parameter_list" || original_tag == "krparameter_list" || original_tag == "argument_list"
    || original_tag == "attribute_list" || original_tag == "association_list" || original_tag == "protocol_list"
    || original_tag == "super_list" || original_tag == "member_init_list" || original_tag == "member_list"
    || original_tag == "argument"
    || original_tag == "range"
    || original_tag == "literal" || original_tag == "operator" || original_tag == "modifier"
    || original_tag == "number" || original_tag == "file"

    // consider having this used to test similarity instead of block
    || original_tag == "block_content"
    )
    return false;

  if(original_tag == "name" && set_original.term(0)->is_simple && set_modified.term(0)->is_simple) return false;
  if(original_tag == "name" && set_original.term(0)->is_simple != set_modified.term(0)->is_simple) return true;

  if((original_tag == "expr" || original_tag == "expr_stmt") && measure.similarity() > 0 && measure.difference() <= measure.max_length()) return false;

  // may need to refine to if child only single name
  if(original_tag == "expr" && set_original.term(0)->parent && (*set_original.term(0)->parent)->name == "argument") return false;
  if(original_tag == "expr" && set_modified.term(0)->parent && (*set_modified.term(0)->parent)->name == "argument") return false;
  if(original_tag == "expr" && is_single_name_expr(set_original.nodes(), original_pos) && is_single_name_expr(set_modified.nodes(), modified_pos)) return false;

  if(original_tag == "block") {

    bool is_pseudo_original = bool(find_attribute(set_original.term(0), "type"));
    bool is_pseudo_modified = bool(find_attribute(set_modified.term(0), "type"));

    if(is_pseudo_original == is_pseudo_modified) {

      return false;

    } else if(measure.similarity()) {

      bool is_reject = true;

      if(is_pseudo_original) {

        size_t block_contents_pos = 1;
        while(set_original.get_node_name(block_contents_pos) != "block_content") {
          ++block_contents_pos;
        }
        ++block_contents_pos;

        construct_list construct_list_original = construct_list(set_original.nodes(), set_original.get_terms().at(block_contents_pos), set_original.end_position());
        construct_list construct_list_modified = construct_list(set_modified.nodes(), set_modified.get_terms().at(0), set_modified.end_position() + 1);

        int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
        srcdiff_nested::check_nestable(construct_list_original, 0, construct_list_original.size(), construct_list_modified, 0, 1,
                      start_nest_original, end_nest_original, start_nest_modified , end_nest_modified, operation);

        is_reject = !(operation == SES_INSERT);

      } else {

        size_t block_contents_pos = 1;
        while(set_modified.get_node_name(block_contents_pos) != "block_content") {
          ++block_contents_pos;
        }
        ++block_contents_pos;

        construct_list construct_list_original = construct_list(set_original.nodes(), set_original.get_terms().at(0), set_original.end_position() + 1);
        construct_list construct_list_modified = construct_list(set_modified.nodes(), set_modified.get_terms().at(block_contents_pos), set_modified.end_position());

        int start_nest_original, end_nest_original, start_nest_modified, end_nest_modified, operation;
        srcdiff_nested::check_nestable(construct_list_original, 0, 1, construct_list_modified, 0, construct_list_modified.size(),
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

  } else if(original_tag == "if_stmt") {

    construct first_original = get_first_child(set_original);
    construct first_modified = get_first_child(set_modified);

    if(is_child_if(first_original) && is_child_if(first_modified)) {

      /** todo play with getting and checking a match with all conditions */
      std::string original_condition = get_condition(set_original.nodes(), original_pos);
      std::string modified_condition = get_condition(set_modified.nodes(), modified_pos);

      bool original_has_block = conditional_has_block(first_original);
      bool modified_has_block = conditional_has_block(first_modified);

      bool original_has_else = if_stmt_has_else(set_original);
      bool modified_has_else = if_stmt_has_else(set_modified);

      if(if_block_equal(first_original, first_modified)
        || (original_condition == modified_condition
          && ( original_has_block == modified_has_block 
            || original_has_else == modified_has_else 
            || (original_has_block && !modified_has_else) 
            || (modified_has_block && !original_has_else)))) {
        return false;
      }
    }

  } else if(original_tag == "if" && original_uri == SRCML_SRC_NAMESPACE_HREF) {

    if(get_condition(set_original.nodes(), original_pos) == get_condition(set_modified.nodes(), modified_pos)) {
      return false;
    }

    if(if_block_equal(set_original, set_modified)) {
     return false;
    }

  } else if(original_tag == "while" || original_tag == "switch" || original_tag == "do") {

    std::string original_condition = get_condition(set_original.nodes(), original_pos);
    std::string modified_condition = get_condition(set_modified.nodes(), modified_pos);

    if(original_condition == modified_condition) return false;

  } else if(original_tag == "for" || original_tag == "foreach") {

    if(for_control_matches(set_original, set_modified)) {
      return false;
    }

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
                                  const construct & set_original,
                                  const construct & set_modified) {

  int original_pos = set_original.start_position();
  int modified_pos = set_modified.start_position();

  const std::string & original_tag = set_original.term(0)->name;
  const std::string & modified_tag = set_modified.term(0)->name;

  const std::string & original_uri = set_original.term(0)->ns.href;
  const std::string & modified_uri = set_modified.term(0)->ns.href;

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

  if(original_tag == "if_stmt") {
    /** todo play with getting and checking a match with all conditions */
    construct first_original = get_first_child(set_original);
    if(is_child_if(first_original)) {
      original_condition = get_condition(set_original.nodes(), original_pos);
    }
  }

  if(original_tag == "while" || original_tag == "for" || original_tag == "foreach") {

    original_condition = get_condition(set_original.nodes(), original_pos);

  }

  std::string modified_condition;
 
  if(modified_tag == "if_stmt") {
    construct first_modified = get_first_child(set_modified);
    if(is_child_if(first_modified)) {
      modified_condition = get_condition(set_modified.nodes(), modified_pos);
    }
  }

  if(modified_tag == "while" || modified_tag == "for" || modified_tag == "foreach") {

    modified_condition = get_condition(set_modified.nodes(), modified_pos);

  }

  if(original_condition != "" && original_condition == modified_condition) return false;


  if(  (original_tag == "expr_stmt" || original_tag == "decl_stmt" || original_tag == "return")
    && (modified_tag == "expr_stmt" || modified_tag == "decl_stmt" || modified_tag == "return")) {

    construct expr_original(set_original.nodes());
    construct expr_modified(set_modified.nodes());
    if(original_tag == "decl_stmt" || modified_tag == "decl_stmt") {

      if(original_tag == "decl_stmt") {

        expr_modified = get_first_expr_child(set_modified.nodes(), modified_pos);

        if(!expr_modified.empty()) {

          construct_list sets = construct_list(set_original.nodes(), set_original.get_terms().at(1), set_original.end_position(), srcdiff_nested::is_match,
                                    &expr_modified.term(0));
          int match = srcdiff_nested::best_match(sets, expr_modified);

          if(match < sets.size()) {
            expr_original = sets.at(match);
          }

        }

      } else {

        expr_original = get_first_expr_child(set_original.nodes(), original_pos);

        if(!expr_original.empty()) {

          construct_list sets = construct_list(set_modified.nodes(), set_modified.get_terms().at(1), set_modified.end_position(), srcdiff_nested::is_match,
                                    &expr_original.term(0));
          int match = srcdiff_nested::best_match(sets, expr_original);

          if(match < sets.size()) {
            expr_modified = sets.at(match);
          }

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
                                 const construct & set_original,
                                 const construct & set_modified) {

  /** if different prefix should not reach here, however, may want to add that here */
  int original_pos = set_original.start_position();
  int modified_pos = set_modified.start_position();

  const std::string & original_tag = set_original.term(0)->name;
  const std::string & modified_tag = set_modified.term(0)->name;

  const std::string & original_uri = set_original.term(0)->ns.href;
  const std::string & modified_uri = set_modified.term(0)->ns.href;

  if(original_tag == modified_tag && original_uri == modified_uri) {
    return reject_match_same(measure, set_original, set_modified);
  } else if(is_interchangeable_match(set_original, set_modified)) {
    return reject_match_interchangeable(measure, set_original, set_modified);
  } else {
    return true;
  }

}

bool srcdiff_match::reject_similarity(const srcdiff_measure & measure,
                                      const construct & set_original,
                                      const construct & set_modified) {

  const std::string & original_tag = set_original.term(0)->name;
  const std::string & modified_tag = set_modified.term(0)->name;

  if(set_original.size() == 1 && set_modified.size() == 1) {
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

  construct_list child_construct_list_original = construct_list(set_original.nodes(), set_original.get_terms().at(1), set_original.end_position());
  construct_list child_construct_list_modified = construct_list(set_modified.nodes(), set_modified.get_terms().at(1), set_modified.end_position());    

  // check block of first child of if_stmt (old if behavior)
  if(original_tag == "if_stmt" && !child_construct_list_original.empty()) {

    std::string tag = child_construct_list_original.at(0).term(0)->name;
    if(tag == "else" || tag == "if") {
      construct_list temp = construct_list(set_original.nodes(), child_construct_list_original.at(0).get_terms().at(1), child_construct_list_original.back().end_position());
      child_construct_list_original = temp;
    }

  }

  // check block of first child of if_stmt (old if behavior)
  if(modified_tag == "if_stmt" && !child_construct_list_modified.empty()) {

    std::string tag =  child_construct_list_modified.at(0).term(0)->name;
    if(tag == "else" || tag == "if") {
      construct_list temp = construct_list(set_modified.nodes(), child_construct_list_modified.at(0).get_terms().at(1), child_construct_list_modified.back().end_position());
      child_construct_list_modified = temp;
    }

  }

  if(!child_construct_list_original.empty() && !child_construct_list_modified.empty()
    && child_construct_list_original.back().term(0)->name == "block" && child_construct_list_modified.back().term(0)->name == "block") {

    /// Why a copy?
    construct original_set = child_construct_list_original.back();
    construct modified_set = child_construct_list_modified.back();

    // block children actually in block_content
    construct_list original_temp = construct_list(set_original.nodes(), child_construct_list_original.back().get_terms().at(1), child_construct_list_original.back().end_position());
    for(const construct & set : original_temp) {
      if(set.term(0)->name == "block_content") {
        original_set = set;
      }
    }

    // block children actually in block_content
    construct_list modified_temp = construct_list(set_modified.nodes(), child_construct_list_modified.back().get_terms().at(1), child_construct_list_modified.back().end_position());
    for(const construct & set : modified_temp) {
      if(set.term(0)->name == "block_content") {
        modified_set = set;
      }
    }

    srcdiff_syntax_measure syntax_measure(original_set, modified_set);
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

#if DEBUG_SIMILARITY
  std::cerr << "Similarity: " << measure.similarity() << '\n';
  std::cerr << "Difference: " << measure.difference() << '\n';
  std::cerr << "Original Difference: " << measure.original_difference() << '\n';
  std::cerr << "Modified Difference: " << measure.modified_difference() << '\n';
  std::cerr << "Min Size: "   << min_size   << '\n';
  std::cerr << "Max Size: "   << max_size   << '\n';
#endif

  /** @todo consider making this configurable.  That is, allow user to specify file or have default file to read from */
  if(measure.difference() != 0 && measure.similarity() == 0) return true;

  if(min_size == measure.similarity() && measure.difference() < 2 * min_size) return false;
  if(min_size < 30 && measure.difference() > 1.25 * min_size)       return true;
  if(min_size >= 30 && measure.original_difference() > 0.25 * measure.original_length()
    && measure.modified_difference() > 0.25 * measure.modified_length()) return true;
  if(measure.difference() > max_size)                               return true;

  if(min_size <= 2)                return 2  * measure.similarity() <     min_size;
  else if(min_size <= 3)           return 3  * measure.similarity() < 2 * min_size;
  else if(min_size <= 30)          return 10 * measure.similarity() < 7 * min_size;
  else                             return 2  * measure.similarity() <     min_size;

}


bool srcdiff_match::reject_similarity_match_only(const construct & set_original,
                                                 const construct & set_modified) {

  int original_pos = set_original.start_position();
  int modified_pos = set_modified.start_position();

  if(srcdiff_compare::node_compare(set_original.term(0),
                                  set_modified.term(0)) != 0)
    return true;

  srcdiff_text_measure complete_measure(set_original, set_modified, false);
  complete_measure.compute();
  int min_size = complete_measure.min_length();

  if(min_size == 0) return true;

  return min_size != complete_measure.similarity();

}
