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

input_source_local::input_source_local(srcml_archive* archive, const std::optional<std::string>& filename,
                                       const std::string& output_filename, const std::string& path)
 : input_source(archive, filename), output_filename(output_filename), path(std::filesystem::path(path)) {

      if(!std::filesystem::exists(path)) {
        throw std::string("Input source '" + path + "' could not be opened");
      }
      input_cache.emplace_back(path);
}

input_source_local::~input_source_local() {
}

input_source_local::operator bool() {

  // delayed as output filename may not be known when this is created
  // next calls this first to make sure this happens
  if(!output_file) {
    output_file = std::filesystem::directory_entry(output_filename);
    if(input_cache.back() == *output_file) {
        throw std::string("Input source '" + path.native() + "' same as output filename");
    }
  }

  while(!input_cache.empty() && std::filesystem::is_directory(input_cache.back())) {
    directory();
  }

  return !input_cache.empty();
}

// determines whether the input path(s) exist and whether they are files or
// directories, and then processes them
std::unique_ptr<input_stream_base> input_source_local::next() {
  if(!*this) return std::unique_ptr<input_stream_base>();

  // if (options.files_from_name) {
    
  //   files_from();
  
  // } else {

  return file();

}

std::unique_ptr<input_stream_base> input_source_local::file() {

  // if(path_modified == "") {
  //   srcml_archive* read_archive = srcml_archive_create();
  //   srcml_archive_read_open_filename(read_archive, path_original->c_str());
  //   srcml_unit* unit = srcml_archive_read_unit(read_archive);
  //   srcml_archive_close(read_archive);
  //   srcml_archive_free(read_archive);
  //   return unit;
  // }

  assert(!input_cache.back().is_directory());

  const std::string& path = input_cache.back().path().native();
  const char* language_string = get_language(path);
  // throw instead?
  if(language_string == SRCML_LANGUAGE_NONE) return std::unique_ptr<input_stream_base>();

  return std::move(std::make_unique<input_stream<input_source_local>>(*this, path, archive, language_string, filename));
}

void input_source_local::directory() {

  std::filesystem::directory_entry directory = input_cache.back();
  assert(directory.is_directory());

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

  //   // if we're not at the end of the original contents list, but: we are at the
  //   // end of the modified files list, or in_original is less than the current
  //   // entry in the modified files list, then in_original has no match; process
  //   // it and then go to the next one in its list
  //   if (in_original != entry.end() &&
  //       (in_modified == modified_contents.end() ||
  //       in_original->path().filename() < in_modified->path().filename())) {
  //     file(in_original->path().string(), std::optional<std::string>());
  //     ++in_original;
  //   } else if(in_original == entry.end() || 
  //       in_modified->path().filename() < in_original->path().filename()) {
  //     // similarly, process in_modified if it doesn't match in_original
  //     file(std::optional<std::string>(), in_modified->path().string());
  //     ++in_modified;
  //   } else {
  //     // having dealt with the problematic cases, we can compare two matching files
  //     file(in_original->path().string(), in_modified->path().string());
  //     ++in_original;
  //     ++in_modified;
  //   }
  // }

  // in_original = entry.begin();
  // in_modified = modified_contents.begin();

  // while (in_original != entry.end() || in_modified != modified_contents.end()) {

  //   if (in_original != entry.end() && !in_original->is_directory()) {
  //     ++in_original;
  //     continue;
  //   }

  //   if (in_modified != modified_contents.end() && !in_modified->is_directory()) {
  //     ++in_modified;
  //     continue;
  //   }

  //   // same logic as processing files
  //   if (in_original != entry.end() &&
  //       (in_modified == modified_contents.end() ||
  //       in_original->path().filename() < in_modified->path().filename())) {
  //     directory(in_original->path().string(), std::optional<std::string>());
  //     ++in_original;
  //   } else if(in_original == entry.end() || 
  //       in_modified->path().filename() < in_original->path().filename()) {
  //     directory(std::optional<std::string>(), in_modified->path().string());
  //     ++in_modified;
  //   } else {
  //     // having dealt with the problematic cases, we can compare two matching
  //     // directories
  //     directory(
  //       in_original->path().string(),
  //       in_modified->path().string()
  //     );
  //     ++in_original;
  //     ++in_modified;
  //   }
  // }
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
