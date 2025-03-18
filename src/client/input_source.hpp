// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NPUT_SOURCE_HPP
#define INCLUDED_NPUT_SOURCE_HPP

#include <options.hpp>
#include <translator.hpp>
#include <view.hpp>

#include <string>

#include <optional>
#include <memory>

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

class input_source {

protected:

  const srcdiff::client::options& options;
  std::unique_ptr<srcdiff::translator> translator;
  std::unique_ptr<view_t> view;

  int directory_length_original;
  int directory_length_modified;

  std::optional<std::string> unit_version;

private:

  static bool show_input;

  static size_t input_count;
  static size_t input_skipped;
  static size_t input_total;

public:

  input_source(const srcdiff::client::options & options);
  virtual ~input_source();

  virtual void consume() = 0;
  virtual const char * get_language(const std::optional<std::string> & path_original, const std::optional<std::string> & path_modified);

  virtual void file(const std::optional<std::string> & path_original,
                    const std::optional<std::string> & path_modified);
  virtual void directory(const std::optional<std::string> & directory_original,
                         const std::optional<std::string> & directory_modified);
  virtual void files_from();

  virtual std::string process_file(const std::optional<std::string> & path_original,
                                   const std::optional<std::string> & path_modified) = 0;
  virtual void process_directory(const std::optional<std::string> & directory_original,
                                 const std::optional<std::string> & directory_modified) = 0;
  virtual void process_files_from() = 0;

};

#endif
