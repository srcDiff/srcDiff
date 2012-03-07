#include "srcDiffNested.hpp"
#include "shortest_edit_script.h"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffDiff.hpp"
#include "srcDiffUtility.hpp"
#include "srcDiffMeasure.hpp"

#include <string.h>

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

// diff attribute
extern xAttr diff_type;
extern const char * change;
extern const char * whitespace;

int nest_id = 0;

struct nest_info {

  const char * type;

  const char * const *  nest_items;

  const char * const * possible_nest_items;

};

// may need to change collection algorithm to gather full and nested of same type and match based on that
// possible is mostly for block and may need to have to test for internal block structure

const char * const block_nest_types[] = { "expr_stmt", "decl_stmt", 0 };
const char * const if_nest_types[] = { "expr_stmt", "decl_stmt", "else", 0 };
const char * const else_nest_types[] = { "expr_stmt", "decl_stmt", 0 };
const char * const while_nest_types[] = { "expr_stmt", "decl_stmt", 0 };
const char * const for_nest_types[] = { "expr_stmt", "decl_stmt", 0 };
const char * const function_nest_types[] = { "expr_stmt", "decl_stmt", "if", "while", "for", 0 };
const char * const class_nest_types[] = { "decl_stmt", "function_decl", 0 };
const char * const struct_nest_types[] = { "decl_stmt", "function_decl", 0 };
const char * const union_nest_types[] = { "decl_stmt",  "function_decl", 0 };

const char * const basic_possible_nest_types[] = { 0 };
const char * const block_possible_nest_types[] = { "block", 0 };
const char * const if_possible_nest_types[] = { "block", "if", "while", "for", 0 };
const char * const else_possible_nest_types[] = { "block", "if", "while", "for", 0 };
const char * const while_possible_nest_types[] = { "block", "if", "while", "for", 0 };
const char * const for_possible_nest_types[] = { "block", "if", "while", "for", 0 };
const char * const function_possible_nest_types[] = { 0 };
const char * const class_possible_nest_types[] = { 0 };
const char * const struct_possible_nest_types[] = { 0 };
const char * const union_possible_nest_types[] = { 0 };

// tags that can have something nested in them (incomplete)
const nest_info nesting[] = {

  { "block", block_nest_types, block_possible_nest_types },
  { "if", if_nest_types, if_possible_nest_types },
  { "else", else_nest_types, else_possible_nest_types },
  { "while", while_nest_types, while_possible_nest_types },
  { "for", for_nest_types, for_possible_nest_types },
  { "function", function_nest_types, function_possible_nest_types },
  { "class", class_nest_types, class_possible_nest_types },
  { "struct", struct_nest_types, struct_possible_nest_types },
  { "union", union_nest_types, union_possible_nest_types },

  { 0, 0, 0 }

};

int is_block_type(NodeSet * structure, std::vector<xNodePtr> & nodes) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return -1;

  if(strcmp(nodes.at(structure->at(0))->ns->href, "http://www.sdml.info/srcML/src") != 0)
    return -1;

  for(int i = 0; nesting[i].type; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, nesting[i].type) == 0)
      return i;

  return -1;
}

bool is_nest_type(NodeSet * structure, std::vector<xNodePtr> & nodes, int type_index) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp(nodes.at(structure->at(0))->ns->href, "http://www.sdml.info/srcML/src") != 0)
    return -1;

  for(int i = 0; nesting[type_index].nest_items[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, nesting[type_index].nest_items[i]) == 0)
      return true;

  return false;
}

bool is_possible_nest_type(NodeSet * structure, std::vector<xNodePtr> & nodes
                           , NodeSet * structure_other, std::vector<xNodePtr> & nodes_other, int type_index) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp(nodes.at(structure->at(0))->ns->href, "http://www.sdml.info/srcML/src") != 0)
    return -1;

  for(int i = 0; nesting[type_index].possible_nest_items[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, nesting[type_index].possible_nest_items[i]) == 0
       && has_internal_structure(structure_other, nodes_other, (const char *)nodes.at(structure->at(0))->name))
      return true;

  return false;
}



bool has_internal_structure(NodeSet * structure, std::vector<xNodePtr> & nodes, const char * type) {

  for(unsigned int i = 1; i < structure->size(); ++i)
    if((xmlReaderTypes)nodes.at(structure->at(i))->type == XML_READER_TYPE_ELEMENT
              && strcmp((const char *)nodes.at(structure->at(i))->name, type) == 0)
      return true;

  return false;
}

