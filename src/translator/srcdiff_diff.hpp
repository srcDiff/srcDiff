#ifndef INCLUDED_SRCDIFF_DIFF_HPP
#define INCLUDED_SRCDIFF_DIFF_HPP

#include <srcDiffTypes.hpp>
#include <shortest_edit_script.h>

#include <vector>

class srcdiff_diff {

private:
  reader_state & rbuf_old;
  reader_state & rbuf_new;
  writer_state & wstate;

  node_sets * node_sets_old;
  node_sets * node_sets_new;

public:
  srcdiff_diff(reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate, node_sets * node_sets_old, node_sets * node_sets_new);
  void output();


};


void * create_node_set_thread(void * arguments);

struct create_node_set_args {

  std::vector<xNodePtr> & nodes;
  int start;
  int end;
  node_sets & sets;

};

void create_node_sets(std::vector<xNodePtr> & nodes_delete, int start_old, int end_old, node_sets & set_old
                      , std::vector<xNodePtr> & nodes_insert, int start_new, int end_new, node_sets & set_new);

bool go_down_a_level(reader_state & rbuf_old, node_sets * node_sets_old
                     , unsigned int start_old
                     , reader_state & rbuf_new, node_sets * node_sets_new
                     , unsigned int start_new
                     , writer_state & wstate);

bool group_sub_elements(reader_state & rbuf_old, node_sets * node_sets_old
                        , unsigned int start_old
                        , reader_state & rbuf_new, node_sets * node_sets_new
                        , unsigned int start_new
                        , writer_state & wstate);

void free_node_sets(node_sets & sets);

#endif
