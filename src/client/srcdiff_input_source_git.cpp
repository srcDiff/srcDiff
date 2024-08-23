// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_input_source_git.cpp
 *
 * @copyright Copyright (C) 2015-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_input_source_git.hpp>

#include <srcdiff_input.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <optional>

#include <cstdio>
#include <unistd.h>

#include <uri_stream.hpp>

#ifdef _WIN32
const char PATH_DELIMITER = ';';
#else
const char PATH_DELIMITER = ':';
#endif

std::optional<std::filesystem::path> search_path(const std::string & val) {

    std::string path = std::getenv("PATH");
    std::istringstream path_stream(path);
    std::vector<std::string> paths;
    for (std::string temp; std::getline(path_stream, temp, PATH_DELIMITER);) {
        paths.push_back(temp);
    }
    for (auto & p : paths) {
        std::string val_path = p + std::filesystem::path::preferred_separator + val;
        if (std::filesystem::is_regular_file(val_path)) {
            return std::optional<std::filesystem::path>(val_path);
        }
    }
    return std::optional<std::filesystem::path>();

}

std::filesystem::path unique_path() {

  return std::filesystem::temp_directory_path() / std::tmpnam(nullptr);

}

srcdiff_input_source_git::srcdiff_input_source_git(const srcdiff_options & options)
  : srcdiff_input_source_local(options), clean_path(false) {

  // half is what is used by gxargs
  arg_max = sysconf(_SC_ARG_MAX) / 2;

  std::string quiet_flag;
  if(is_option(options.flags, OPTION_QUIET)) quiet_flag = "--quiet ";

  std::string::size_type comma_pos = options.git_url->find(',');
  if(comma_pos != std::string::npos) {

    original_clone_path = options.git_url->substr(0, comma_pos);
    modified_clone_path = options.git_url->substr(comma_pos + 1);

  } else {

    clean_path = true;

    original_clone_path = unique_path().native();
    modified_clone_path = unique_path().native();
  
    std::string clone_original_command("git clone " + quiet_flag + *options.git_url + " "
                                      + original_clone_path.native());
    FILE * clone_original_process = popen(clone_original_command.c_str(), "r");
    int clone_original_error = pclose(clone_original_process);
    if(clone_original_error) throw std::string("Unable to clone " + original_clone_path.native());

    std::string clone_modified_command("git clone " + quiet_flag + *options.git_url + " "
                                      + modified_clone_path.native());
    FILE * clone_modified_process = popen(clone_modified_command.c_str(), "r");
    int clone_modified_error = pclose(clone_modified_process);
    if(clone_modified_error) throw std::string("Unable to clone " + modified_clone_path.native());

  }

  std::string checkout_original_command(search_path("git")->native() + " -C " + original_clone_path.native() + " checkout " + quiet_flag + options.git_revision_one);
  std::string checkout_modified_command(search_path("git")->native() + " -C " + modified_clone_path.native() + " checkout " + quiet_flag + options.git_revision_two);
  if(options.files_from_name) {

    std::string original_files, modified_files;
    std::ifstream input(options.files_from_name->c_str());
    std::string line;
    while(getline(input, line, '\n'), input) {

      // skip over whitespace
      // TODO:  Other types of whitespace?  backspace?
      int white_length = strspn(line.c_str(), " \t\f");

      line.erase(0, white_length);

      // skip blank lines or comment lines
      if (line[0] == '\0' || line[0] == '#')
        continue;

      // remove any end whitespace
      // TODO:  Extract function, and use elsewhere
      for (int i = line.size() - 1; i != 0; --i) {
        if (isspace(line[i]))
          line[i] = 0;
        else
          break;

      }

      std::string::size_type sep_pos = line.find('|');
      std::string path_original = line.substr(0, sep_pos);
      std::string path_modified = line.substr(sep_pos + 1);

      if(!path_original.empty()) {
        original_files += " \"" + path_original + '"';
      }

      if(!path_modified.empty()) {
        modified_files += " \"" + path_modified + '"';
      }

    }

    if((checkout_original_command.size() + original_files.size()) <= arg_max) {
      checkout_original_command += original_files;
    }

    if((checkout_modified_command.size() + modified_files.size()) <= arg_max) {
      checkout_modified_command += modified_files;
    }

  }

  FILE* checkout_original_process = popen(checkout_original_command.c_str(), "r");
  int checkout_original_error = pclose(checkout_original_process);
  if (checkout_original_error) throw std::string("Unable to checkout " + options.git_revision_one);

  FILE* checkout_modified_process = popen(checkout_modified_command.c_str(), "r");
  int checkout_modified_error = pclose(checkout_modified_process);
  if (checkout_modified_error) throw std::string("Unable to checkout " + options.git_revision_two);
}

