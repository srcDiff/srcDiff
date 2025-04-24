// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SHORTEST_EDIT_SCRIPT_T_HPP
#define INCLUDED_SHORTEST_EDIT_SCRIPT_T_HPP

#include <shortest_edit_script.h>

#include <edit_list.hpp>

#include <vector>
#include <iostream>
#include <cmath>

#include <optional>
#include <functional>

namespace ses {

class shortest_edit_script {
  
protected:

  typedef std::function<int (const void * item_one, const void * item_two, const void * context)> compare_func;
  typedef std::function<const void* (int index, const void * structure, const void * context)>    accessor_func;

  const static size_t SIZE_THRESHOLD;

  const void* context;
  compare_func compare;
  accessor_func accessor;

  int threshold;

public:

  shortest_edit_script(int (*compare)(const void * item_one, const void * item_two, const void * context),
                       const void * (*accessor)(int index, const void * structure, const void * context),
                       const void * context, int threshold = 1000);

  virtual ~shortest_edit_script();

  static size_t get_size_threshold();


  virtual edit_list compute(const void * structure_one, int size_one, const void * structure_two, int size_two);

};

}
#endif
