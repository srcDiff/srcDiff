/*
  srcdiff.cpp

  Copyright (C) 2011-2014  SDML (www.sdml.info)

  This file is part of the srcDiff translator.

  The srcDiff translator is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcDiff translator is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcDiff translator; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Main program to run the srcDiff translator.
*/

#include <srcdiff_input_filename.hpp>
#include <srcdiff_translator.hpp>
#include <srcdiff_options.hpp>
#include <methods.hpp>

#include <LineDiffRange.hpp>

#include <srcml.h>

#include <cstdlib>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <URIStream.hpp>

#ifdef SVN
#include <svn_io.hpp>
#endif

void srcdiff_libxml_error(void *ctx, const char *msg, ...) {}

#ifdef __GNUG__
extern "C" void terminate_handler(int);
#endif

void srcdiff_dir_top(srcdiff_translator & translator, srcdiff_options & options, const char * directory_old, const char * directory_new);
void srcdiff_dir(srcdiff_translator & translator, srcdiff_options & options, const char * directory_old, int directory_length_old, const char * directory_new, int directory_length_new, const struct stat& outstat);
void srcdiff_files_from(srcdiff_translator & translator, srcdiff_options & options);

// translate a file, maybe an archive
void srcdiff_file(srcdiff_translator & translator, srcdiff_options & options, const char * path_one, const char * path_two);

void srcdiff_text(srcdiff_translator & translator, srcdiff_options & options, const char * path_one, const char * path_two, int directory_length_old, int directory_length_new);

int main(int argc, char* argv[]) {

  int exit_status = EXIT_SUCCESS;

  LIBXML_TEST_VERSION

  xmlGenericErrorFunc handler = (xmlGenericErrorFunc) srcdiff_libxml_error;
  initGenericErrorDefaultFunc(&handler);

  // process command-line arguments
  srcdiff_options options = process_command_line(argc, argv);

  try {

    // translator from input to output using determined language
    srcdiff_translator translator(options.srcdiff_filename->c_str(),
                                 options.methods,
                                 options.css_url ? *options.css_url : std::string(),
                                 options.archive,
                                 options.flags,
                                 options.number_context_lines);

#ifdef __GNUG__
    // setup so we can gracefully stop after a file at a time
    signal(SIGINT, terminate_handler);
#endif

    if(options.files_from_name) {

      srcdiff_files_from(translator, options);

    } else {

     for(std::pair<std::string, std::string> input_pair : options.input_pairs)
        srcdiff_file(translator, options, input_pair.first.c_str(), input_pair.second.c_str());

    }

  } catch (const std::exception & e) {

    exit_status = EXIT_FAILURE;

  }

  srcml_free_archive(options.archive);
  
  return exit_status;

}

#ifdef __GNUG__
extern "C" void terminate_handler(int) {

  // turn off handler for this signal
  signal(SIGINT, SIG_DFL);

}
#endif

void srcdiff_file(srcdiff_translator & translator, srcdiff_options & options, const char * path_one, const char * path_two) {

  // handle local directories specially
  struct stat instat = { 0 };
  int stat_status = stat(path_one, &instat);
  if (!stat_status && S_ISDIR(instat.st_mode)) {
    srcdiff_dir_top(translator, options, path_one, path_two);
    return;
  }

  srcdiff_text(translator, options, path_one, path_two, 0, 0);

}

void srcdiff_text(srcdiff_translator & translator, srcdiff_options & options, const char * path_one, const char * path_two, int directory_length_old, int directory_length_new) {

  std::string filename = path_one[0] ? path_one + directory_length_old : path_one;
  if(path_two[0] == 0 || strcmp(path_one + directory_length_old, path_two + directory_length_new) != 0) {

    filename += "|";
    filename += path_two[0] ? path_two + directory_length_new : path_two;

  }

  srcdiff_input_filename input_old(options.archive, path_one, options.flags);
  srcdiff_input_filename input_new(options.archive, path_two, options.flags);
  LineDiffRange line_diff_range(path_one, path_two, options.svn_url ? options.svn_url->c_str() : 0, options.flags);

  const char * path = path_one;
  if(path_one == 0 || path_one[0] == 0 || path_one[0] == '@')
    path = path_two;

  const char * language_string = srcml_archive_check_extension(options.archive, path);

  translator.translate(input_old, input_new, line_diff_range, language_string, srcml_archive_get_directory(options.archive), filename.c_str(), 0);

}


