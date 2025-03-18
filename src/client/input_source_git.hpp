// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_git.hpp
 *
 * @copyright Copyright (C) 2015-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#if GIT

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP

#include <input_source_local.hpp>

#include <filesystem>

class input_source_git : public input_source_local {

protected:

private:

  long arg_max;

  std::filesystem::path original_clone_path;
  std::filesystem::path modified_clone_path;
  bool clean_path;

public:

  input_source_git(const options & options);

  virtual ~input_source_git();

  virtual void consume();

  virtual std::string process_file(const std::optional<std::string> & path_original,
                                   const std::optional<std::string> & path_modified);
  virtual void process_directory(const std::optional<std::string> & directory_original,
                                 const std::optional<std::string> & directory_modified);
  virtual void process_files_from();

};

#endif

#endif
