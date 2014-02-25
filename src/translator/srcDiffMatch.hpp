#ifndef INCLUDED_SRCDIFFMATCH_HPP
#define INCLUDED_SRCDIFFMATCH_HPP

#include <vector>
#include "xmlrw.hpp"
#include "srcDiffDiff.hpp"
#include "shortest_edit_script.h"

struct offset_pair {

  int old_offset;
  int old_length;
  int new_offset;
  int new_length;
  int similarity;
  offset_pair * next;
};

struct difference {

  //unsigned long long similarity;
  int similarity;
  int num_unmatched;
  bool marked;
  int direction;
  unsigned int opos;
  unsigned int npos;

};

void match_differences_dynamic(std::vector<xNodePtr> & nodes_old, NodeSets * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, NodeSets * node_sets_new
                               , offset_pair ** matches);

void match_differences_dynamic(std::vector<xNodePtr> & nodes_old, NodeSets * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, NodeSets * node_sets_new
                               , edit * edit_script, offset_pair ** matches);

void match_differences_dynamic_unordered(std::vector<xNodePtr> & nodes_old, NodeSets * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, NodeSets * node_sets_new
                               , edit * edit_script, offset_pair ** matches);

void match_differences(std::vector<xNodePtr> & nodes_old, NodeSets * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, NodeSets * node_sets_new
                               , edit * edit_script, offset_pair ** matches);

#endif
