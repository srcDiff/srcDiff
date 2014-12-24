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

  static bool reject_match(int similarity, int difference, int text_old_length, int text_new_length,
    std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new);
  static bool reject_similarity(int similarity, int difference, int text_old_length, int text_new_length,
    std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new);


  static const char * find_attribute(const xNodePtr node, const char * attr_name);

  static bool is_interchangeable_match(const std::string & old_tag, const std::string & new_tag);

};



#endif
