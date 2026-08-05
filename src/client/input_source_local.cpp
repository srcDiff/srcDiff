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

#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace srcdiff {

input_source_local::input_source_local(srcml_archive* archive, const std::string& path)
 : input_source(archive), path(path) {
  // output_file = std::filesystem::directory_entry(options.output_filename);
}

input_source_local::~input_source_local() {
}

/// @todo implement
input_source_local::operator bool() {
  return true;
}

// determines whether the input path(s) exist and whether they are files or
// directories, and then processes them
std::unique_ptr<input_stream_base> input_source_local::next() {

  // if (options.files_from_name) {
    
  //   files_from();
  
  // } else {

      std::filesystem::path path_inspector(path);

      if(!std::filesystem::exists(path_inspector)) {
        throw std::string("Input source '" + path + "' could not be opened");
      }

      // if (std::filesystem::is_directory(original)) {

      //   srcml_archive_enable_solitary_unit(archive);

      //   if (!srcml_archive_get_url(archive)) {

      //     std::string directory_path = this->original == this->modified ? this->original : this->original + '|' + this->modified;
      //     srcml_archive_set_url(archive, directory_path.c_str());
      //   }

      //   directory_length_original = this->original.back() == '/' ? this->original.size() : this->original.size() + 1;
      //   directory_length_modified = this->modified.back() == '/' ? this->modified.size() : this->modified.size() + 1;

      //   directory(this->original, this->modified);
      // } else {
      return file(path);
      // }
  // }
}

std::unique_ptr<input_stream_base> input_source_local::file(const std::string& path) {

  // if(path_modified == "") {
  //   srcml_archive* read_archive = srcml_archive_create();
  //   srcml_archive_read_open_filename(read_archive, path_original->c_str());
  //   srcml_unit* unit = srcml_archive_read_unit(read_archive);
  //   srcml_archive_close(read_archive);
  //   srcml_archive_free(read_archive);
  //   return unit;
  // }

  const char* language_string = get_language(path);

  if(language_string == SRCML_LANGUAGE_NONE) return std::unique_ptr<input_stream_base>();

  // std::string path_one = path_original ? *path_original : std::string();
  // std::string path_two = path_modified ? *path_modified : std::string();

  // std::string unit_filename = !path_one.empty() ? path_one.substr(directory_length_original) : path_one;
  // std::string filename_two  = !path_two.empty() ? path_two.substr(directory_length_modified) : path_two;
  // if(path_two.empty() || unit_filename != filename_two) {

  //   unit_filename += "|";
  //   unit_filename += filename_two;

  // }

  return std::move(std::make_unique<input_stream<input_source_local>>(*this, path, archive, language_string));

}

// void input_source_local::process_directory(const std::optional<std::string>& directory_original,
//                                            const std::optional<std::string>& directory_modified) {

//   std::filesystem::directory_entry original_entry(directory_original ? *directory_original : "");
//   std::filesystem::directory_entry modified_entry(directory_modified ? *directory_modified : "");

//   if (!original_entry.is_directory() && !modified_entry.is_directory()) {
//     throw std::string("Directories '" + (directory_original ? *directory_original : "")
//       + "' and '" + (directory_modified ? *directory_modified : "") + "' could not be opened");
//   }

//   std::vector<std::filesystem::directory_entry> original_contents;
//   if(original_entry.is_directory()) {
//     for (std::filesystem::directory_entry e : std::filesystem::directory_iterator(original_entry)){
//       original_contents.push_back(e);
//     }
//     std::sort(original_contents.begin(), original_contents.end());
//   }

//   std::vector<std::filesystem::directory_entry> modified_contents;
//   if(modified_entry.is_directory()) {
//     for (std::filesystem::directory_entry e : std::filesystem::directory_iterator(modified_entry)){
//       modified_contents.push_back(e);
//     }
//     std::sort(modified_contents.begin(), modified_contents.end());
//   }

//   // process all non-directory files
//   std::vector<std::filesystem::directory_entry>::iterator in_original = original_contents.begin();
//   std::vector<std::filesystem::directory_entry>::iterator in_modified = modified_contents.begin();
//   while (in_original != original_contents.end() || in_modified != modified_contents.end()) {

//     // if we're not at the end of each file list, check to make sure the current
//     // entry in the file list is not a directory and is not our output file;
//     // skip the current entry otherwise
//     if(in_original != original_contents.end() &&
//         (in_original->is_directory() || *in_original == output_file)) {
//       ++in_original;
//       continue;
//     }

//     if(in_modified != modified_contents.end() &&
//         (in_modified->is_directory() || *in_modified == output_file)) {
//       ++in_modified;
//       continue;
//     }

//     // if we're not at the end of the original contents list, but: we are at the
//     // end of the modified files list, or in_original is less than the current
//     // entry in the modified files list, then in_original has no match; process
//     // it and then go to the next one in its list
//     if (in_original != original_contents.end() &&
//         (in_modified == modified_contents.end() ||
//         in_original->path().filename() < in_modified->path().filename())) {
//       file(in_original->path().string(), std::optional<std::string>());
//       ++in_original;
//     } else if(in_original == original_contents.end() || 
//         in_modified->path().filename() < in_original->path().filename()) {
//       // similarly, process in_modified if it doesn't match in_original
//       file(std::optional<std::string>(), in_modified->path().string());
//       ++in_modified;
//     } else {
//       // having dealt with the problematic cases, we can compare two matching files
//       file(in_original->path().string(), in_modified->path().string());
//       ++in_original;
//       ++in_modified;
//     }
//   }

//   in_original = original_contents.begin();
//   in_modified = modified_contents.begin();

//   while (in_original != original_contents.end() || in_modified != modified_contents.end()) {

//     if (in_original != original_contents.end() && !in_original->is_directory()) {
//       ++in_original;
//       continue;
//     }

//     if (in_modified != modified_contents.end() && !in_modified->is_directory()) {
//       ++in_modified;
//       continue;
//     }

//     // same logic as processing files
//     if (in_original != original_contents.end() &&
//         (in_modified == modified_contents.end() ||
//         in_original->path().filename() < in_modified->path().filename())) {
//       directory(in_original->path().string(), std::optional<std::string>());
//       ++in_original;
//     } else if(in_original == original_contents.end() || 
//         in_modified->path().filename() < in_original->path().filename()) {
//       directory(std::optional<std::string>(), in_modified->path().string());
//       ++in_modified;
//     } else {
//       // having dealt with the problematic cases, we can compare two matching
//       // directories
//       directory(
//         in_original->path().string(),
//         in_modified->path().string()
//       );
//       ++in_original;
//       ++in_modified;
//     }
//   }
// }


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
