#ifndef INCLUDED_SRCDIFF_DIFF_HPP
#define INCLUDED_SRCDIFF_DIFF_HPP

#include <srcdiff_output.hpp>
#include <node_sets.hpp>

#include <shortest_edit_script.h>

#include <vector>

class srcdiff_diff {

protected:
  srcdiff_output & out;

  node_sets * node_sets_old;
  node_sets * node_sets_new;

public:
  srcdiff_diff(srcdiff_output & out, node_sets * node_sets_old, node_sets * node_sets_new);

  virtual void output();
  virtual void output_common(int end_old, int end_new);
  virtual void output_pure(int end_old, int end_new);
  virtual void output_change(int end_old, int end_new);
  virtual void output_change_whitespace(int end_old, int end_new);

  static bool go_down_a_level(std::vector<xNodePtr> & nodes_old, node_sets * node_sets_old
                     , unsigned int start_old
                     , std::vector<xNodePtr> & nodes_new, node_sets * node_sets_new
                     , unsigned int start_new);

  static bool group_sub_elements(std::vector<xNodePtr> & nodes_old, node_sets * node_sets_old
                        , unsigned int start_old
                        , std::vector<xNodePtr> & nodes_new, node_sets * node_sets_new
                        , unsigned int start_new);

};

#endif
