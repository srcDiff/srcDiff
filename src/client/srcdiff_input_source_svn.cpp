// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_input_source_svn.cpp
 *
 * @copyright Copyright (C) 2013-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_input_source_svn.hpp>

#include <srcdiff_input.hpp>

#include <mutex>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>

#include <cstring>
  
#include <uri_stream.hpp>

#include <svn_version.h>

std::mutex mutex;

int abortfunc(int retcode) {

  std::cout << retcode << '\n';

  return retcode;
}

srcdiff_input_source_svn::srcdiff_input_source_svn(const srcdiff_options & options) : srcdiff_input_source(options) {


  apr_initialize();

  apr_allocator_t * allocator;
  apr_allocator_create(&allocator);

  apr_pool_create_ex(&pool, NULL, abortfunc, allocator);

  svn_client_ctx_t * ctx;
  apr_hash_t * cfg_hash;
  svn_config_t * cfg_config;

  svn_ra_initialize(pool);
  svn_config_get_config(&cfg_hash, NULL, pool);
  
#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 8) || SVN_VER_MAJOR > 1
  svn_client_create_context2(&ctx, 0, pool);
#else
  svn_client_create_context(&ctx, pool);
#endif

  ctx->config = cfg_hash;
  cfg_config = (svn_config_t *)apr_hash_get(ctx->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING);

  svn_boolean_t non_interactive = false;
  const char * auth_username = "";
  const char * auth_password = "";
  const char * config_dir = 0;

  svn_boolean_t no_auth_cache = false;
  svn_boolean_t trust_server_cert = true;

  svn_auth_baton_t * ab;
  svn_cmdline_create_auth_baton(&ab, non_interactive, auth_username, auth_password, config_dir, no_auth_cache, trust_server_cert, cfg_config, ctx->cancel_func, ctx->cancel_baton, pool);

  ctx->auth_baton = ab;
  ctx->conflict_func = NULL;
  ctx->conflict_baton = NULL;

#if (SVN_VER_MAJOR == 1 && SVN_VER_MINOR >= 8) || SVN_VER_MAJOR > 1
  svn_error_t * svn_error = svn_client_open_ra_session2(&session, options.svn_url ? options.svn_url->c_str() : 0, 0, ctx, pool, pool);
#else
  svn_error_t * svn_error = svn_client_open_ra_session(&session, options.svn_url ? options.svn_url->c_str() : 0, ctx, pool);
#endif

  if(svn_error) {

    apr_pool_destroy(pool);
    apr_terminate();

    fprintf(stderr, "%s\n", svn_error->message);
    exit(1);

  }

}

srcdiff_input_source_svn::~srcdiff_input_source_svn() {

  apr_pool_destroy(pool);

  apr_terminate();

}

void srcdiff_input_source_svn::consume() {

  if(options.files_from_name)                              files_from();
  else if(is_option(options.flags, OPTION_SVN_CONTINUOUS)) session_range();
  else                                                     session_single();

}

const char * srcdiff_input_source_svn::get_language(const std::optional<std::string> & path_original, const std::optional<std::string> & path_modified) {

  const char * archive_language = srcml_archive_get_language(options.archive);
  if (archive_language) {
    return archive_language;
  } else {
    std::optional<std::string> path = path_original;
    if(!path || path->empty()) path = path_modified;
    if(!path || path->empty()) path = options.svn_url->c_str();

    return srcml_archive_check_extension(options.archive, path->c_str());
  }

}

void srcdiff_input_source_svn::session_single() {

  this->revision_one = options.revision_one;
  this->revision_two = options.revision_two;

  srcdiff_translator translator(options.srcdiff_filename, options.flags, options.methods, options.archive,
                                options.unit_filename,
                                options.view_options,
                                options.summary_type_str);

  this->translator = &translator;

  /** @todo two pools */

  std::optional<std::string> path = std::string();

  svn_dirent_t * dirent;
  svn_ra_stat(session, path->c_str(), revision_one, &dirent, pool);

  if(dirent->kind == svn_node_file)         file(path, path);
  else if(dirent->kind == svn_node_dir)     directory(path, path);
  else if(dirent->kind == svn_node_none)    fprintf(stderr, "%s\n", "Path does not exist");
  else if(dirent->kind == svn_node_unknown) fprintf(stderr, "%s\n", "Unknown");

}



