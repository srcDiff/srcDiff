#ifndef INCLUDED_SRCDIFF_NESTED_HPP
#define INCLUDED_SRCDIFF_NESTED_HPP

#include <srcdiff_many.hpp>

class srcdiff_nested : public srcdiff_many {

protected:

  int start_old;
  int end_old;
  int start_new;
  int end_new;
  int operation;

private:

public:

  srcdiff_nested(const srcdiff_many & diff, int start_old, int end_old, int start_new, int end_new, int operation);

  virtual void output();

  static void check_nestable(node_sets * node_sets_old, std::vector<xNodePtr> & nodes_old, int start_old, int end_old
                 , node_sets * node_sets_new, std::vector<xNodePtr> & nodes_new, int start_new, int end_new
                 , int & start_nest_old, int & end_nest_old, int & start_nest_new, int & end_nest_new
                 , int & operation);

  static bool is_nestable(node_set * structure_one, std::vector<xNodePtr> & nodes_one
                 , node_set * structure_two, std::vector<xNodePtr> & nodes_two);


  static bool is_same_nestable(node_set *  structure_one, std::vector<xNodePtr> & nodes_one
                      , node_set * structure_two, std::vector<xNodePtr> & nodes_two);

  static bool is_better_nested(std::vector<xNodePtr> & nodes_old, node_sets * node_sets_old, int start_pos_old,
                    std::vector<xNodePtr> & nodes_new, node_sets * node_sets_new, int start_pos_new,
                    int similarity, int difference, int text_old_length, int text_new_length);

  static bool reject_match_nested(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new);

};

#endif
