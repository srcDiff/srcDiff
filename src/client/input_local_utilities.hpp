// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_local.hpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_INPUT_LOCAL_UTILITIES_HPP
#define INCLUDED_SRCDIFF_INPUT_LOCAL_UTILITIES_HPP

#include <fstream>

namespace srcdiff {

  struct local_input_context {
    std::ifstream in;
  };

  local_input_context* open_local(const char* uri);
  ssize_t read_local(void* context, void* buffer, size_t len);
  int     close_local(void* context);

}

#endif