srcdiff_input_source_git::~srcdiff_input_source_git() {

  if(clean_path) {

    std::string remove_original_command("rm -rf ");
    remove_original_command += original_clone_path.native();

    FILE * remove_original_process = popen(remove_original_command.c_str(), "r");
    pclose(remove_original_process);

    std::string remove_modified_command("rm -rf ");
    remove_modified_command += modified_clone_path.native();

    FILE * remove_modified_process = popen(remove_modified_command.c_str(), "r");
    pclose(remove_modified_process);

  }

}

void srcdiff_input_source_git::consume() {

  if(options.files_from_name) {
    files_from();
  } else if(options.input_pairs.size()) {
    for(std::pair<std::string, std::string> input_pair : options.input_pairs) {
      file(input_pair.first, input_pair.second);
    }
  } else {

    srcml_archive_enable_solitary_unit(options.archive);
    directory(std::string(), std::string());
  }

}

std::string srcdiff_input_source_git::process_file(const std::optional<std::string> & path_original,
                                                   const std::optional<std::string> & path_modified) {

  const char * language_string = get_language(path_original, path_modified);

  if(language_string == SRCML_LANGUAGE_NONE) return;

  std::string path_one = path_original ? *path_original : std::string();
  std::string path_two = path_modified ? *path_modified : std::string();

  std::string path_one_full = path_original ? original_clone_path.native() + PATH_SEPARATOR + *path_original : std::string();
  std::string path_two_full = path_modified ? modified_clone_path.native() + PATH_SEPARATOR + *path_modified : std::string();

  std::string unit_filename = !path_one.empty() ? path_one.substr(directory_length_original) : path_one;
  std::string filename_two =  !path_two.empty() ? path_two.substr(directory_length_modified) : path_two;
  if(path_two.empty() || unit_filename != filename_two) {

    unit_filename += "|";
    unit_filename += filename_two;

  }

  srcdiff_input<srcdiff_input_source_local> input_original(options.archive, path_one_full, language_string, options.flags, *this);
  srcdiff_input<srcdiff_input_source_local> input_modified(options.archive, path_two_full, language_string, options.flags, *this);

  return translator->translate(input_original, input_modified, language_string, unit_filename, unit_version);

}