bool complete_nestable(NodeSets & structure_one, std::vector<xNodePtr> & nodes_one
                  , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two) {

  unsigned int num_nest = 0;

  for(unsigned int i = 0; i < structure_one.size(); ++i) {

    if(is_nestable(structure_one.at(i), nodes_one, structure_two, nodes_two))
       ++num_nest;

  }

  return num_nest == structure_one.size();

}

// create the node sets for shortest edit script
NodeSets create_node_set(std::vector<xNodePtr> & nodes, int start, int end, xNode * type) {

  NodeSets node_sets;

  // runs on a subset of base array
  for(int i = start; i < end; ++i) {

    if((xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_ELEMENT && node_compare(nodes.at(i), type) == 0) {

      // save position to collect internal of same type on all levels
      int save_start = i;

      std::vector <int> * node_set = new std::vector <int>;

      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(i)->name);

      collect_entire_tag(nodes, *node_set, i);

      node_sets.push_back(node_set);

      // collect type on all levels
      i = save_start;

    }

  }

  return node_sets;

}

int best_match(std::vector<xNodePtr> & nodes, NodeSets & node_set
               , std::vector<xNodePtr> & nodes_match, NodeSet * match, int operation) {

  int match_pos = 0;
  int match_similarity = 0;
  if(node_set.size() > 0) {

    match_pos = 0;
    if(operation == SESDELETE)
      match_similarity = compute_similarity(nodes, node_set.at(0), nodes_match, match);
    else
      match_similarity = compute_similarity(nodes_match, match, nodes, node_set.at(0));

  } else
    return 1;

  for(unsigned int i = 1; i < node_set.size(); ++i) {

    int similarity;
    if((similarity =
        (operation == SESDELETE) ? compute_similarity(nodes, node_set.at(i), nodes_match, match) 
        : compute_similarity(nodes_match, match, nodes, node_set.at(i)))
       > match_similarity) {

      match_pos = i;
      match_similarity = similarity;

    }

  }

  return match_pos;

}

bool is_same_nestable(NodeSet *  structure_one, std::vector<xNodePtr> & nodes_one
                      , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two) {

  if(!is_nestable(structure_one, nodes_one, structure_two, nodes_two))
    return false;

  //unsigned int similarity = compute_similarity(nodes_one, structure_one, nodes_two, structure_two);

  NodeSets node_set = create_node_set(nodes_two, structure_two->at(1), structure_two->back()
                                                             , nodes_one.at(structure_one->at(0)));

  unsigned int match = best_match(nodes_two, node_set, nodes_one, structure_one, SESDELETE);

  if(match >= node_set.size())
    return false;

  unsigned int match_similarity = compute_similarity(nodes_one, structure_one, nodes_two, node_set.at(match));
  unsigned int match_difference = compute_difference(nodes_one, structure_one, nodes_two, node_set.at(match));
  unsigned int difference = compute_difference(nodes_one, structure_one, nodes_two, structure_two);

  unsigned int size_one = 0;

  for(unsigned int i = 0; i < structure_one->size(); ++i)
    if(is_text(nodes_one.at(structure_one->at(i))) && !is_white_space(nodes_one.at(structure_one->at(i))))
      ++size_one;

  unsigned int size_two = 0;

  for(unsigned int i = 0; i < structure_two->size(); ++i)
    if(is_text(nodes_two.at(structure_two->at(i))) && !is_white_space(nodes_two.at(structure_two->at(i))))
      ++size_two;

  unsigned int size_match = 0;

  for(unsigned int i = 0; i < node_set.at(match)->size(); ++i)
    if(is_text(nodes_two.at(node_set.at(match)->at(i))) && !is_white_space(nodes_two.at(node_set.at(match)->at(i))))
      ++size_match;

  unsigned int max_size = size_one;
  if(size_match > max_size)
    max_size = size_match;

  unsigned int min_size = size_one;
  if(size_match < min_size)
    min_size = size_match;

  return match_similarity * 10 > max_size * 9 && match_difference * 10 <= min_size && match_difference < difference;

}

bool is_nestable(NodeSet * structure_one, std::vector<xNodePtr> & nodes_one
                 , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two) {

  int block = is_block_type(structure_two, nodes_two);

  if(block == -1)
    return false;

  if(is_nest_type(structure_one, nodes_one, block)) {

    return true;

  }

  if(is_possible_nest_type(structure_one, nodes_one, structure_two, nodes_two, block)) {

    return true;

  }

  return false;
}

