#ifndef INCLUDED_SRCDIFFDIFF_HPP
#define INCLUDED_SRCDIFFDIFF_HPP

#include "srcDiffTypes.hpp"
#include "shortest_edit_script.h"

#include <vector>

struct offset_pair {

  int old_offset;
  int old_length;
  int new_offset;
  int new_length;
  int similarity;
  offset_pair * next;
};

#define MIN -1


// create sets of nodes
std::vector<std::vector<int> *> create_node_set(std::vector<xNodePtr> & nodes, int start, int end);

void * create_node_set_thread(void * arguments);

struct create_node_set_args {

  std::vector<xNodePtr> & nodes;
  int start;
  int end;
  std::vector<std::vector<int> *> & node_sets;

};

void create_node_sets(std::vector<xNodePtr> & nodes_old, int start_old, int end_old, std::vector<std::vector<int> *> node_set_old
                     , std::vector<xNodePtr> & nodes_new, int start_new, int end_new, std::vector<std::vector<int> *> node_set_new);

void collect_entire_tag(std::vector<xNodePtr> & nodes, std::vector<int> & node_set, int & start);

void output_diffs(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, writer_state & wstate);

int compute_collect_similarity(std::vector<int> * node_set_old, std::vector<int> * node_set_new);
int compute_similarity(std::vector<int> * node_set_old, std::vector<int> * node_set_new);

void match_differences(std::vector<std::vector<int> *> * node_sets_old
                       , std::vector<std::vector<int> *> * node_sets_new
                       , edit * edit_script, offset_pair ** matches);

void output_unmatched(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , int start_old, int end_old
                      , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , int start_new, int end_new
                      , writer_state & wstate);

void compare_many2many(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                       , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                       , edit * edit_script, writer_state & wstate);

void output_recursive(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate);

void free_node_sets(std::vector<std::vector<int> *> & node_sets);

#endif