void srcdiff_input_source_svn::session_range() {

  const svn_revnum_t & start_revision = options.revision_one;

  if(start_revision == SVN_INVALID_REVNUM) revision_one = 1;
  else revision_one = start_revision;

  revision_two = revision_one + 1;

  svn_revnum_t end_revision = options.revision_two;
  if(end_revision == SVN_INVALID_REVNUM) svn_ra_get_latest_revnum(session, &end_revision, pool);

  for(; revision_one < end_revision; ++revision_one, ++revision_two) {

    std::ostringstream full_srcdiff(options.srcdiff_filename != "-" ? options.srcdiff_filename : "", std::ios_base::ate);
    full_srcdiff << '_';
    full_srcdiff << revision_one;
    full_srcdiff << '-';
    full_srcdiff << revision_two;
    full_srcdiff << ".xml";

    srcdiff_translator translator(full_srcdiff.str(), options.flags, options.methods, options.archive,
                                  options.unit_filename,
                                  options.view_options,
                                  options.summary_type_str);

    this->translator = &translator;

    std::optional<std::string> path = std::string();
    svn_dirent_t * dirent;
    svn_ra_stat(session, path->c_str(), revision_one, &dirent, pool);

    if(dirent->kind == svn_node_file)         file(path, path);
    else if(dirent->kind == svn_node_dir)     directory(path, path);
    else if(dirent->kind == svn_node_none)    fprintf(stderr, "%s\n", "Path does not exist");
    else if(dirent->kind == svn_node_unknown) fprintf(stderr, "%s\n", "Unknown");

  }

}

void srcdiff_input_source_svn::process_file(const std::optional<std::string> & path_original,
                                            const std::optional<std::string> & path_modified) {

  const char * language_string = get_language(path_original, path_modified);
  if(language_string == SRCML_LANGUAGE_NONE) return;

  std::string path_original_temp = path_original ? *path_original : std::string();
  std::string path_modified_temp = path_modified ? *path_modified : std::string();

  std::string unit_filename = !path_original_temp.empty() ? path_original_temp.substr(directory_length_original) : std::string();
  std::string filename_two =  !path_modified_temp.empty() ? path_modified_temp.substr(directory_length_modified) : std::string();
  if(path_modified_temp.empty() || unit_filename != filename_two) {

    unit_filename += "|";
    unit_filename += filename_two;

  }

  // set path to include revision
  std::ostringstream svn_path_original(path_original_temp, std::ios_base::ate);
  svn_path_original << '@';
  svn_path_original << revision_one;

  std::ostringstream svn_path_modified(path_modified_temp, std::ios_base::ate);
  svn_path_modified << '@';
  svn_path_modified << revision_two;

  std::string svn_path_original_temp = svn_path_original.str();
  std::string svn_path_modified_temp = svn_path_modified.str();

  srcdiff_input<srcdiff_input_source_svn> input_original(options.archive, svn_path_original_temp, language_string, 0, *this);
  srcdiff_input<srcdiff_input_source_svn> input_modified(options.archive, svn_path_modified_temp, language_string, 0, *this);

  translator->translate(input_original, input_modified, language_string, unit_filename, unit_version);

}

