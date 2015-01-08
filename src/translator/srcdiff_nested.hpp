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

  static void check_nestable(const node_sets & node_sets_old, const srcml_nodes & nodes_old, int start_old, int end_old
                 , const node_sets & node_sets_new, const srcml_nodes & nodes_new, int start_new, int end_new
                 , int & start_nest_old, int & end_nest_old, int & start_nest_new, int & end_nest_new
                 , int & operation);

  static bool is_nestable(const node_set & structure_one, const srcml_nodes & nodes_one
                 , const node_set & structure_two, const srcml_nodes & nodes_two);


  static bool is_same_nestable(const node_set & structure_one, const srcml_nodes & nodes_one
                      , const node_set & structure_two, const srcml_nodes & nodes_two);

  static bool is_better_nested(const srcml_nodes & nodes_old, const node_sets & node_sets_old, int start_pos_old,
                    const srcml_nodes & nodes_new, const node_sets & node_sets_new, int start_pos_new,
                    int similarity, int difference, int text_old_length, int text_new_length);

  static bool reject_match_nested(int similarity, int difference, int text_old_length, int text_new_length,
  const srcml_nodes & nodes_old, const node_set & set_old, const srcml_nodes & nodes_new, const node_set & set_new);

};

#endif
