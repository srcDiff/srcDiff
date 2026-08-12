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

#include <uri_stream.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>

#include <cstring>
#include <cassert>

namespace srcdiff {

input_source_local::input_source_local(srcml_archive* archive, const std::string& output_filename, const std::string& path)
 : input_source(archive, path), output_filename(output_filename),
   is_initialized(false), input_cache() {

      if(!std::filesystem::exists(base_path)) {
        throw std::string("Input source '" + path + "' could not be opened");
      }
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
      if(input_cache.back() == *output_file) {
          throw std::string("Input source '" + input_cache.back().path().native() + "' same as output filename");
      }
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

// void input_source_local::process_files_from() {

// #define FILELIST_COMMENT '#'

//   try {

//     // translate all the filenames listed in the named file

//     input_context * context = open(options.files_from_name->c_str());
//     uri_stream<input_source_local> uriinput(context);

//     const char * c_line = 0;
//     while ((c_line = uriinput.readline())) {

//       std::string line = c_line;
 
//       int white_length = strspn(line.c_str(), " \t\f");
//       line.erase(0, white_length);

//       // skip blank lines or comment lines
//       if (line.empty() || line[0] == FILELIST_COMMENT) continue;

//       std::string path_original = line.substr(0, line.find('|'));
//       std::string path_modified = line.substr(line.find('|') + 1);

//       file(path_original, path_modified);

//     }

//   } catch (uri_stream_error) {

//     fprintf(stderr, "error: file/URI \'%s\' does not exist.\n", options.files_from_name->c_str());
//     exit(EXIT_FAILURE);

//   }

// #undef FILELIST_COMMENT

// }

input_source_local::input_context * input_source_local::open(const char* uri) const {

  input_context* context = new input_context;
  context->in.open(uri);

  return context->in ? context : (delete context, nullptr);

}

ssize_t input_source_local::read(void* context, void* buffer, size_t len) {

  input_context* ctx = (input_context*)context;
  ctx->in.read((char*)buffer, len);

  return ctx->in.gcount();
}

int input_source_local::close(void* context) {

  input_context* ctx = (input_context*)context;
  ctx->in.close();
  delete ctx;

  return 1;
}

}
