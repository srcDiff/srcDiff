// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_local.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_LOCAL_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_LOCAL_HPP

#include <input_source.hpp>

#include <fstream>

#include <sys/stat.h>
#include <filesystem>

namespace srcdiff {

class input_source_local : public input_source {
public:
  input_source_local(srcml_archive* archive, const std::string& output_filename, const std::string& path);
  virtual ~input_source_local();

  virtual operator bool() override;
  virtual std::filesystem::directory_entry   entry() override;
  virtual std::shared_ptr<input_stream_base> stream() override;
  virtual void next() override;

  std::shared_ptr<input_stream_base> file();
  std::shared_ptr<input_stream_base> directory();
  //void files_from();

  void expand_directory();

  struct input_context {
    std::ifstream in;
  };

  input_context* open(const char* uri) const;
  static ssize_t read(void* context, void* buffer, size_t len);
  static int     close(void* context);

private:
  const std::string& output_filename;
  std::optional<std::filesystem::directory_entry> output_file;
  std::filesystem::path base_path;

  bool is_initialized;
  // stack for dfs
  std::list<std::filesystem::directory_entry> input_cache;
};

}

#endif
