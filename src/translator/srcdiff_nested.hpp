#ifndef INCLUDED_SRCDIFF_NESTED_HPP
#define INCLUDED_SRCDIFF_NESTED_HPP

#include <srcdiff_many.hpp>

class srcdiff_nested : public srcdiff_many {

protected:

  int start_original;
  int end_original;
  int start_modified;
  int end_modified;
  int operation;

private:

public:

  srcdiff_nested(const srcdiff_many & diff, int start_original, int end_original, int start_modified, int end_modified, int operation);

  virtual void output();

  static void check_nestable(const node_sets & node_sets_original, const srcml_nodes & nodes_original, int start_original, int end_original
                 , const node_sets & node_sets_modified, const srcml_nodes & nodes_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation);

  static bool is_nestable(const node_set & structure_one, const srcml_nodes & nodes_one
                 , const node_set & structure_two, const srcml_nodes & nodes_two);


  static bool is_same_nestable(const node_set & structure_one, const srcml_nodes & nodes_one
                      , const node_set & structure_two, const srcml_nodes & nodes_two);

  static bool is_better_nested(const srcml_nodes & nodes_original, const node_sets & node_sets_original, int start_pos_original,
                    const srcml_nodes & nodes_modified, const node_sets & node_sets_modified, int start_pos_modified,
                    int similarity, int difference, int text_original_length, int text_modified_length);

  static bool reject_match_nested(int similarity, int difference, int text_original_length, int text_modified_length,
  const srcml_nodes & nodes_original, const node_set & set_original, const srcml_nodes & nodes_modified, const node_set & set_modified);

};

#endif
