#include <srcdiff_match.hpp>

#include <srcdiff_nested.hpp>
#include <srcdiff_text_measure.hpp>
#include <srcdiff_syntax_measure.hpp>
#include <srcdiff_shortest_edit_script.hpp>
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

bool srcdiff_match::is_match_default(const construct::construct_list & sets_original, int start_pos_original,
                                     const construct::construct_list & sets_modified, int start_pos_modified) {

  const srcdiff_measure & measure = *sets_original.at(start_pos_original)->measure(*sets_modified.at(start_pos_modified));
  if(measure.similarity() == MAX_INT) return false;

  if(!sets_original.at(start_pos_original)->is_match_similar(*sets_modified.at(start_pos_modified))
    && !sets_original.at(start_pos_original)->can_refine_difference(*sets_modified.at(start_pos_modified)))
    return false;

  if(srcdiff_nested::is_better_nested(sets_original, start_pos_original,
                                      sets_modified, start_pos_modified))
    return false;

  return true;

}


srcdiff_match::srcdiff_match(const construct::construct_list & construct_list_original, const construct::construct_list & construct_list_modified,
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
      const srcdiff_measure & measure = *construct_list_original.at(j)->measure(*construct_list_modified.at(i));
      int similarity = measure.similarity();

      //unsigned long long max_similarity = (unsigned long long)-1;
      int max_similarity = -1;
      int unmatched = 0;

      // check if unmatched
      /** loop text O(nd) + syntax O(nd) + best match is O(nd) times number of matches */
      if(!is_match(construct_list_original, j, construct_list_modified, i)) {

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


std::optional<std::string> find_attribute(const std::shared_ptr<const srcml_node> & node, const char * attr_name) {

  const std::list<srcml_node::srcml_attr> & attributes = node->properties;

  std::list<srcml_node::srcml_attr>::size_type pos = 0;
  for(const srcml_node::srcml_attr & attr : attributes) {

    if(attr.name == attr_name)
      return attr.value;

  }

  return std::optional<std::string>();

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

  srcdiff_shortest_edit_script ses;

  ses.compute_edit_script(name_list_original, name_list_modified);

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

/** loop O(n) */
std::string get_case_expr(const srcml_nodes & nodes, int start_pos) {

  if(nodes.at(start_pos)->type != XML_READER_TYPE_ELEMENT || nodes.at(start_pos)->name != "case") return "";

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
std::shared_ptr<construct> get_first_expr_child(const srcml_nodes & nodes, int start_pos) {

  int expr_pos = start_pos;

  while((nodes.at(expr_pos)->type != XML_READER_TYPE_ELEMENT || nodes.at(expr_pos)->name != "expr")
    && !(nodes.at(expr_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == nodes.at(start_pos)->name)) {
    ++expr_pos;
  }

  if(nodes.at(expr_pos)->type == XML_READER_TYPE_END_ELEMENT && nodes.at(expr_pos)->name == nodes.at(start_pos)->name) {
    return std::make_shared<construct>(nodes);
  }

  return std::make_shared<construct>(nodes, expr_pos);

}
