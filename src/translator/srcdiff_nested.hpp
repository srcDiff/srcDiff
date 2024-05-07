#ifndef INCLUDED_SRCDIFF_NESTED_HPP
#define INCLUDED_SRCDIFF_NESTED_HPP

#include <srcml_nodes.hpp>
#include <construct.hpp>
#include <srcdiff_many.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcdiff_measure.hpp>

class srcdiff_nested : public srcdiff_many {

protected:

  int start_original;
  int end_original;
  int start_modified;
  int end_modified;
  int operation;

private:

static bool check_nestable_predicate(construct::construct_list_view construct_list_outer,
                                     construct::construct_list_view construct_list_inner);
static std::tuple<std::vector<int>, int, int> check_nestable(construct::construct_list_view parent_list, construct::construct_list_view child_list);

public:

  static bool is_decl_stmt_from_expr(const srcml_nodes & nodes, int pos);
  static bool check_nest_name(const construct & set_original,
                              std::optional<std::shared_ptr<srcML::node>> parent_original,
                              const construct & set_modified,
                              std::optional<std::shared_ptr<srcML::node>> parent_modified);

  /// @todo replace? start/end 
  srcdiff_nested(const srcdiff_many & diff, int start_original, int end_original, int start_modified, int end_modified, int operation);

  /// @todo replace? start/end
  void output_inner(construct::construct_list_view original, construct::construct_list_view modified);

  virtual void output();

  /// @todo Return Object?
  static void check_nestable(construct::construct_list_view original, construct::construct_list_view modified,
                             int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified,
                             int & operation);

  static bool is_nestable(std::shared_ptr<const construct> structure_one,
                          std::shared_ptr<const construct> structure_two);


  static bool is_same_nestable(std::shared_ptr<const construct> structure_one,
                               std::shared_ptr<const construct> structure_two);

  static bool is_better_nested(construct::construct_list_view original, construct::construct_list_view modified);
};

#endif
