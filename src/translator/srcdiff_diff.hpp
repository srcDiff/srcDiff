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

  virtual void output();


  static bool go_down_a_level(reader_state & rbuf_old, node_sets * node_sets_old
                     , unsigned int start_old
                     , reader_state & rbuf_new, node_sets * node_sets_new
                     , unsigned int start_new
                     , writer_state & wstate);

  static bool group_sub_elements(reader_state & rbuf_old, node_sets * node_sets_old
                        , unsigned int start_old
                        , reader_state & rbuf_new, node_sets * node_sets_new
                        , unsigned int start_new
                        , writer_state & wstate);

};

#endif
