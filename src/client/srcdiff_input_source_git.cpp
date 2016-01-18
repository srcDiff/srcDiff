#include <srcdiff_input_source_git.hpp>

#include <srcdiff_input.hpp>

#include <mutex>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <cstdio>

#include <uri_stream.hpp>

static std::mutex mutex;
  
srcdiff_input_source_git::srcdiff_input_source_git(const srcdiff_options & options)
  : srcdiff_input_source(options),
    original_clone_path(boost::filesystem::temp_directory_path().native() + boost::filesystem::unique_path().native()),
    modified_clone_path(boost::filesystem::temp_directory_path().native() + boost::filesystem::unique_path().native()),
    clean_path(true) {

  std::string quiet_flag;
  if(is_option(options.flags, OPTION_QUIET)) quiet_flag = "--quiet ";

  std::string clone_original_command("git clone " + quiet_flag + *options.git_url + " "
                                    + original_clone_path.native());
  FILE * process = popen(clone_original_command.c_str(), "r");
  int error = pclose(process);
  if(error) throw std::string("Unable to clone " + original_clone_path.native());

  std::string clone_modified_command("git clone " + quiet_flag + *options.git_url + " "
                                    + modified_clone_path.native());
  FILE * process = popen(clone_modified_command.c_str(), "r");
  int error = pclose(process);
  if(error) throw std::string("Unable to clone " + modified_clone_path.native());

  std::string checkout_original_command("git checkout " + options.git_revision_one);
  FILE * process = popen(checkout_original_command.c_str(), "r");
  int error = pclose(process);
  if(error) throw std::string("Unable to checkout " + options.git_revision_one);

  std::string checkout_modified_command("git checkout " + options.git_revision_two);
  FILE * process = popen(checkout_modified_command.c_str(), "r");
  int error = pclose(process);
  if(error) throw std::string("Unable to checkout " + options.git_revision_two);

  translator = new srcdiff_translator(options.srcdiff_filename, options.flags, options.methods, options.archive,
                                      options.unit_filename,
                                      options.unified_view_context, options.side_by_side_tab_size,
                                      options.summary_type_str);
}

srcdiff_input_source_git::~srcdiff_input_source_git() {

  if(translator) delete translator;

  if(clean_path) {

    std::string remove_original_command("rm -rf ");
    remove_original_command += original_clone_path.native();

    FILE * process = popen(remove_original_command.c_str(), "r");
    pclose(process);

    std::string remove_modified_command("rm -rf ");
    remove_modified_command += modified_clone_path.native();

    FILE * process = popen(remove_modified_command.c_str(), "r");
    pclose(process);

  }

}

void srcdiff_input_source_git::consume() {

  directory(std::string(), nullptr, std::string(), nullptr);

}

const char * srcdiff_input_source_git::get_language(const boost::optional<std::string> & path_original, const boost::optional<std::string> & path_modified) {

  boost::optional<std::string> path = path_original;
  if(!path || path->empty()) path = path_modified;

  return srcml_archive_check_extension(options.archive, path->c_str());

}

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

void srcdiff_input_source_git::process_file(const boost::optional<std::string> & path_original, const void * context_original,
                                            const boost::optional<std::string> & path_modified, const void * context_modified) {

  const char * language_string = get_language(path_original, path_modified);

  if(language_string == SRCML_LANGUAGE_NONE) return;

  std::string path_one = path_original ? original_clone_path.native() + PATH_SEPARATOR + *path_original : std::string();
  std::string path_two = path_modified ? modified_clone_path.native() + PATH_SEPARATOR + *path_modified : std::string();

  std::string unit_filename = !path_one.empty() ? path_one.substr(directory_length_original) : path_one;
  std::string filename_two =  !path_two.empty() ? path_two.substr(directory_length_modified) : path_two;
  if(path_two.empty() || unit_filename != filename_two) {

    unit_filename += "|";
    unit_filename += filename_two;

  }

  srcdiff_input<srcdiff_input_source_local> input_original(options.archive, path_original, language_string, options.flags, *this);
  srcdiff_input<srcdiff_input_source_local> input_modified(options.archive, path_modified, language_string, options.flags, *this);
  line_diff_range<srcdiff_input_source_local> line_diff_range(path_one, path_two, this);

  translator->translate(input_original, input_modified, line_diff_range, language_string,
                        unit_filename, boost::optional<std::string>());

}

bool operator<(const std::pair<std::string, size_t> & pair_one, const std::pair<std::string, size_t> & pair_two) {

  return pair_one.first < pair_two.first;

}

