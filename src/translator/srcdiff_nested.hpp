#ifndef INCLUDED_SRCDIFF_NESTED_HPP
#define INCLUDED_SRCDIFF_NESTED_HPP

#include <srcml_nodes.hpp>
#include <construct.hpp>
#include <srcdiff_many.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcdiff_measure.hpp>

struct nest_result {
  nest_result(int start_original = 0, int end_original = 0,
              int start_modified = 0, int end_modified = 0,
              int operation = SES_COMMON)
    : start_original(start_original), end_original(end_original),
      start_modified(start_modified), end_modified(end_modified),
    operation(operation) {}
  operator bool() { return operation != SES_COMMON; }

  int start_original, end_original;
  int start_modified, end_modified;
  int operation;
};

class srcdiff_nested : public srcdiff_diff {

protected:

  int operation;

private:

static bool check_nestable_predicate(construct::construct_list_view construct_list_outer,
                                     construct::construct_list_view construct_list_inner);
static std::tuple<std::vector<int>, int, int> check_nestable_inner(construct::construct_list_view parent_list, construct::construct_list_view child_list);

public:

  srcdiff_nested(std::shared_ptr<srcdiff_output> out, const construct::construct_list_view original, const construct::construct_list_view modified, int operation);

  virtual void output();

  static nest_result check_nestable(construct::construct_list_view original, construct::construct_list_view modified);

  static bool is_nestable(std::shared_ptr<const construct> structure_one,
                          std::shared_ptr<const construct> structure_two);


  static bool is_same_nestable(std::shared_ptr<const construct> structure_one,
                               std::shared_ptr<const construct> structure_two);

  static bool is_better_nested(construct::construct_list_view original, construct::construct_list_view modified);
};

#endif
