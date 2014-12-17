#ifndef INCLUDED_SRCDIFF_DIFF_HPP
#define INCLUDED_SRCDIFF_DIFF_HPP

#include <srcdiff_output.hpp>
#include <node_sets.hpp>

#include <shortest_edit_script.h>

#include <vector>

class srcdiff_diff {

protected:
  srcdiff_output & out;
  reader_state & rbuf_old;
  reader_state & rbuf_new;
  writer_state & wstate;

  node_sets * node_sets_old;
  node_sets * node_sets_new;

public:
  srcdiff_diff(srcdiff_output & out, node_sets * node_sets_old, node_sets * node_sets_new);

  virtual void output();
  virtual void output_common(int end_old, int end_new);
  virtual void output_pure(int end_old, int end_new);
  virtual void output_change(int end_old, int end_new);
  virtual void output_change_whitespace(int end_old, int end_new);

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
