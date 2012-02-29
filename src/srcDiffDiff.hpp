#ifndef INCLUDED_SRCDIFFDIFF_HPP
#define INCLUDED_SRCDIFFDIFF_HPP

#include "srcDiffTypes.hpp"
#include "shortest_edit_script.h"

#include <vector>

// create sets of nodes
NodeSets create_node_set(std::vector<xNodePtr> & nodes, int start, int end);

void * create_node_set_thread(void * arguments);

struct create_node_set_args {

  std::vector<xNodePtr> & nodes;
  int start;
  int end;
  NodeSets & node_sets;

};

void create_node_sets(std::vector<xNodePtr> & nodes_delete, int start_old, int end_old, NodeSets & node_set_old
                      , std::vector<xNodePtr> & nodes_insert, int start_new, int end_new, NodeSets & node_set_new);

void collect_entire_tag(std::vector<xNodePtr> & nodes, NodeSet & node_set, int & start);

bool go_down_a_level(reader_state & rbuf_old, NodeSets * node_sets_old
                     , unsigned int start_old
                     , reader_state & rbuf_new, NodeSets * node_sets_new
                     , unsigned int start_new
                     , writer_state & wstate);


void output_diffs(reader_state & rbuf_old, NodeSets * node_sets_old, reader_state & rbuf_new, NodeSets * node_sets_new, writer_state & wstate);

void output_recursive(reader_state & rbuf_old, NodeSets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate);

void free_node_sets(NodeSets & node_sets);

#endif
