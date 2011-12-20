#include "srcDiffNested.hpp"
#include "shortest_edit_script.h"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffDiff.hpp"
#include "srcDiffUtility.hpp"

#include <string.h>

// global structures defined in main
extern std::vector<xNode *> nodes_old;
extern std::vector<xNode *> nodes_new;

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

// tags that can have something nested in them
const char * block_types[] = { "if", 0 };
//const char * block_types[] = { "block", "if", "while", "for", "function", 0 };

// tags that can be nested in something else (incomplete)
const char * nest_types[] = { "expr_stmt", "decl_stmt", 0 };
// const char * nest_types[] = { "block", "expr_stmt", "decl_stmt", 0 };

bool is_block_type(std::vector<int> * structure, std::vector<xNodePtr> & nodes) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp(nodes.at(structure->at(0))->ns->href, "http://www.sdml.info/srcML/src") != 0)
    return false;

  for(int i = 0; block_types[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, block_types[i]) == 0)
      return true;

  return false;
}

bool is_nest_type(std::vector<int> * structure, std::vector<xNodePtr> & nodes) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  for(int i = 0; nest_types[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, nest_types[i]) == 0)
      return true;

  return false;
}

bool has_interal_block(std::vector<int> * structure, std::vector<xNodePtr> & nodes) {

  if(strcmp((const char *)nodes.at(structure->at(0))->name, "block") == 0)
    return false;

  for(unsigned int i = 1; i < structure->size(); ++i)
    if(strcmp((const char *)nodes.at(structure->at(i))->name, "block") == 0)
      return true;

  return false;
}

bool is_nestable(std::vector<int> * structure_one, std::vector<xNodePtr> & nodes_one
                 , std::vector<int> * structure_two, std::vector<xNodePtr> & nodes_two) {


  if(is_nest_type(structure_one, nodes_one) && is_block_type(structure_two, nodes_two)) {

    if(strcmp((const char *)nodes_one.at(structure_one->at(0))->name, "block") != 0) {

      return true;

    } else {

      if(has_interal_block(structure_two, nodes_two))
        return true;

    }
  }

  return false;
}

// create the node sets for shortest edit script
std::vector<std::vector<int> *> create_node_set(std::vector<xNodePtr> & nodes, int start, int end, xNode * type) {

  std::vector<std::vector<int> *> node_sets;

  // runs on a subset of base array
  for(int i = start; i < end; ++i) {

    if((xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_ELEMENT && node_compare(nodes.at(i), type) == 0) {
      
      std::vector <int> * node_set = new std::vector <int>;

      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(i)->name);

      collect_entire_tag(nodes, *node_set, i);

      node_sets.push_back(node_set);

    }

  }

  return node_sets;

}

int best_match(std::vector<std::vector<int> *> & node_set, std::vector<int> * match) {

  int match_pos = 0;
  int match_similarity = 0;
  if(node_set.size() > 0) {

    match_pos = 0;
    match_similarity = compute_collect_similarity(node_set.at(0), match);

  } else
    return 1;

  for(unsigned int i = 1; i < node_set.size(); ++i) {

    int similarity;
    if((similarity = compute_collect_similarity(node_set.at(1), match)) < match_similarity) {

      match_pos = i;
      match_similarity = similarity;

    }

  }

  return match_pos;

}

void output_nested(reader_state & rbuf_old, std::vector<int> * structure_old
                   , reader_state & rbuf_new ,std::vector<int> * structure_new
                   , int operation, writer_state & wstate) {

  output_white_space_prefix(rbuf_old, rbuf_new, wstate);

  unsigned int start;
  //unsigned int end;
  unsigned int start_pos;
  unsigned int end_pos;

  if(operation == DELETE) {

    std::vector<std::vector<int> *> node_set = create_node_set(nodes_old, structure_old->at(0), structure_old->back() + 1
      , nodes_new.at(structure_new->at(0)));

    /*
      for(start = 0; start < structure_old->size()
            && ((xmlReaderTypes)nodes_old.at(structure_old->at(start))->type != XML_READER_TYPE_ELEMENT
                || strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "then") != 0); ++start)
        ;

      ++start;

      start_pos = structure_old->at(start);
      end_pos = structure_old->back() - 1;


      //diff_type.value = change;
      //diff_old_start.properties = &diff_type;
      //diff_new_start.properties = &diff_type;
      
    output_change(rbuf_old, start_pos, rbuf_new, rbuf_new.last_output, wstate);

    output_white_space_suffix(rbuf_old, rbuf_new, wstate);

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(nodes_old, start_pos
                        , end_pos);

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(nodes_new,  structure_new->at(0)
                        , structure_new->back() + 1);

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    output_white_space_nested(rbuf_old, rbuf_new, DELETE, wstate);

    output_change(rbuf_old,  structure_old->back() + 1, rbuf_new, rbuf_new.last_output, wstate);
    */
  } else {

      for(start = 0; start < structure_new->size()
            && ((xmlReaderTypes)nodes_new.at(structure_new->at(start))->type != XML_READER_TYPE_ELEMENT
                || strcmp((const char *)nodes_new.at(structure_new->at(start))->name, "then") != 0); ++start)
        ;

      ++start;

      start_pos = structure_new->at(start);
      end_pos = structure_new->back() - 1;

      //diff_type.value = change;
      //diff_old_start.properties = &diff_type;
      //diff_new_start.properties = &diff_type;


    output_change(rbuf_old, rbuf_old.last_output, rbuf_new, start_pos, wstate);

    output_white_space_suffix(rbuf_old, rbuf_new, wstate);

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(nodes_old,  structure_old->at(0)
                        , structure_old->back() + 1);

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(nodes_new, start_pos
                        , end_pos);

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    output_white_space_nested(rbuf_old, rbuf_new, INSERT, wstate);

    output_change(rbuf_old,  rbuf_old.last_output, rbuf_new, structure_new->back() + 1, wstate);

  }

  output_white_space_all(rbuf_old, rbuf_new, wstate);

  //diff_old_start.properties = 0;
  //diff_new_start.properties = 0;

}
