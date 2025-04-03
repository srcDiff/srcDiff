// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <shortest_edit_script_t.hpp>

namespace ses {

const size_t shortest_edit_script_t::SIZE_THRESHOLD = 20480;
shortest_edit_script_t::shortest_edit_script_t(int (*compare)(const void* item_one, const void* item_two, const void* context),
                                               const void* (*accessor)(int index, const void* structure, const void* context),
                                               const void* context,
                                               int threshold)
    : edit_script(nullptr),
      context(context),
      compare(compare),
      accessor(accessor),
      threshold(threshold) { }

shortest_edit_script_t::~shortest_edit_script_t() {

  if(edit_script) {
    free_shortest_edit_script(edit_script);
  }

}

size_t shortest_edit_script_t::get_size_threshold() {
  return SIZE_THRESHOLD;
}

int shortest_edit_script_t::compute(const void* structure_one, int size_one, const void* structure_two, int size_two) {
    return shortest_edit_script_hybrid(structure_one, size_one, structure_two, size_two, &edit_script,
                                       compare,
                                       accessor,
                                       context, threshold);
}

edit_t * shortest_edit_script_t::script() const {

  return edit_script;

}

void shortest_edit_script_t::script(edit_t* edit_script) {
  this->edit_script = edit_script;
}

}