void srcdiff_input_source_git::process_directory(const boost::optional<std::string> & directory_original, const void * context_original,
                                                 const boost::optional<std::string> & directory_modified, const void * context_modified) {

#ifndef _MSC_BUILD

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

  // collect the filenames in alphabetical order
  struct dirent ** namelist_original;
  struct dirent ** namelist_modified;

  std::string directory_original_full
    = directory_original ? 
      original_clone_path.native() + PATH_SEPARATOR + directory_original
      : std::string();

  std::string directory_modified_full
    = directory_modified ? 
      modified_clone_path.native() + PATH_SEPARATOR + directory_modified
      : std::string();

  int n = scandir(directory_original_full, &namelist_original, dir_filter, alphasort);
  int m = scandir(directory_modified_full, &namelist_modified, dir_filter, alphasort);

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

    std::string path_original_full = original_clone_path + PATH_SEPARATOR + path_original;
    std::string path_modified_full = modified_clone_path + PATH_SEPARATOR + path_modified;

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


    boost::optional<std::string> file_path_original;
    boost::optional<std::string> file_path_modified;
    if(comparison <= 0) ++i, file_path_original = path_original;
    if(comparison >= 0) ++j, file_path_modified = path_modified;

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(file_path_original, nullptr, file_path_modified, nullptr);

  }

  // process all non-directory files that are remaining in the original version
  for ( ; i < n; ++i) {

    // form the full path
    path_original.replace(basesize_original, std::string::npos, namelist_original[i]->d_name);
    std::string path_original_full = original_clone_path + PATH_SEPARATOR + path_original;

    // skip directories
    if(is_dir(namelist_original[i], path_original_full.c_str()) != 0)
      continue;

    // skip over output file
    if (is_output_file(path_original_full.c_str(), outstat) != 0) {
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(path_original, nullptr, boost::optional<std::string>(), nullptr);

  }

  // process all non-directory files that are remaining in the modified version
  for ( ; j < m; ++j) {

    // form the full path
    path_modified.replace(basesize_modified, std::string::npos, namelist_modified[j]->d_name);
    std::string path_modified_full = modified_clone_path + PATH_SEPARATOR + path_modified;

    // skip directories
    if(is_dir(namelist_modified[j], path_modified_full.c_str()) != 0)
      continue;

    // skip over output file
    if (is_output_file(path_modified_full.c_str(), outstat) != 0) {
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
   file(boost::optional<std::string>(), nullptr, path_modified, nullptr);

  }

  // process all directories
  i = 0;
  j = 0;
  while (i < n && j < m) {

    // form the full path
    path_original.replace(basesize_original, std::string::npos, namelist_original[i]->d_name);
    path_modified.replace(basesize_modified, std::string::npos, namelist_modified[j]->d_name);

    std::string path_original_full = original_clone_path + PATH_SEPARATOR + path_original;
    std::string path_modified_full = modified_clone_path + PATH_SEPARATOR + path_modified;

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

    boost::optional<std::string> directory_path_one;
    boost::optional<std::string> directory_path_two;
    if(comparison <= 0) ++i, directory_path_one = path_original;
    if(comparison >= 0) ++j, directory_path_two = path_modified;

    // process these directories
    directory(directory_path_one, nullptr, directory_path_two, nullptr);

  }

  // process all directories that remain in the original version
  for ( ; i < n; ++i) {

    // form the full path
    path_original.replace(basesize_original, std::string::npos, namelist_original[i]->d_name);
    std::string path_original_full = original_clone_path + PATH_SEPARATOR + path_original;

    // skip non-directories
    if(is_dir(namelist_original[i], path_original_full.c_str()) != 1)
      continue;

    // skip over output file
    if (is_output_file(path_original_full.c_str(), outstat) == 1) {
      continue;
    }

    // process this directory
    directory(path_original, nullptr, boost::optional<std::string>(), nullptr);

  }

  // process all directories that remain in the modified version
  for ( ; j < m; ++j) {

    // form the full path
    path_modified.replace(basesize_modified, std::string::npos, namelist_modified[j]->d_name);
    std::string path_modified_full = modified_clone_path + PATH_SEPARATOR + path_modified;

    // skip non-directories
    if(is_dir(namelist_modified[j], path_modified_full.c_str()) != 1)
      continue;

    // skip over output file
    if (is_output_file(path_modified_full.c_str(), outstat) == 1) {
      continue;
    }

    // process this directory
    directory(boost::optional<std::string>(), nullptr, path_modified, nullptr);

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

#undef PATH_SEPARATOR

#endif
  
}

void srcdiff_input_source_git::process_files_from() {}