void srcdiff_dir_top(srcdiff_translator & translator, srcdiff_options & options, const char * directory_old, const char * directory_new) {

  // by default, all dirs are treated as an archive
  srcml_archive_enable_option(options.archive, SRCML_OPTION_ARCHIVE);

  // record the stat info on the output file

  struct stat outstat = { 0 };
  stat(options.srcdiff_filename->c_str(), &outstat);

  int directory_length_old = strlen(directory_old);

  char * dold = (char *)directory_old;

  char * dnew = (char *)directory_new;

  if(dold[directory_length_old - 1] != '/') {

    ++directory_length_old;

  } else {

    dold[directory_length_old - 1] = '\0';

  }

  int directory_length_new = strlen(directory_new);

  if(dnew[directory_length_new - 1] != '/') {

    ++directory_length_new;

  } else {

    dnew[directory_length_new - 1] = '\0';

  }

  std::string directory = dold;

  if(strcmp(dold, dnew) != 0) {

    directory += "|";
    directory += dnew;

  }

 if(!srcml_archive_get_directory(options.archive)) srcml_archive_set_directory(options.archive, directory.c_str());

  srcdiff_dir(translator, options, dold, directory_length_old, dnew, directory_length_new, outstat);

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

void srcdiff_dir(srcdiff_translator & translator, srcdiff_options & options,
  const char * directory_old, int directory_length_old, const char * directory_new, int directory_length_new, const struct stat& outstat) {

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#if defined(__GNUC__) && !defined(__MINGW32__)

  // collect the filenames in alphabetical order
  struct dirent ** namelist_old;
  struct dirent ** namelist_new;

  int n = scandir(directory_old, &namelist_old, dir_filter, alphasort);
  int m = scandir(directory_new, &namelist_new, dir_filter, alphasort);
  // TODO:  Fix error handling.  What if one is in error?
  if (n < 0 && m < 0) {
    return;
  }

  // start of path from directory name
  std::string filename_old = directory_old;
  if (filename_old != "" && !filename_old.empty() && filename_old[filename_old.size() - 1] != PATH_SEPARATOR)
    filename_old += PATH_SEPARATOR;
  int basesize_old = filename_old.length();

  std::string filename_new = directory_new;
  if (filename_new != "" && !filename_new.empty() && filename_new[filename_new.size() - 1] != PATH_SEPARATOR)
    filename_new += PATH_SEPARATOR;
  int basesize_new = filename_new.length();

  // process all non-directory files
  int i = 0;
  int j = 0;
  while (i < n && j < m) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, namelist_old[i]->d_name);
    filename_new.replace(basesize_new, std::string::npos, namelist_new[j]->d_name);

    // skip directories
    if(is_dir(namelist_old[i], filename_old.c_str()) != 0) {
      ++i;
      continue;
    }
    if(is_dir(namelist_new[j], filename_new.c_str()) != 0) {
      ++j;
      continue;
    }

    // skip over output file
    if (is_output_file(filename_old.c_str(), outstat) == 1) {
      ++i;
      continue;
    }
    if (is_output_file(filename_new.c_str(), outstat) == 1) {
      ++j;
      continue;
    }

    // is this a common, inserted, or deleted file?
    int comparison = strcoll(namelist_old[i]->d_name, namelist_new[j]->d_name);

    // translate the file listed in the input file using the directory and filename extracted from the path
    srcdiff_text(translator,
                 options,
                 comparison <= 0 ? (++i, filename_old.c_str()) : "",
                 comparison >= 0 ? (++j, filename_new.c_str()) : "",
                 directory_length_old,
                 directory_length_new);
  }

  // process all non-directory files that are remaining in the old version
  for ( ; i < n; ++i) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, namelist_old[i]->d_name);

    // skip directories
    if(is_dir(namelist_old[i], filename_old.c_str()) != 0)
      continue;

    // skip over output file
    if (is_output_file(filename_old.c_str(), outstat) != 0) {
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
    srcdiff_text(translator, options, filename_old.c_str(), "", directory_length_old, directory_length_new);

  }

  // process all non-directory files that are remaining in the new version
  for ( ; j < m; ++j) {

    // form the full path
    filename_new.replace(basesize_new, std::string::npos, namelist_new[j]->d_name);

    // skip directories
    if(is_dir(namelist_new[j], filename_new.c_str()) != 0)
      continue;

    // skip over output file
    if (is_output_file(filename_new.c_str(), outstat) != 0) {
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
    srcdiff_text(translator, options, "", filename_new.c_str(), directory_length_old, directory_length_new);

  }

  // process all directories
  i = 0;
  j = 0;
  while (i < n && j < m) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, namelist_old[i]->d_name);
    filename_new.replace(basesize_new, std::string::npos, namelist_new[j]->d_name);

    // skip non-directories
    if(is_dir(namelist_old[i], filename_old.c_str()) != 1) {
      ++i;
      continue;
    }
    if(is_dir(namelist_new[j], filename_new.c_str()) != 1) {
      ++j;
      continue;
    }

    // is this a common, inserted, or deleted directory?
    int comparison = strcoll(namelist_old[i]->d_name, namelist_new[j]->d_name);

    // process these directories
    srcdiff_dir(translator,
                options,
                comparison <= 0 ? (++i, filename_old.c_str()) : "",
                directory_length_old,
                comparison >= 0 ? (++j, filename_new.c_str()) : "",
                directory_length_new,
                outstat);
  }

  // process all directories that remain in the old version
  for ( ; i < n; ++i) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, namelist_old[i]->d_name);

    // skip non-directories
    if(is_dir(namelist_old[i], filename_old.c_str()) != 1)
      continue;

    // skip over output file
    if (is_output_file(filename_old.c_str(), outstat) == 1) {
      continue;
    }

    // process this directory
    srcdiff_dir(translator,
                options,
                filename_old.c_str(),
                directory_length_old,
                "",
                directory_length_new,
                outstat);
  }

  // process all directories that remain in the new version
  for ( ; j < m; ++j) {

    // form the full path
    filename_new.replace(basesize_new, std::string::npos, namelist_new[j]->d_name);

    // skip non-directories
    if(is_dir(namelist_new[j], filename_new.c_str()) != 1)
      continue;

    // skip over output file
    if (is_output_file(filename_new.c_str(), outstat) == 1) {
      continue;
    }

    // process this directory
    srcdiff_dir(translator,
                options,
                "",
                directory_length_old,
                filename_new.c_str(),
                directory_length_new,
                outstat);
  }

  // all done with this directory
  if (n > 0) {
    for (int i = 0; i < n; ++i)
      free(namelist_old[i]);
    free(namelist_old);
  }

  if (m > 0) {
    for (int j = 0; j < m; ++j)
      free(namelist_new[j]);
    free(namelist_new);
  }