void srcdiff_input_source_git::process_directory(const std::optional<std::string> & directory_original,
                                                 const std::optional<std::string> & directory_modified) {

#ifndef _MSC_BUILD

  // collect the filenames in alphabetical order
  struct dirent ** namelist_original;
  struct dirent ** namelist_modified;

  std::string directory_original_full
    = directory_original ? 
      original_clone_path.native() + PATH_SEPARATOR + *directory_original
      : std::string();

  std::string directory_modified_full
    = directory_modified ? 
      modified_clone_path.native() + PATH_SEPARATOR + *directory_modified
      : std::string();

  int n = scandir(directory_original_full.c_str(), &namelist_original, dir_filter, alphasort);
  int m = scandir(directory_modified_full.c_str(), &namelist_modified, dir_filter, alphasort);

  // TODO:  Fix error handling.  What if one is in error?
  if (n < 0 && m < 0) {

    throw std::string("Directories '" + (directory_original ? *directory_original : "")
      + "' and '" + (directory_modified ? *directory_modified : "") + "' could not be opened");

  } else if(n < 0) {

    throw std::string("Directory '" + (directory_original ? *directory_original : "") + "' could not be opened");

  } else if(m < 0){

    throw std::string("Directory '" + (directory_modified ? *directory_modified : "") + "' could not be opened");


  }

  std::string path_original;
  int basesize_original = 0;

  if(directory_original) {

    // start of path from directory name
   path_original = *directory_original;
   if (!path_original.empty() && path_original.back() != PATH_SEPARATOR) path_original += PATH_SEPARATOR;
   basesize_original = path_original.size();

  }

  std::string path_modified;
  int basesize_modified = 0;

  if(directory_modified) {

    path_modified = *directory_modified;
    if (!path_modified.empty() && path_modified.back() != PATH_SEPARATOR) path_modified += PATH_SEPARATOR;
    basesize_modified = path_modified.size();

  }

  // process all non-directory files
  int i = 0;
  int j = 0;
  while (i < n && j < m) {

    // form the full path
    path_original.replace(basesize_original, std::string::npos, namelist_original[i]->d_name);
    path_modified.replace(basesize_modified, std::string::npos, namelist_modified[j]->d_name);

    std::string path_original_full = original_clone_path.native() + PATH_SEPARATOR + path_original;
    std::string path_modified_full = modified_clone_path.native() + PATH_SEPARATOR + path_modified;

    // skip directories
    if(is_dir(namelist_original[i], path_original_full.c_str()) != 0) {
      ++i;
      continue;
    }

    if(is_dir(namelist_modified[j], path_modified_full.c_str()) != 0) {
      ++j;
      continue;
    }

    // skip over output file
    if (is_output_file(path_original_full.c_str(), outstat) == 1) {
      ++i;
      continue;
    }

    if (is_output_file(path_modified_full.c_str(), outstat) == 1) {
      ++j;
      continue;
    }

    // is this a common, inserted, or deleted file?
    int comparison = strcoll(namelist_original[i]->d_name, namelist_modified[j]->d_name);


    std::optional<std::string> file_path_original;
    std::optional<std::string> file_path_modified;
    if(comparison <= 0) ++i, file_path_original = path_original;
    if(comparison >= 0) ++j, file_path_modified = path_modified;

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(file_path_original, file_path_modified);

  }

  // process all non-directory files that are remaining in the original version
  for ( ; i < n; ++i) {

    // form the full path
    path_original.replace(basesize_original, std::string::npos, namelist_original[i]->d_name);
    std::string path_original_full = original_clone_path.native() + PATH_SEPARATOR + path_original;

    // skip directories
    if(is_dir(namelist_original[i], path_original_full.c_str()) != 0) {
      continue;
    }

    // skip over output file
    if (is_output_file(path_original_full.c_str(), outstat) != 0) {
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(path_original, std::optional<std::string>());

  }

  // process all non-directory files that are remaining in the modified version
  for ( ; j < m; ++j) {

    // form the full path
    path_modified.replace(basesize_modified, std::string::npos, namelist_modified[j]->d_name);
    std::string path_modified_full = modified_clone_path.native() + PATH_SEPARATOR + path_modified;

    // skip directories
    if(is_dir(namelist_modified[j], path_modified_full.c_str()) != 0) {
      continue;
    }

    // skip over output file
    if (is_output_file(path_modified_full.c_str(), outstat) != 0) {
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
   file(std::optional<std::string>(), path_modified);

  }

  // process all directories
  i = 0;
  j = 0;
  while (i < n && j < m) {

    // form the full path
    path_original.replace(basesize_original, std::string::npos, namelist_original[i]->d_name);
    path_modified.replace(basesize_modified, std::string::npos, namelist_modified[j]->d_name);

    std::string path_original_full = original_clone_path.native() + PATH_SEPARATOR + path_original;
    std::string path_modified_full = modified_clone_path.native() + PATH_SEPARATOR + path_modified;

    // skip non-directories
    if(is_dir(namelist_original[i], path_original_full.c_str()) != 1) {
      ++i;
      continue;
    }
    if(is_dir(namelist_modified[j], path_modified_full.c_str()) != 1) {
      ++j;
      continue;
    }

    // skip over output file
    if (is_output_file(path_original_full.c_str(), outstat) == 1) {
      ++i;
      continue;
    }

    if (is_output_file(path_modified_full.c_str(), outstat) == 1) {
      ++j;
      continue;
    }

    // is this a common, inserted, or deleted directory?
    int comparison = strcoll(namelist_original[i]->d_name, namelist_modified[j]->d_name);

    std::optional<std::string> directory_path_one;
    std::optional<std::string> directory_path_two;
    if(comparison <= 0) ++i, directory_path_one = path_original;
    if(comparison >= 0) ++j, directory_path_two = path_modified;

    // process these directories
    directory(directory_path_one, directory_path_two);

  }

  // process all directories that remain in the original version
  for ( ; i < n; ++i) {

    // form the full path
    path_original.replace(basesize_original, std::string::npos, namelist_original[i]->d_name);
    std::string path_original_full = original_clone_path.native() + PATH_SEPARATOR + path_original;

    // skip non-directories
    if(is_dir(namelist_original[i], path_original_full.c_str()) != 1) {
      continue;
    }

    // skip over output file
    if (is_output_file(path_original_full.c_str(), outstat) == 1) {
      continue;
    }

    // process this directory
    directory(path_original, std::optional<std::string>());

  }

  // process all directories that remain in the modified version
  for ( ; j < m; ++j) {

    // form the full path
    path_modified.replace(basesize_modified, std::string::npos, namelist_modified[j]->d_name);
    std::string path_modified_full = modified_clone_path.native() + PATH_SEPARATOR + path_modified;

    // skip non-directories
    if(is_dir(namelist_modified[j], path_modified_full.c_str()) != 1) {
      continue;
    }

    // skip over output file
    if (is_output_file(path_modified_full.c_str(), outstat) == 1) {
      continue;
    }

    // process this directory
    directory(std::optional<std::string>(), path_modified);

  }

  // all done with this directory
  if (n > 0) {
    for (int i = 0; i < n; ++i)
      free(namelist_original[i]);
    free(namelist_original);
  }

  if (m > 0) {
    for (int j = 0; j < m; ++j)
      free(namelist_modified[j]);
    free(namelist_modified);
  }

#endif
  
}

void srcdiff_input_source_git::process_files_from() {

  try {

    // translate all the filenames listed in the named file
    // Use libxml2 routines so that we can handle http:, file:, and gzipped files automagically
    std::ifstream input(options.files_from_name->c_str());
    std::string line;
    while(getline(input, line, '\n'), input) {

      // skip over whitespace
      // TODO:  Other types of whitespace?  backspace?
      int white_length = strspn(line.c_str(), " \t\f");

      line.erase(0, white_length);

      // skip blank lines or comment lines
      if (line[0] == '\0' || line[0] == '#') {
        continue;
      }

      // remove any end whitespace
      // TODO:  Extract function, and use elsewhere
      for (int i = line.size() - 1; i != 0; --i) {
        if (isspace(line[i]))
          line[i] = 0;
        else
          break;

      }

      std::string::size_type sep_pos = line.find('|');
      std::string path_original = line.substr(0, sep_pos);
      std::string path_modified = line.substr(sep_pos + 1);

      std::optional<std::string> path = path_original;
      std::string path_full = original_clone_path.native() + PATH_SEPARATOR + *path;
      if(*path == "") {

         path = path_modified;
         path_full = modified_clone_path.native() + PATH_SEPARATOR + *path;

       }

      if(!is_dir(0, path_full.c_str())) file(path_original, path_modified);

    }

  } catch (uri_stream_error) {

    fprintf(stderr, "%s error: file/URI \'%s\' does not exist.\n", "srcdiff", options.files_from_name->c_str());
    exit(EXIT_FAILURE);

  }

}
