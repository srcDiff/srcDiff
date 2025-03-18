// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file nested_differ.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NESTED_DIFFER_HPP
#define INCLUDED_NESTED_DIFFER_HPP

#include <differ.hpp>
#include <srcml_nodes.hpp>
#include <construct.hpp>
#include <measurer.hpp>

namespace srcdiff {

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

class nested_differ : public differ {

protected:

  int operation;

private:

static bool check_nestable_predicate(construct::construct_list_view construct_list_outer,
                                     construct::construct_list_view construct_list_inner);
static std::tuple<std::vector<int>, int, int> check_nestable_inner(construct::construct_list_view parent_list, construct::construct_list_view child_list);

public:

  nested_differ(std::shared_ptr<output_stream> out, const construct::construct_list_view original, const construct::construct_list_view modified, int operation);

  virtual void output();

  static nest_result check_nestable(construct::construct_list_view original, construct::construct_list_view modified);

  static bool is_better_nested(construct::construct_list_view original, construct::construct_list_view modified);
};

}

#endif
