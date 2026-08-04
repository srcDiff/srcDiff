// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_manager.hpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_INPUT_SOURCE_MANAGER_HPP
#define INCLUDED_INPUT_SOURCE_MANAGER_HPP

#include <input_source.hpp>
#include <list>

#include <input_stream_manager.hpp>
#include <deltor.hpp>
#include <view.hpp>

#include <string>

#include <optional>
#include <memory>

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

namespace srcdiff {

class input_source_manager {

public:

  input_source_manager();
  ~input_source_manager();

  void append_source(std::unique_ptr<input_source> input);

  operator bool();
  void consume();

private:

  std::list<std::unique_ptr<input_source>> input_sources;

  // bool show_input;

  // size_t input_count;
  // size_t input_skipped;
  // size_t input_total;

  // input_stream_manager manager;
  // std::unique_ptr<class deltor> deltor;
  // std::unique_ptr<view_t> view;

  // std::optional<std::string> unit_version;
};

}

#endif
