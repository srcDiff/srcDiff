#ifndef INCLUDED_SRCDIFF_NESTED_HPP
#define INCLUDED_SRCDIFF_NESTED_HPP

#include <srcdiff_many.hpp>
#include <srcdiff_measure.hpp>

class srcdiff_nested : public srcdiff_many {

protected:

  int start_original;
  int end_original;
  int start_modified;
  int end_modified;
  int operation;

private:

static bool check_nestable_predicate(const node_sets & node_sets_outer, int pos_outer, int start_outer, int end_outer,
                                     const node_sets & node_sets_inner, int pos_inner, int start_inner, int end_inner);
public:

  static bool is_match(int & node_pos, const srcml_nodes & nodes, const void * context);
  static int best_match(const node_sets & set, const node_set & match);
  
  srcdiff_nested(const srcdiff_many & diff, int start_original, int end_original, int start_modified, int end_modified, int operation);

  virtual void output();

  static void check_nestable(const node_sets & node_sets_original, int start_original, int end_original
                 , const node_sets & node_sets_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation);

  static bool is_nestable(const node_set & structure_one,
                          const node_set & structure_two);


  static bool is_same_nestable(const node_set & structure_one,
                               const node_set & structure_two);

  static bool is_better_nested(const node_sets & node_sets_original, int start_pos_original,
                               const node_sets & node_sets_modified, int start_pos_modified,
                               const srcdiff_measure & measure);

  static bool reject_match_nested(const srcdiff_measure & measure,
                                  const node_set & set_original,
                                  const node_set & set_modified);

};

#endif
