// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_INPUT_SOURCE_HPP
#define INCLUDED_INPUT_SOURCE_HPP

#include <input_stream.hpp>

#include <string>

#include <optional>
#include <memory>

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

namespace srcdiff {

class input_source {
private:

  // handle in input source manager. Maybe
  // bool() return false if not anymore in source. Exception is somethign skipped
  static bool show_input;

  static size_t input_count;
  static size_t input_skipped;
  static size_t input_total;

public:

  input_source(srcml_archive* archive, const std::optional<std::string>& filename);
  virtual ~input_source();

  virtual operator bool() = 0;
  virtual std::unique_ptr<input_stream_base> stream() = 0;
  virtual void next() = 0;

  virtual const char* get_language(const std::string& path);

protected:
  srcml_archive* archive;
  const std::optional<std::string>& filename;
};

}

#endif
