/*
  svn_t.cpp

  Testing as subversion.

  Michael J. Decker
  mdecker6@kent.edu
*/

#if SVN

#include <srcdiff_svn_input.hpp>

#include <srcdiff_input_svn.hpp>

#include <thread>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>

#include <URIStream.hpp>

#include <svn_version.h>

std::mutex mutex;

int abortfunc(int retcode) {

  std::cout << retcode << '\n';

  return retcode;
}

srcdiff_svn_input::srcdiff_svn_input(srcdiff_options & options) : options(options) {


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

srcdiff_svn_input::~srcdiff_svn_input() {

  apr_pool_destroy(pool);

  apr_terminate();

}

void srcdiff_svn_input::session_single(svn_revnum_t revision_one, svn_revnum_t revision_two) {

  this->revision_one = revision_one;
  this->revision_two = revision_two;

  srcdiff_translator translator(options.srcdiff_filename->c_str(),
                                options.methods,
                                options.css_url ? *options.css_url : std::string(),
                                options.archive,
                                options.flags,
                                options.number_context_lines);

  this->translator = &translator;

  /** @todo two pools */

  const char * path = "";

  svn_dirent_t * dirent;
  svn_ra_stat(session, path, revision_one, &dirent, pool);

  if(dirent->kind == svn_node_file)         file(path, path, 0, 0);
  else if(dirent->kind == svn_node_dir)     directory(path, 0, path, 0);
  else if(dirent->kind == svn_node_none)    fprintf(stderr, "%s\n", "Path does not exist");
  else if(dirent->kind == svn_node_unknown) fprintf(stderr, "%s\n", "Unknown");

}

void srcdiff_svn_input::session_files_from(svn_revnum_t revision_one, svn_revnum_t revision_two, const char * list) {


  srcdiff_translator translator(options.srcdiff_filename->c_str(),
                                options.methods,
                                options.css_url ? *options.css_url : std::string(),
                                options.archive,
                                options.flags,
                                options.number_context_lines);

  this->translator = &translator;

  try {

    // translate all the filenames listed in the named file
    // Use libxml2 routines so that we can handle http:, file:, and gzipped files automagically
    std::ifstream input(list);
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

      std::string path_one = line.substr(0, line.find('|'));
      std::string path_two = line.substr(line.find('|') + 1);

      const char * path = path_one.c_str();
      svn_revnum_t revision = revision_one;
      if(path_one == "") {

         path = path_two.c_str();
         revision = revision_two;

      }

      svn_dirent_t * dirent;
      svn_ra_stat(session, path, revision, &dirent, pool);

      if(dirent->kind == svn_node_file)         file(path_one.c_str(), path_two.c_str(), 0,0);
      else if(dirent->kind == svn_node_dir)     fprintf(stderr, "Skipping directory: %s", path);
      else if(dirent->kind == svn_node_none)    fprintf(stderr, "%s\n", "Path does not exist");
      else if(dirent->kind == svn_node_unknown) fprintf(stderr, "%s\n", "Unknown");

    }

  } catch (URIStreamFileError) {

    fprintf(stderr, "%s error: file/URI \'%s\' does not exist.\n", "srcdiff", list);
    exit(EXIT_FAILURE);

  }

}

void srcdiff_svn_input::session_range(svn_revnum_t start_revision, svn_revnum_t end_revision) {

  if(start_revision == SVN_INVALID_REVNUM) revision_one = 1;
  else revision_one = start_revision;

  revision_two = revision_one + 1;

  svn_revnum_t last_revision = end_revision;
  if(last_revision == SVN_INVALID_REVNUM) svn_ra_get_latest_revnum(session, &last_revision, pool);

  for(; revision_one < last_revision; ++revision_one, ++revision_two) {

    std::ostringstream full_srcdiff(options.srcdiff_filename && *options.srcdiff_filename != "-" ? *options.srcdiff_filename : "", std::ios_base::ate);
    full_srcdiff << '_';
    full_srcdiff << revision_one;
    full_srcdiff << '-';
    full_srcdiff << revision_two;
    full_srcdiff << ".xml";

    srcdiff_translator translator(full_srcdiff.str().c_str(),
                                  options.methods,
                                  options.css_url ? *options.css_url : std::string(),
                                  options.archive,
                                  options.flags,
                                  options.number_context_lines);

    const char * path = "";
    svn_dirent_t * dirent;
    svn_ra_stat(session, path, revision_one, &dirent, pool);

    if(dirent->kind == svn_node_file)         file(path, path, 0, 0);
    else if(dirent->kind == svn_node_dir)     directory(path, 0, path, 0);
    else if(dirent->kind == svn_node_none)    fprintf(stderr, "%s\n", "Path does not exist");
    else if(dirent->kind == svn_node_unknown) fprintf(stderr, "%s\n", "Unknown");

  }

}