void srcdiff_input_source_svn::process_directory(const std::optional<std::string> & directory_original,
                                                 const std::optional<std::string> & directory_modified) {

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

  apr_hash_t * dirents_one;
  svn_revnum_t fetched_rev_one;
  std::vector<std::string> dir_entries_one;

  if(directory_original) {

    apr_hash_index_t * item;
    const void * key;
    void * value;

    svn_ra_get_dir2(session, &dirents_one, &fetched_rev_one, NULL, directory_original->c_str(), revision_one, SVN_DIRENT_ALL, pool);

    for (item = apr_hash_first(pool, dirents_one); item; item = apr_hash_next(item)) {

      apr_hash_this(item, &key, NULL, &value);

      dir_entries_one.push_back((const char *)key);
    }

    sort(dir_entries_one.begin(), dir_entries_one.end());

  }

  apr_hash_t * dirents_two;
  svn_revnum_t fetched_rev_two;
  std::vector<std::string> dir_entries_two;

  if(directory_modified) {

    apr_hash_index_t * item;
    const void * key;
    void * value;

    svn_ra_get_dir2(session, &dirents_two, &fetched_rev_two, NULL, directory_modified->c_str(), revision_two, SVN_DIRENT_ALL, pool);

    for (item = apr_hash_first(pool, dirents_two); item; item = apr_hash_next(item)) {

      apr_hash_this(item, &key, NULL, &value);

      dir_entries_two.push_back((const char *)key);
    }

    sort(dir_entries_two.begin(), dir_entries_two.end());

  }

  std::string path_original;
  int basesize_original = 0;

  // process directory
  if(directory_original) {

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
  int n = dir_entries_one.size();
  int m = dir_entries_two.size();
  while (i < n && j < m) {

    // form the full path
    path_original.replace(basesize_original, std::string::npos, dir_entries_one[i]);
    path_modified.replace(basesize_modified, std::string::npos, dir_entries_two[j]);

    svn_dirent_t * dirent_original;
    svn_ra_stat(session, path_original.c_str(), revision_one, &dirent_original, pool);

    svn_dirent_t * dirent_modified;
    svn_ra_stat(session, path_modified.c_str(), revision_two, &dirent_modified, pool);


    // skip directories
    if(dirent_original->kind != svn_node_file) {
      ++i;
      continue;
    }

    if(dirent_modified->kind != svn_node_file) {
      ++j;
      continue;
    }

    // is this a common, inserted, or deleted file?
    int comparison = strcoll(dir_entries_one[i].c_str(), dir_entries_two[j].c_str());

    std::optional<std::string> file_path_original;
    std::optional<std::string> file_path_modified;
    if(comparison <= 0) ++i, file_path_original = path_original;
    if(comparison >= 0) ++j, file_path_modified = path_modified;

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(file_path_original, file_path_modified);

  }

  // process all non-directory files that are remaining in the original version
  for (; i < n; ++i) {

    // form the full path
    path_original.replace(basesize_original, std::string::npos, dir_entries_one[i]);

    svn_dirent_t * dirent_original;
    svn_ra_stat(session, path_original.c_str(), revision_one, &dirent_original, pool);

    // skip directories
    if(dirent_original->kind != svn_node_file) {
      ++i;
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(path_original, std::optional<std::string>());

  }

  // process all non-directory files that are remaining in the modified version
  for ( ; j < m; ++j) {

    // form the full path
    path_modified.replace(basesize_modified, std::string::npos, dir_entries_two[j]);

    svn_dirent_t * dirent_modified;
    svn_ra_stat(session, path_modified.c_str(), revision_two, &dirent_modified, pool);

    // skip directories
    if(dirent_modified->kind != svn_node_file) {
      ++j;
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(std::optional<std::string>(), path_modified);

  }

  // process all directories
  i = 0;
  j = 0;
  while (i < n && j < m) {

    path_original.replace(basesize_original, std::string::npos, dir_entries_one[i]);
    path_modified.replace(basesize_modified, std::string::npos, dir_entries_two[j]);

    svn_dirent_t * dirent_original;
    svn_ra_stat(session, path_original.c_str(), revision_one, &dirent_original, pool);

    svn_dirent_t * dirent_modified;
    svn_ra_stat(session, path_modified.c_str(), revision_two, &dirent_modified, pool);


    // skip directories
    if(dirent_original->kind != svn_node_dir) {
      ++i;
      continue;
    }

    if(dirent_modified->kind != svn_node_dir) {
      ++j;
      continue;
    }

    // is this a common, inserted, or deleted directory?
    int comparison = strcoll(dir_entries_one[i].c_str(), dir_entries_two[j].c_str());

    std::optional<std::string> directory_path_one;
    std::optional<std::string> directory_path_two;
    if(comparison <= 0) ++i, directory_path_one = path_original;
    if(comparison >= 0) ++j, directory_path_two = path_modified;

    // process these directories
    directory(directory_path_one, directory_path_two);

  }

  // process all directories that remain in the original version
  for ( ; i < n; ++i) {

    path_original.replace(basesize_original, std::string::npos, dir_entries_one[i]);

    svn_dirent_t * dirent_original;
    svn_ra_stat(session, path_original.c_str(), revision_one, &dirent_original, pool);

    // skip directories
    if(dirent_original->kind != svn_node_dir) {
      ++i;
      continue;
    }

    // process this directory
    directory(path_original, std::optional<std::string>());

  }

  // process all directories that remain in the modified version
  for ( ; j < m; ++j) {

    path_modified.replace(basesize_modified, std::string::npos, dir_entries_two[j]);

    svn_dirent_t * dirent_modified;
    svn_ra_stat(session, path_modified.c_str(), revision_two, &dirent_modified, pool);

    // skip directories
    if(dirent_modified->kind != svn_node_dir) {
      ++j;
      continue;
    }

    directory(std::optional<std::string>(), path_modified);

  }

#undef PATH_SEPARATOR

}

void srcdiff_input_source_svn::process_files_from() {

  this->revision_one = options.revision_one;
  this->revision_two = options.revision_two;

  srcdiff_translator translator(options.srcdiff_filename, options.flags, options.methods, options.archive,
                                options.unit_filename,
                                options.view_options,
                                options.summary_type_str);

  this->translator = &translator;

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
        if (isspace(line[i])) {
          line[i] = 0;
    }
        else {
          break;
    }

      }

      std::string::size_type sep_pos = line.find('|');
      std::string path_original = line.substr(0, sep_pos);
      std::string path_modified = line.substr(sep_pos + 1);

      std::optional<std::string> path = path_original;
      svn_revnum_t revision = options.revision_one;
      if(*path == "") {

         path = path_modified;
         revision = revision_two;

      }

      svn_dirent_t * dirent;
      svn_ra_stat(session, path->c_str(), revision, &dirent, pool);

      if(dirent->kind == svn_node_file) file(path_original, path_modified);

    }

  } catch (uri_stream_error) {

    fprintf(stderr, "%s error: file/URI \'%s\' does not exist.\n", "srcdiff", options.files_from_name->c_str());
    exit(EXIT_FAILURE);

  }

}

