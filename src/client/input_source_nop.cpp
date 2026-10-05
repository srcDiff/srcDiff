// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_nop.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <input_source_nop.hpp>

#include <srcml.h>

#include <input_stream.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>

#include <cstring>
#include <cassert>

namespace srcdiff {

input_source_nop::input_source_nop(srcml_archive* archive, const std::string& path)
 : input_source(archive, path) {

    if(!std::filesystem::exists(base_path)) {
      throw std::string("Input source '" + path + "' could not be opened");
    }
}

input_source_nop::~input_source_nop() {
}

input_source_nop::operator bool() {
  return true;
}

void input_source_nop::next() {
  return;
}

std::filesystem::directory_entry input_source_nop::entry() {
  return std::filesystem::directory_entry();
}

std::shared_ptr<input_stream_base> input_source_nop::stream() {
  return std::make_shared<input_stream_base>(std::optional<std::string>());
}

bool input_source_nop::is_single_source() const {
  return true;
}

}
