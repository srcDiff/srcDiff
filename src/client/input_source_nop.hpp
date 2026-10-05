// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_nop.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_NOP_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_NOP_HPP

#include <input_source.hpp>
#include <input_local_utilities.hpp>

#include <fstream>

#include <sys/stat.h>
#include <filesystem>

namespace srcdiff {

class input_source_nop : public input_source {
public:
  input_source_nop(srcml_archive* archive, const std::string& path);
  virtual ~input_source_nop();

  virtual operator bool() override;

  virtual std::filesystem::directory_entry   entry() override;
  virtual std::shared_ptr<input_stream_base> stream() override;
  virtual void next() override;

  virtual bool is_single_source() const override;

private:
};

}

#endif