srcdiff_input_source_svn::input_context * srcdiff_input_source_svn::open(const char * uri) const {

  input_context * context = new input_context;

  apr_allocator_t * allocator = 0;
  apr_allocator_create(&allocator);

  apr_pool_t * pool = 0;
  apr_pool_create_ex(&pool, NULL, abortfunc, allocator);

  context->pool = pool;

  svn_stringbuf_t * str = svn_stringbuf_create_ensure(0, context->pool);
  context->stream = svn_stream_from_stringbuf(str, context->pool);

  svn_revnum_t fetched_rev = 0;
  apr_hash_t * props = 0;

  // parse uri
  const char * end = index(uri, '@');

  const char * path = strndup(uri, end - uri);

  svn_revnum_t revision = atoi(end + 1);

  mutex.lock();
  svn_ra_get_file(session, path, revision, context->stream, &fetched_rev, &props, context->pool);
  mutex.unlock();

  return context;

}

ssize_t srcdiff_input_source_svn::read(void * context, void * buffer, size_t len) {

  input_context * ctx = (input_context *)context;

  apr_size_t length = len;

  svn_error_t * error = svn_stream_read(ctx->stream, (char *)buffer, &length);

  if(error) return 0;

  return length;
}

int srcdiff_input_source_svn::close(void * context) {

  input_context * ctx = (input_context *)context;

  svn_stream_close(ctx->stream);

  apr_pool_destroy(ctx->pool);

  delete ctx;

  return 1;
}
