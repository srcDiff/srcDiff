// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_diff.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_DIFF_HPP
#define INCLUDED_SRCDIFF_DIFF_HPP

#include <output_stream.hpp>
#include <construct.hpp>

#include <vector>

class srcdiff_diff {

protected:
  std::shared_ptr<srcdiff::output_stream> out;

  const construct::construct_list_view original;
  const construct::construct_list_view modified;

public:
  srcdiff_diff(std::shared_ptr<srcdiff::output_stream> out, const construct::construct_list_view original, const construct::construct_list_view modified);


  virtual void output();
  virtual void output_pure(int end_original, int end_modified);
  virtual void output_change_whitespace(int end_original, int end_modified);
  virtual void output_replace_inner_whitespace(int start_original, int end_original,
                                               int start_modified, int end_modified,
                                               int common_offset);
};

#endif
