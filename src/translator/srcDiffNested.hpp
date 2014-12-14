#ifndef INCLUDED_SRCDIFFNESTED_HPP
#define INCLUDED_SRCDIFFNESTED_HPP

#include <srcDiffTypes.hpp>
#include <vector>

bool complete_nestable(NodeSets & structure_one, std::vector<xNodePtr> & nodes_one
                       , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two);

bool is_nestable(NodeSet * structure_one, std::vector<xNodePtr> & nodes_one
                 , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two);

void check_nestable(NodeSets * node_sets_old, std::vector<xNodePtr> & nodes_old, int start_old, int end_old
                 , NodeSets * node_sets_new, std::vector<xNodePtr> & nodes_new, int start_new, int end_new
                 , int & start_nest_old, int & end_nest_old, int & start_nest_new, int & end_nest_new, int & operation);

void set_nestable(NodeSets * node_sets_old, std::vector<xNodePtr> & nodes_old, int start_old, int end_old
                 , NodeSets * node_sets_new, std::vector<xNodePtr> & nodes_new, int start_new, int end_new);

bool is_same_nestable(NodeSet *  structure_one, std::vector<xNodePtr> & nodes_one
                      , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two);

bool has_internal_structure(NodeSet * structure, std::vector<xNodePtr> & nodes, const char * type);

void output_nested_recursive(reader_state & rbuf_old,
                  NodeSets * nodes_sets_old,
                  int start_old, int end_old,
                  reader_state & rbuf_new,
                  NodeSets * nodes_sets_new,
                  int start_new, int end_new,
                  int operation, writer_state & wstate);

void output_nested(reader_state & rbuf_old, NodeSet * structure_old
                   , reader_state & rbuf_new ,NodeSet * structure_new
                   , int operation, writer_state & wstate);

int is_block_type(NodeSet * structure, std::vector<xNodePtr> & nodes);

bool is_better_nested(std::vector<xNodePtr> & nodes_old, NodeSets * node_sets_old, int start_pos_old,
                    std::vector<xNodePtr> & nodes_new, NodeSets * node_sets_new, int start_pos_new,
                    int similarity, int difference, int text_old_length, int text_new_length);

bool reject_match_nested(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, NodeSet * set_old, std::vector<xNodePtr> & nodes_new, NodeSet * set_new);

#endif
