// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_local.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <input_local_utilities.hpp>

#include <fstream>

namespace srcdiff {

local_input_context* open_local(const char* uri) {

  local_input_context* context = new local_input_context;
  context->in.open(uri);

  return context->in ? context : (delete context, nullptr);

}

ssize_t read_local(void* context, void* buffer, size_t len) {

  local_input_context* ctx = (local_input_context*)context;
  ctx->in.read((char*)buffer, len);

  return ctx->in.gcount();
}

int close_local(void* context) {

  local_input_context* ctx = (local_input_context*)context;
  ctx->in.close();
  delete ctx;

  return 1;
}

}