void srcdiff_svn_input::file(const char * path_one, const char * path_two, int directory_length_old, int directory_length_new) {

  std::string unit_filename = path_one[0] ? path_one + directory_length_old : path_one;
  if(path_two[0] == 0 || strcmp(path_one + directory_length_old, path_two + directory_length_new) != 0) {

    unit_filename += "|";
    unit_filename += path_two[0] ? path_two + directory_length_new : path_two;

  }

  if(srcml_archive_check_extension(options.archive, path_one) == SRCML_LANGUAGE_NONE && srcml_archive_check_extension(options.archive, path_two) == SRCML_LANGUAGE_NONE)
    return;

  // set path to include revision
  std::ostringstream file_one(path_one, std::ios_base::ate);
  file_one << '@';
  file_one << revision_one;

  std::ostringstream file_two(path_two, std::ios_base::ate);
  file_two << '@';
  file_two << revision_two;

  std::string file_old = file_one.str();
  std::string file_new = file_two.str();

  srcdiff_input_svn input_old(options.archive, file_old.c_str(), 0, *this);
  srcdiff_input_svn input_new(options.archive, file_new.c_str(), 0, *this);

  LineDiffRange line_diff_range(file_old, file_new, options.svn_url ? options.svn_url->c_str() : 0, 0);

  const char * path = path_one;
  if(path_one == 0 || path_one[0] == 0 || path_one[0] == '@')
    path = path_two;

  const char * end = index(path, '@');
  const char * filename = strndup(path, end - path);
  const char * language_string = srcml_archive_check_extension(options.archive, filename);
  free((void *)filename);

  translator->translate(input_old, input_new, line_diff_range, language_string, NULL, unit_filename.c_str(), 0);

}

void srcdiff_svn_input::directory(const char * directory_old, int directory_length_old, const char * directory_new, int directory_length_new) {

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

  apr_hash_t * dirents_one;
  svn_revnum_t fetched_rev_one;
  std::vector<std::string> dir_entries_one;

  if(directory_old) {

    apr_hash_index_t * item;
    const void * key;
    void * value;

    svn_ra_get_dir2(session, &dirents_one, &fetched_rev_one, NULL, directory_old, revision_one, SVN_DIRENT_ALL, pool);

    for (item = apr_hash_first(pool, dirents_one); item; item = apr_hash_next(item)) {

      apr_hash_this(item, &key, NULL, &value);

      dir_entries_one.push_back((const char *)key);
    }

    sort(dir_entries_one.begin(), dir_entries_one.end());

  }

  apr_hash_t * dirents_two;
  svn_revnum_t fetched_rev_two;
  std::vector<std::string> dir_entries_two;

  if(directory_new) {

    apr_hash_index_t * item;
    const void * key;
    void * value;

    svn_ra_get_dir2(session, &dirents_two, &fetched_rev_two, NULL, directory_new, revision_two, SVN_DIRENT_ALL, pool);

    for (item = apr_hash_first(pool, dirents_two); item; item = apr_hash_next(item)) {

      apr_hash_this(item, &key, NULL, &value);

      dir_entries_two.push_back((const char *)key);
    }

    sort(dir_entries_two.begin(), dir_entries_two.end());

  }

  std::string filename_old = "";
  int basesize_old = 0;

  // process directory
  if(directory_old) {

    filename_old = directory_old;
    if (filename_old != "" && !filename_old.empty() && filename_old[filename_old.size() - 1] != PATH_SEPARATOR)
      filename_old += PATH_SEPARATOR;
    basesize_old = filename_old.length();

  }

  std::string filename_new = "";
  int basesize_new = 0;
  if(directory_new) {

    filename_new = directory_new;
    if (filename_new != "" && !filename_new.empty() && filename_new[filename_new.size() - 1] != PATH_SEPARATOR)
      filename_new += PATH_SEPARATOR;
    basesize_new = filename_new.length();

  }

  // process all non-directory files
  int i = 0;
  int j = 0;
  int n = dir_entries_one.size();
  int m = dir_entries_two.size();
  while (i < n && j < m) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, dir_entries_one[i]);
    filename_new.replace(basesize_new, std::string::npos, dir_entries_two[j]);

    svn_dirent_t * dirent_old;
    svn_ra_stat(session, filename_old.c_str(), revision_one, &dirent_old, pool);

    svn_dirent_t * dirent_new;
    svn_ra_stat(session, filename_new.c_str(), revision_two, &dirent_new, pool);


    // skip directories
    if(dirent_old->kind != svn_node_file) {
      ++i;
      continue;
    }

    if(dirent_new->kind != svn_node_file) {
      ++j;
      continue;
    }

    apr_allocator_t * allocator;
    apr_allocator_create(&allocator);
