#ifndef INCLUDED_SRCDIFF_DIFF_HPP
#define INCLUDED_SRCDIFF_DIFF_HPP

#include <srcdiff_output.hpp>
#include <node_sets.hpp>
#include <shortest_edit_script.h>

#include <vector>

struct diff_nodes {

  const std::vector<xNode *> & nodes_old;
  const std::vector<xNode *> & nodes_new;

};

class srcdiff_diff {

protected:
  srcdiff_output & out;

  const node_sets & node_sets_old;
  const node_sets & node_sets_new;

public:
  srcdiff_diff(srcdiff_output & out, const node_sets & node_sets_old, const node_sets & node_sets_new);

  virtual void output();
  virtual void output_common(int end_old, int end_new);
  virtual void output_pure(int end_old, int end_new);
  virtual void output_change(int end_old, int end_new);
  virtual void output_change_whitespace(int end_old, int end_new);

};

#endif
