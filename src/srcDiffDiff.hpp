#ifndef INCLUDED_SRCDIFFDIFF_HPP
#define INCLUDED_SRCDIFFDIFF_HPP

#include "srcDiffTypes.hpp"

#include <vector>

// create sets of nodes
std::vector<std::vector<int> *> create_node_set(std::vector<xmlNodePtr> & nodes, int start, int end);

void collect_entire_tag(std::vector<xmlNodePtr> & nodes, std::vector<int> & node_set, int & start);

void output_diffs(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, writer_state & wstate) {

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

#endif
