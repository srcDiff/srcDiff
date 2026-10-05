// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source.cpp
 *
 * @copyright Copyright (C) 2015-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <input_source.hpp>

namespace srcdiff {

input_source::input_source(srcml_archive* archive, const std::string& path) 
  : archive(archive), base_path(path) {
}

input_source::~input_source() {
}

bool input_source::is_single_source() const {
  return false;
}

const char* input_source::get_language(const std::string& path) {

  const char* archive_language = srcml_archive_get_language(archive);
  if (archive_language) {
    return archive_language;
  } else {
    return srcml_archive_check_extension(archive, path.c_str());
  }

}

}
