#ifndef INCLUDED_SRCDIFFMATCH_HPP
#define INCLUDED_SRCDIFFMATCH_HPP

#include <node_sets.hpp>
#include <xmlrw.hpp>

struct offset_pair {

  int old_offset;
  int old_length;
  int new_offset;
  int new_length;
  int similarity;
  offset_pair * next;
};

class srcdiff_match {

protected:

  std::vector<xNodePtr> & nodes_old;
  std::vector<xNodePtr> & nodes_new;
  node_sets * node_sets_old;
  node_sets * node_sets_new;

private:

public:

  srcdiff_match(std::vector<xNodePtr> & nodes_old, std::vector<xNodePtr> & nodes_new, node_sets * node_sets_old, node_sets * node_sets_new);
  offset_pair * match_differences();

};



#endif
