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
#include <filesystem>

namespace srcdiff {

class input_source {
public:

  input_source(srcml_archive* archive);
  virtual ~input_source();

  virtual operator bool() = 0;
  virtual std::filesystem::directory_entry   entry()  = 0;
  virtual std::shared_ptr<input_stream_base> stream() = 0;
  virtual void next() = 0;

  virtual const char* get_language(const std::string& path);

protected:
  srcml_archive* archive;
};

}

#endif
