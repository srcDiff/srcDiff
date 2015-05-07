#include <srcdiff_input_source_local.hpp>

#include <srcml.h>

#include <srcdiff_input.hpp>

#include <uri_stream.hpp>

#include <cstring>
#include <dirent.h>

srcdiff_input_source_local::srcdiff_input_source_local(const srcdiff_options & options) : srcdiff_input_source(options) {

  translator = new srcdiff_translator(options.srcdiff_filename, options.flags, options.methods, options.archive,
                                      options.bash_view_context, options.summary_type_str);

  outstat = { 0 };
  stat(options.srcdiff_filename.c_str(), &outstat);

}

srcdiff_input_source_local::~srcdiff_input_source_local() {

  if(translator) delete translator;

}

void srcdiff_input_source_local::consume() {

  if(options.files_from_name) {

    files_from();

  } else {

   for(std::pair<std::string, std::string> input_pair : options.input_pairs) {

      struct stat in_stat_original = { 0 };
      int stat_status_original = stat(input_pair.first.c_str(), &in_stat_original);

      struct stat in_stat_modified = { 0 };
      int stat_status_modified = stat(input_pair.second.c_str(), &in_stat_modified);

      if(stat_status_original == -1 && stat_status_modified == -1)
        throw std::string("Input sources '" + input_pair.first + "' and '" + input_pair.second + "' could not be opened");
      else if(stat_status_original == -1)
        throw std::string("Input source '" + input_pair.first + "' could not be opened");
      else if(stat_status_modified == -1)
        throw std::string("Input source '" + input_pair.second + "' could not be opened");

      if (!stat_status_original && S_ISDIR(in_stat_original.st_mode)) {

        srcml_archive_enable_option(options.archive, SRCML_OPTION_ARCHIVE);

        if(!srcml_archive_get_url(options.archive)) {

          std::string directory_path = input_pair.first == input_pair.second ? input_pair.first : input_pair.first + '|' + input_pair.second;
          srcml_archive_set_url(options.archive, directory_path.c_str());

        }

        directory_length_original = input_pair.first.back() == '/' ? input_pair.first.size() : input_pair.first.size() + 1;
        directory_length_modified = input_pair.second.back() == '/' ? input_pair.second.size() : input_pair.second.size() + 1;

        directory(input_pair.first, nullptr, input_pair.second, nullptr);

      } else {

        file(input_pair.first, nullptr, input_pair.second, nullptr);

      }

   }

  }

}

const char * srcdiff_input_source_local::get_language(const boost::optional<std::string> & path_original, const boost::optional<std::string> & path_modified) {

  boost::optional<std::string> path = path_original;
  if(!path || path->empty()) path = path_modified;
  if(!path) path = std::string();

  return srcml_archive_check_extension(options.archive, path->c_str());

}

void srcdiff_input_source_local::process_file(const boost::optional<std::string> & path_original, const void * context_original,
                                              const boost::optional<std::string> & path_modified, const void * context_modified) {

  const char * language_string = get_language(path_original, path_modified);

  if(language_string == SRCML_LANGUAGE_NONE) return;

  std::string path_one = path_original ? *path_original : std::string();
  std::string path_two = path_modified ? *path_modified : std::string();

  std::string unit_filename = !path_one.empty() ? path_one.substr(directory_length_original) : path_one;
  std::string filename_two =  !path_two.empty() ? path_two.substr(directory_length_modified) : path_two;
  if(path_two.empty() || unit_filename != filename_two) {

    unit_filename += "|";
    unit_filename += filename_two;

  }

  srcdiff_input<srcdiff_input_source_local> input_original(options.archive, path_original, language_string, options.flags, *this);
  srcdiff_input<srcdiff_input_source_local> input_modified(options.archive, path_modified, language_string, options.flags, *this);
  line_diff_range<srcdiff_input_source_local> line_diff_range(path_one, path_two, this);

  const char * dir = srcml_archive_get_url(options.archive);

  translator->translate(input_original, input_modified, line_diff_range, language_string, dir ? boost::optional<std::string>(dir) : boost::optional<std::string>(), unit_filename, 0);

}

// file/directory names to ignore when processing a directory
// const/non-const versions for linux/bsd different declarations
int dir_filter(const struct dirent* d) {

    return d->d_name[0] != '.';
}

int dir_filter(struct dirent* d) {

  return dir_filter((const struct dirent*)d);
}

int is_dir(struct dirent * file, const char * filename) {

#ifdef _DIRENT_HAVE_D_TYPE
  if (file->d_type == DT_DIR)
    return 1;
#endif

  // path with current filename
  // handle directories later after all the filenames
  struct stat instat = { 0 };

  int stat_status = stat(filename, &instat);

  if(stat_status)
    return stat_status;

#ifndef _DIRENT_HAVE_D_TYPE
  if(S_ISDIR(instat.st_mode))
    return 1;
#endif

  return 0;

}

