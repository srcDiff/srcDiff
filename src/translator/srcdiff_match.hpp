#ifndef INCLUDED_SRCDIFF_MATCH_HPP
#define INCLUDED_SRCDIFF_MATCH_HPP

#include <node_sets.hpp>

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

  const std::vector<srcml_node *> & nodes_old;
  const std::vector<srcml_node *> & nodes_new;
  const node_sets & node_sets_old;
  const node_sets & node_sets_new;

private:

public:

  srcdiff_match(const std::vector<srcml_node *> & nodes_old, const std::vector<srcml_node *> & nodes_new, const node_sets & node_sets_old, const node_sets & node_sets_new);
  offset_pair * match_differences();

  static bool reject_match(int similarity, int difference, int text_old_length, int text_new_length,
    const std::vector<srcml_node *> & nodes_old, const node_set & set_old, const std::vector<srcml_node *> & nodes_new, const node_set & set_new);
  static bool reject_similarity(int similarity, int difference, int text_old_length, int text_new_length,
    const std::vector<srcml_node *> & nodes_old, const node_set & set_old, const std::vector<srcml_node *> & nodes_new, const node_set & set_new);


  static const char * find_attribute(const srcml_node * node, const char * attr_name);

  static bool is_interchangeable_match(const std::string & old_tag, const std::string & new_tag);

};



#endif