void set_nestable(NodeSet * structure_one, std::vector<xNodePtr> & nodes_one
                 , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two) {

  nodes_one.at(structure_one->at(0))->nest = ++nest_id;
  nodes_one.at(structure_one->back())->nest = nest_id;
  nodes_two.at(structure_two->at(0))->nest = nest_id;
  nodes_two.at(structure_two->back())->nest = nest_id;

}

void clear_nestable(NodeSet * structure_one, std::vector<xNodePtr> & nodes_one
                 , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two) {

  nodes_one.at(structure_one->at(0))->nest = 0;
  nodes_one.at(structure_one->back())->nest = 0;
  nodes_two.at(structure_two->at(0))->nest = 0;
  nodes_two.at(structure_two->back())->nest = 0;

}

void output_nested(reader_state & rbuf_old, NodeSet * structure_old
                   , reader_state & rbuf_new ,NodeSet * structure_new
                   , int operation, writer_state & wstate) {

  clear_nestable(structure_old, rbuf_old.nodes, structure_new, rbuf_new.nodes);

  output_white_space_prefix(rbuf_old, rbuf_new, wstate);

  unsigned int end_pos;

  // idea best match first of multi then pass all on to algorithm or set ending pos to recurse down
  if(operation == SESDELETE) {

    NodeSets node_set = create_node_set(rbuf_old.nodes, structure_old->at(1), structure_old->back()
                                                               , rbuf_new.nodes.at(structure_new->at(0)));

    NodeSets nest_set = create_node_set(rbuf_new.nodes, structure_new->at(0), structure_new->back() + 1);

    unsigned int match = best_match(rbuf_old.nodes, node_set, rbuf_new.nodes, nest_set.at(0), SESDELETE);

    if(match < node_set.size()) {

      end_pos = node_set.at(match)->at(0) - 1;

      for(; (signed)end_pos > structure_old->at(0) && is_white_space(rbuf_old.nodes.at(end_pos)); --end_pos)
        ;

      ++end_pos;

      output_change(rbuf_old, end_pos, rbuf_new, rbuf_new.last_output, wstate);

      output_white_space_suffix(rbuf_old, rbuf_new, wstate);

      // collect subset of nodes
      NodeSets next_node_set_old
        = create_node_set(rbuf_old.nodes, end_pos, node_set.back()->back() + 1);

      output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &nest_set, wstate);

      output_white_space_nested(rbuf_old, rbuf_new, SESDELETE, wstate);

      output_change(rbuf_old, structure_old->back() + 1, rbuf_new, rbuf_new.last_output, wstate);

    } else {

      output_change(rbuf_old, structure_old->back() + 1, rbuf_new, structure_new->back() + 1, wstate);

    }

  } else {

    NodeSets node_set = create_node_set(rbuf_new.nodes, structure_new->at(1), structure_new->back()
                                                               , rbuf_old.nodes.at(structure_old->at(0)));

    NodeSets nest_set = create_node_set(rbuf_old.nodes, structure_old->at(0), structure_old->back() + 1);

    unsigned int match = best_match(rbuf_new.nodes, node_set, rbuf_old.nodes, nest_set.at(0), SESINSERT);

    if(match < node_set.size()) {

      end_pos = node_set.at(match)->at(0) - 1;

      for(; (signed)end_pos > structure_new->at(0) && is_white_space(rbuf_new.nodes.at(end_pos)); --end_pos)
        ;

      ++end_pos;

      output_change(rbuf_old, rbuf_old.last_output, rbuf_new, end_pos, wstate);

      output_white_space_suffix(rbuf_old, rbuf_new, wstate);

      // collect subset of nodes
      NodeSets next_node_set_new
        = create_node_set(rbuf_new.nodes, end_pos, node_set.back()->back() + 1);

      output_diffs(rbuf_old, &nest_set, rbuf_new, &next_node_set_new, wstate);

      output_white_space_nested(rbuf_old, rbuf_new, SESINSERT, wstate);

      output_change(rbuf_old,  rbuf_old.last_output, rbuf_new, structure_new->back() + 1, wstate);

    } else {

      output_change(rbuf_old, structure_old->back() + 1, rbuf_new, structure_new->back() + 1, wstate);

    }

  }

  output_white_space_all(rbuf_old, rbuf_new, wstate);

  //diff_old_start.properties = 0;
  //diff_new_start.properties = 0;

}