#else

#endif

#undef PATH_SEPARATOR

}


void srcdiff_files_from(srcdiff_translator & translator, srcdiff_options & options) {

#define FILELIST_COMMENT '#'

  try {

    // translate all the filenames listed in the named file
    URIStream uriinput(options.files_from_name->c_str());
    char * file_one;

    while ((file_one = uriinput.readline())) {

      // skip over whitespace
      // TODO:  Other types of whitespace?  backspace?
      file_one += strspn(file_one, " \t\f");

      // skip blank lines or comment lines
      if (file_one[0] == '\0' || file_one[0] == FILELIST_COMMENT)
        continue;

      char * separator = strchr(file_one, '|');

      // remove any end whitespace
      // TODO:  Extract function, and use elsewhere
      for (char * p = separator- 1; p != file_one; --p) {
        if (isspace(*p))
          *p = 0;
        else
          break;
      }

      *separator = 0;

      char * file_two = separator + 1;

      file_two += strspn(file_two, " \t\f");

      // remove any end whitespace
      // TODO:  Extract function, and use elsewhere
      for (char * p = file_two + strlen(file_two) - 1; p != file_two; --p) {
        if (isspace(*p))
          *p = 0;
        else
          break;
      }

      std::string filename = file_one;
      filename += "|";
      filename += file_two;

      srcdiff_file(translator, options, file_one, file_two);

      *separator = '|';

    }

  } catch (URIStreamFileError) {

    fprintf(stderr, "error: file/URI \'%s\' does not exist.\n", options.files_from_name->c_str());
    exit(EXIT_FAILURE);

  }

#undef FILELIST_COMMENT

}
