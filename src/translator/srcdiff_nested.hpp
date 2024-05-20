/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
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

  srcdiff_nested(const srcdiff_many & diff, int start_original, int end_original, int start_modified, int end_modified, int operation);

  void output_inner(srcdiff_whitespace & whitespace,
                  const construct::construct_list & construct_list_outer,
                  int start_outer,
                  int end_outer,
                  const construct::construct_list & construct_list_inner,
                  int start_inner,
                  int end_inner,
                  int operation);

  virtual void output();

  static void check_nestable(const construct::construct_list & construct_list_original, int start_original, int end_original
                 , const construct::construct_list & construct_list_modified, int start_modified, int end_modified
                 , int & start_nest_original, int & end_nest_original, int & start_nest_modified, int & end_nest_modified
                 , int & operation);

  static bool is_nestable(std::shared_ptr<const construct> structure_one,
                          std::shared_ptr<const construct> structure_two);


  static bool is_same_nestable(std::shared_ptr<const construct> structure_one,
                               std::shared_ptr<const construct> structure_two);

  static bool is_better_nested(const construct::construct_list & construct_list_original, int start_pos_original,
                               const construct::construct_list & construct_list_modified, int start_pos_modified);
};

#endif