int is_output_file(const char * filename, const struct stat & outstat) {

  struct stat instat = { 0 };

  int stat_status = stat(filename, &instat);

  if(stat_status)
    return stat_status;

  if(instat.st_ino == outstat.st_ino && instat.st_dev == outstat.st_dev)
    return 1;

  return 0;

}

void srcdiff_input_source_local::process_directory(const boost::optional<std::string> & directory_original, const void * context_original,
                                                   const boost::optional<std::string> & directory_modified, const void * context_modified) {

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

  // collect the filenames in alphabetical order
  struct dirent ** namelist_original;
  struct dirent ** namelist_modified;

  int n = scandir(directory_original ? directory_original->c_str() : "", &namelist_original, dir_filter, alphasort);
  int m = scandir(directory_modified ? directory_modified->c_str() : "", &namelist_modified, dir_filter, alphasort);

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

    // skip directories
    if(is_dir(namelist_original[i], path_original.c_str()) != 0) {
      ++i;
      continue;
    }

    if(is_dir(namelist_modified[j], path_modified.c_str()) != 0) {
      ++j;
      continue;
    }

    // skip over output file
    if (is_output_file(path_original.c_str(), outstat) == 1) {
      ++i;
      continue;
    }

    if (is_output_file(path_modified.c_str(), outstat) == 1) {
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

    // skip directories
    if(is_dir(namelist_original[i], path_original.c_str()) != 0)
      continue;

    // skip over output file
    if (is_output_file(path_original.c_str(), outstat) != 0) {
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(path_original, nullptr, boost::optional<std::string>(), nullptr);

  }

  // process all non-directory files that are remaining in the modified version
  for ( ; j < m; ++j) {

    // form the full path
    path_modified.replace(basesize_modified, std::string::npos, namelist_modified[j]->d_name);

    // skip directories
    if(is_dir(namelist_modified[j], path_modified.c_str()) != 0)
      continue;

    // skip over output file
    if (is_output_file(path_modified.c_str(), outstat) != 0) {
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

    // skip non-directories
    if(is_dir(namelist_original[i], path_original.c_str()) != 1) {
      ++i;
      continue;
    }
    if(is_dir(namelist_modified[j], path_modified.c_str()) != 1) {
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

    // skip non-directories
    if(is_dir(namelist_original[i], path_original.c_str()) != 1)
      continue;

    // skip over output file
    if (is_output_file(path_original.c_str(), outstat) == 1) {
      continue;
    }

    // process this directory
    directory(path_original, nullptr, boost::optional<std::string>(), nullptr);

  }

  // process all directories that remain in the modified version
  for ( ; j < m; ++j) {

    // form the full path
    path_modified.replace(basesize_modified, std::string::npos, namelist_modified[j]->d_name);

    // skip non-directories
    if(is_dir(namelist_modified[j], path_modified.c_str()) != 1)
      continue;

    // skip over output file
    if (is_output_file(path_modified.c_str(), outstat) == 1) {
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

}


void srcdiff_input_source_local::process_files_from() {

#define FILELIST_COMMENT '#'

  try {

    // translate all the filenames listed in the named file

    input_context * context = open(options.files_from_name->c_str());
    uri_stream<srcdiff_input_source_local> uriinput(context);

    const char * c_line = 0;
    while ((c_line = uriinput.readline())) {

      std::string line = c_line;
 
      int white_length = strspn(line.c_str(), " \t\f");
      line.erase(0, white_length);

      // skip blank lines or comment lines
      if (line.empty() || line[0] == FILELIST_COMMENT) continue;

      std::string path_original = line.substr(0, line.find('|'));
      std::string path_modified = line.substr(line.find('|') + 1);

      file(path_original, nullptr, path_modified, nullptr);

    }

  } catch (uri_stream_error) {

    fprintf(stderr, "error: file/URI \'%s\' does not exist.\n", options.files_from_name->c_str());
    exit(EXIT_FAILURE);

  }

#undef FILELIST_COMMENT

}

srcdiff_input_source_local::input_context * srcdiff_input_source_local::open(const char * uri) const {

  input_context * context = new input_context;

  context->in.open(uri);

  return context->in ? context : (delete context, nullptr);

}

int srcdiff_input_source_local::read(void * context, char * buffer, size_t len) {

  input_context * ctx = (input_context *)context;

  ctx->in.read(buffer, len);

  return ctx->in.gcount();
}

int srcdiff_input_source_local::close(void * context) {

  input_context * ctx = (input_context *)context;

  ctx->in.close();

  delete ctx;

  return 1;
}
