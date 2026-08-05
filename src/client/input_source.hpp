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

protected:

  srcml_archive* archive;
  const std::optional<std::string>& filename;

  int directory_length_original;
  int directory_length_modified;

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

  // virtual bool is_directory() = 0;
  virtual operator bool() = 0;
  virtual std::unique_ptr<input_stream_base> next() = 0;
  virtual const char* get_language(const std::string& path);

  // virtual std::unique_ptr<input_stream_base> file(const std::string& path) = 0;
  // virtual void directory(const std::optional<std::string>& directory_original,
  //                        const std::optional<std::string>& directory_modified);
  // virtual void files_from();

  // virtual input_stream_base process_file(const std::optional<std::string>& path_original,
  //                                        const std::optional<std::string>& path_modified) = 0;
  // virtual void process_directory(const std::optional<std::string>& directory_original,
  //                                const std::optional<std::string>& directory_modified) = 0;
  // virtual void process_files_from() = 0;

};

}

#endif
