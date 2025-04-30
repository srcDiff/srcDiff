// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <shortest_edit_script.hpp>

namespace ses {

const size_t shortest_edit_script::SIZE_THRESHOLD = 20480;
shortest_edit_script::shortest_edit_script(int (*compare)(const void* item_one, const void* item_two, const void* context),
                                               const void* (*accessor)(int index, const void* structure, const void* context),
                                               const void* context,
                                               int threshold)
    : context(context),
      compare(compare),
      accessor(accessor),
      threshold(threshold) {
}

shortest_edit_script::~shortest_edit_script() {
}

size_t shortest_edit_script::get_size_threshold() {
  return SIZE_THRESHOLD;
}

edit_list shortest_edit_script::compute(const void* structure_one, int size_one, const void* structure_two, int size_two) {
  edit_t* edit_script = nullptr;
  int distance = shortest_edit_script_hybrid(structure_one, size_one, structure_two, size_two, &edit_script,
                                     *compare.target<int (*)(void const*, void const*, void const*)>(),
                                     *accessor.target<void const* (*)(int, void const*, void const*)>(),
                                     context, threshold);
  if(distance < 0) throw std::logic_error("Error computing shortest edit script");

  static std::unordered_map<edit_operations, edit_operation> edit_operation_map
                = { {SES_DELETE, ses::DELETE}, {SES_INSERT, ses::INSERT}, {SES_CHANGE, ses::CHANGE}};

  edit_list edits;
  for(edit_t* edit = edit_script; edit != nullptr; edit = edit->next) {
    edits.emplace_back(edit_operation_map[edit->operation],
                       edit->offset_one, edit->length_one,
                       edit->offset_two, edit->length_two
    );
  }

  free_shortest_edit_script(edit_script);

  return edits;
}

}
