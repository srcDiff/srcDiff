// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_local.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <input_source_local.hpp>

#include <srcml.h>

#include <input_stream.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <cstring>
#include <cassert>

namespace srcdiff {

input_source_local::input_source_local(srcml_archive* archive, const std::string& output_filename, const std::string& path)
 : input_source(archive, path), output_filename(output_filename),
   is_initialized(false), input_cache() {
      input_cache.emplace_back(base_path);
}

input_source_local::~input_source_local() {
}

input_source_local::operator bool() {

  if(!is_initialized) { 

    // delayed as output filename may not be known when this is created
    // next calls this first to make sure this happens
    if(!output_file) {
      output_file = std::filesystem::directory_entry(output_filename);
      is_initialized = true;
    }
  }

  return !input_cache.empty();
}

void input_source_local::next() {

  if(!*this) return;

  if(std::filesystem::is_directory(input_cache.back())) {
    expand_directory();
  } else {
    input_cache.pop_back();
  }
}

std::filesystem::directory_entry input_source_local::entry() {
  if(!*this) return std::filesystem::directory_entry();
  return input_cache.back();
}

std::shared_ptr<input_stream_base> input_source_local::stream() {
  if(!*this) return std::make_shared<input_stream_base>(std::optional<std::string>());
  if(input_cache.back().is_directory()) return directory();

  return file();
}

std::shared_ptr<input_stream_base> input_source_local::file() {
  assert(!input_cache.back().is_directory());

  const std::string& path = input_cache.back().path().native();

  // throw instead?
  const char* language_string = get_language(path);
  if(language_string == SRCML_LANGUAGE_NONE) return std::make_shared<input_stream_base>(path);

  return std::make_shared<input_stream<input_source_local>>(*this, path, archive, language_string);
}

std::shared_ptr<input_stream_base> input_source_local::directory() {
  std::string path = input_cache.back().path().native();
  return std::make_shared<input_stream<input_source_local>>(*this, path, archive, nullptr);
}

void input_source_local::expand_directory() {

  std::filesystem::directory_entry directory = input_cache.back();
  assert(directory.is_directory());

  input_cache.pop_back();

  std::vector<std::filesystem::directory_entry> entries;
  for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(directory)){
    entries.push_back(entry);
  }
  std::sort(entries.begin(), entries.end(), std::greater{});

  // process all non-directory files
  for(const std::filesystem::directory_entry& entry : entries) {

    // process directories last
    if((!entry.is_directory() || entry == *output_file)) {
      continue;
    }
    input_cache.push_back(entry);
  }

  for(const std::filesystem::directory_entry& entry : entries) {

    // process directories last
    if((entry.is_directory() || entry == *output_file)) {
      continue;
    }
    input_cache.push_back(entry);
  }

}

input_source_local::input_context* input_source_local::open(const char* uri) {
  return srcdiff::open_local(uri);
}

ssize_t input_source_local::read(void* context, void* buffer, size_t len) {
  return srcdiff::read_local(context, buffer, len);
}

int input_source_local::close(void* context) {
  return srcdiff::close_local(context);
}

}