/** @todo why a new pool */
    apr_pool_t * new_pool;
    apr_pool_create_ex(&new_pool, NULL, abortfunc, allocator);

    // is this a common, inserted, or deleted file?
    int comparison = strcoll(dir_entries_one[i].c_str(), dir_entries_two[j].c_str());

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(comparison <= 0 ? (++i, filename_old.c_str()) : "", comparison >= 0 ? (++j, filename_new.c_str()) : "",
         directory_length_old, directory_length_new);

    apr_pool_destroy(new_pool);

  }

  // process all non-directory files that are remaining in the old version
  for (; i < n; ++i) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, dir_entries_one[i]);

    svn_dirent_t * dirent_old;
    svn_ra_stat(session, filename_old.c_str(), revision_one, &dirent_old, pool);

    // skip directories
    if(dirent_old->kind != svn_node_file) {
      ++i;
      continue;
    }

    apr_allocator_t * allocator;
    apr_allocator_create(&allocator);

    apr_pool_t * new_pool;
    apr_pool_create_ex(&new_pool, NULL, abortfunc, allocator);

    // translate the file listed in the input file using the directory and filename extracted from the path
    file(filename_old.c_str(), "", directory_length_old, directory_length_new);

    apr_pool_destroy(new_pool);

  }

  // process all non-directory files that are remaining in the new version
  for ( ; j < m; ++j) {

    // form the full path
    filename_new.replace(basesize_new, std::string::npos, dir_entries_two[j]);

    svn_dirent_t * dirent_new;
    svn_ra_stat(session, filename_new.c_str(), revision_two, &dirent_new, pool);

    // skip directories
    if(dirent_new->kind != svn_node_file) {
      ++j;
      continue;
    }

    apr_allocator_t * allocator;
    apr_allocator_create(&allocator);

    apr_pool_t * new_pool;
    apr_pool_create_ex(&new_pool, NULL, abortfunc, allocator);

    // translate the file listed in the input file using the directory and filename extracted from the path
    file("", filename_new.c_str(), directory_length_old, directory_length_new);

    apr_pool_destroy(new_pool);

  }

  // no need to handle subdirectories, unless recursive
  //  if (!isoption(options, OPTION_RECURSIVE))
  //    return;

  // process all directories
  i = 0;
  j = 0;
  while (i < n && j < m) {

    filename_old.replace(basesize_old, std::string::npos, dir_entries_one[i]);
    filename_new.replace(basesize_new, std::string::npos, dir_entries_two[j]);

    svn_dirent_t * dirent_old;
    svn_ra_stat(session, filename_old.c_str(), revision_one, &dirent_old, pool);

    svn_dirent_t * dirent_new;
    svn_ra_stat(session, filename_new.c_str(), revision_two, &dirent_new, pool);


    // skip directories
    if(dirent_old->kind != svn_node_dir) {
      ++i;
      continue;
    }

    if(dirent_new->kind != svn_node_dir) {
      ++j;
      continue;
    }

    apr_allocator_t * allocator;
    apr_allocator_create(&allocator);

    apr_pool_t * new_pool;
    apr_pool_create_ex(&new_pool, NULL, abortfunc, allocator);

    // is this a common, inserted, or deleted directory?
    int comparison = strcoll(dir_entries_one[i].c_str(), dir_entries_two[j].c_str());

    // process these directories
    directory(comparison <= 0 ? (++i, filename_old.c_str()) : NULL, directory_length_old,
              comparison >= 0 ? (++j, filename_new.c_str()) : NULL, directory_length_new);

    apr_pool_destroy(new_pool);

  }

  // process all directories that remain in the old version
  for ( ; i < n; ++i) {

    filename_old.replace(basesize_old, std::string::npos, dir_entries_one[i]);

    svn_dirent_t * dirent_old;
    svn_ra_stat(session, filename_old.c_str(), revision_one, &dirent_old, pool);

    // skip directories
    if(dirent_old->kind != svn_node_dir) {
      ++i;
      continue;
    }

    apr_allocator_t * allocator;
    apr_allocator_create(&allocator);

    apr_pool_t * new_pool;
    apr_pool_create_ex(&new_pool, NULL, abortfunc, allocator);

    // process this directory
    directory(filename_old.c_str(), directory_length_old, NULL, directory_length_new);

    apr_pool_destroy(new_pool);

  }

  // process all directories that remain in the new version
  for ( ; j < m; ++j) {

    filename_new.replace(basesize_new, std::string::npos, dir_entries_two[j]);

    svn_dirent_t * dirent_new;
    svn_ra_stat(session, filename_new.c_str(), revision_two, &dirent_new, pool);

    // skip directories
    if(dirent_new->kind != svn_node_dir) {
      ++j;
      continue;
    }

    apr_allocator_t * allocator;
    apr_allocator_create(&allocator);

    apr_pool_t * new_pool;
    apr_pool_create_ex(&new_pool, NULL, abortfunc, allocator);

    directory(NULL, directory_length_old, filename_new.c_str(), directory_length_new);

    apr_pool_destroy(new_pool);

  }

#undef PATH_SEPARATOR

}

srcdiff_svn_input::svn_context * srcdiff_svn_input::open(const char * uri) const {

  svn_context * context = new svn_context;

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

int srcdiff_svn_input::read(void * context, char * buffer, int len) {

  svn_context * ctx = (svn_context *)context;

  apr_size_t length = len;

  svn_error_t * error = svn_stream_read(ctx->stream, buffer, &length);

  if(error) return 0;

  return length;
}

int srcdiff_svn_input::close(void * context) {

  svn_context * ctx = (svn_context *)context;

  svn_stream_close(ctx->stream);

  apr_pool_destroy(ctx->pool);

  delete ctx;

  return 1;
}

#endif
