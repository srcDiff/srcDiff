// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file null_input_source.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NULL_INPUT_SOURCE_HPP
#define INCLUDED_NULL_INPUT_SOURCE_HPP

#include <string>
#include <input_source.hpp>

#include <optional>
#include <memory>
#include <filesystem>

namespace srcdiff {

class null_input_source : public input_source {
public:

  null_input_source(srcml_archive* archive) : input_source(archive, "") {}
  virtual ~null_input_source() {}

  virtual operator bool() { return false; }
  virtual std::filesystem::directory_entry   entry()  { return std::filesystem::directory_entry();    }
  virtual std::shared_ptr<input_stream_base> stream() { return std::shared_ptr<input_stream_base>(); }
  virtual void next() {}

};

}

#endif
