/*
  svn_t.cpp

  Testing as subversion.

  Michael J. Decker
  mdecker6@kent.edu
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "svn_io.hpp"
#include "Language.hpp"
#include "srcmlapps.hpp"

svn_ra_session_t * global_session;

struct svn_context {

  svn_stream_t * stream;
  apr_pool_t * pool;

};

int abortfunc(int retcode) {

  std::cout << retcode << '\n';

  return retcode;
}

void svn_process_dir(svn_ra_session_t * session, svn_revnum_t revision_one, svn_revnum_t revision_two, apr_pool_t * pool, srcDiffTranslator& translator, const char * directory_old, int directory_length_old, const char * directory_new, int directory_length_new, OPTION_TYPE options, int language, int& count, int & skipped, int & error, bool & showinput, bool shownumber) {

  apr_hash_t * dirents_one;
  svn_revnum_t fetched_rev_one;

  svn_ra_get_dir2(session, &dirents_one, &fetched_rev_one, NULL, directory_old, revision_one, SVN_DIRENT_ALL, pool);

  apr_hash_t * dirents_two;
  svn_revnum_t fetched_rev_two;

  svn_ra_get_dir2(session, &dirents_two, &fetched_rev_two, NULL, directory_new, revision_two, SVN_DIRENT_ALL, pool);

  apr_hash_index_t * item;
  const void * key;
  void * value;

  std::vector<std::string> dir_entries_one;
  for (item = apr_hash_first(pool, dirents_one); item; item = apr_hash_next(item)) {

    apr_hash_this(item, &key, NULL, &value);

    dir_entries_one.push_back((const char *)key);
  }

  sort(dir_entries_one.begin(), dir_entries_one.end());

  std::vector<std::string> dir_entries_two;
  for (item = apr_hash_first(pool, dirents_two); item; item = apr_hash_next(item)) {

    apr_hash_this(item, &key, NULL, &value);

    dir_entries_two.push_back((const char *)key);
  }

  sort(dir_entries_two.begin(), dir_entries_two.end());

  //
  for(unsigned int i = 0; i < dir_entries_one.size(); ++i) {

    std::string new_path = directory_old;
    if(directory_old && directory_old[0] != 0)
      new_path += "/";
    new_path += dir_entries_one.at(i);

    apr_allocator_t * allocator;
    apr_allocator_create(&allocator);

    apr_pool_t * new_pool;
    apr_pool_create_ex(&new_pool, NULL, abortfunc, allocator);

    svn_dirent_t * dirent;
    svn_ra_stat(session, new_path.c_str(), revision_one, &dirent, pool);

    if(dirent->kind == svn_node_file)
      svn_process_file(session, revision_one, revision_two, new_pool, translator, new_path.c_str(), new_path.c_str(), 0,0, options, language, count, skipped, error, showinput, shownumber);
    else if(dirent->kind == svn_node_dir)
      svn_process_dir(session, revision_one, revision_two, new_pool, translator, new_path.c_str(), 0, new_path.c_str(), 0, options, language, count, skipped, error, showinput, shownumber);
    else if(dirent->kind == svn_node_none)
      fprintf(stderr, "%s\n", "Path does not exist");
    else if(dirent->kind == svn_node_unknown)
      fprintf(stderr, "%s\n", "Unknown");

    apr_pool_destroy(new_pool);

  }

}

void svn_process_file(svn_ra_session_t * session, svn_revnum_t revision_one, svn_revnum_t revision_two, apr_pool_t * pool, srcDiffTranslator& translator, const char* path_one, const char* path_two, int directory_length_old, int directory_length_new, OPTION_TYPE options, int language, int& count, int & skipped, int & error, bool & showinput, bool shownumber) {

  // Do not nest individual files
  OPTION_TYPE local_options = options & ~OPTION_NESTED;

  std::string filename = path_one[0] ? path_one + directory_length_old : path_one;
  if(path_two[0] == 0 || strcmp(path_one + directory_length_old, path_two + directory_length_new) != 0) {

    filename += "|";
    filename += path_two[0] ? path_two + directory_length_new : path_two;

  }

  // set path to include revision
  path_one += '@';
  path_one += revision_one;

  path_two += '@';
  path_one += revision_two;

  // Remove eventually
  int real_language = language ? language : Language::getLanguageFromFilename(path_one);

  real_language = real_language ? real_language : Language::getLanguageFromFilename(path_two);

  if (!(real_language == Language::LANGUAGE_JAVA || real_language == Language::LANGUAGE_ASPECTJ))
    local_options |= OPTION_CPP;

  if (!real_language && !isoption(options, OPTION_QUIET)) {
    fprintf(stderr, !shownumber ? "Skipped '%s|%s':  Unregistered extension\n" :
            "    - '%s|%s'\tSkipped: Unregistered extension\n",
            path_one, path_two);

    ++skipped;

    return;
  }

  ++count;

  if(showinput && !isoption(local_options, OPTION_QUIET))
    fprintf(stderr, "%5d '%s|%s'\n", count, path_one, path_two);

  translator.translate(path_one, path_two, local_options,
                       NULL,
                       filename.c_str(),
                       0,
                       real_language);


}

void svn_process_session(svn_revnum_t revision_one, svn_revnum_t revision_two, srcDiffTranslator& translator, const char * url, OPTION_TYPE options, int language, int& count, int & skipped, int & error, bool & showinput, bool shownumber) {

  apr_initialize();

  apr_allocator_t * allocator;
  apr_allocator_create(&allocator);

  apr_pool_t * pool;
  apr_pool_create_ex(&pool, NULL, abortfunc, allocator);

  svn_client_ctx_t * ctx;
  apr_hash_t * cfg_hash;
  svn_config_t * cfg_config;

  svn_ra_initialize(pool);
  svn_config_get_config(&cfg_hash, NULL, pool);
  svn_client_create_context(&ctx, pool);
  //svn_client_create_context2(&ctx, cfg_hash, pool);
  ctx->config = cfg_hash;
  cfg_config = (svn_config_t *)apr_hash_get(ctx->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING);

  svn_boolean_t non_interactive = false;
  const char * auth_username = "";
  const char * auth_password = "";
  const char * config_dir = 0;

  svn_boolean_t no_auth_cache = false;
  svn_boolean_t trust_server_cert = true;

  svn_auth_baton_t * ab;
  svn_cmdline_create_auth_baton(&ab, non_interactive, auth_username, auth_password, config_dir, no_auth_cache, trust_server_cert,
                                cfg_config, ctx->cancel_func, ctx->cancel_baton, pool);

  ctx->auth_baton = ab;
  ctx->conflict_func = NULL;
  ctx->conflict_baton = NULL;

  svn_ra_session_t * session;
  svn_error_t * svn_error = svn_client_open_ra_session(&session, url, ctx, pool);
  global_session = session;

  if(svn_error)
    fprintf(stderr, "%s\n", svn_error->message);

  const char * path = "";
  apr_pool_t * path_pool;
  apr_pool_create_ex(&path_pool, NULL, abortfunc, allocator);

  svn_dirent_t * dirent;
  svn_ra_stat(session, path, revision_one, &dirent, path_pool);

  if(dirent->kind == svn_node_file)
    svn_process_file(session, revision_one, revision_two, path_pool, translator, path, path, 0,0, options, language ? language : Language::getLanguageFromFilename(url), count, skipped, error, showinput, shownumber);
  else if(dirent->kind == svn_node_dir)
    svn_process_dir(session, revision_one, revision_two, path_pool, translator, path, 0, path, 0, options, language, count, skipped, error, showinput, shownumber);
  else if(dirent->kind == svn_node_none)
    fprintf(stderr, "%s\n", "Path does not exist");
  else if(dirent->kind == svn_node_unknown)
    fprintf(stderr, "%s\n", "Unknown");

  apr_pool_destroy(path_pool);

  apr_terminate();

}

// check svn match
int svnReadMatch(const char * URI) {

  return 1;
}

void * svnReadOpen(const char * URI) {

  svn_context * context = new svn_context;

  apr_allocator_t * allocator;
  apr_allocator_create(&allocator);

  apr_pool_t * pool;
  apr_pool_create_ex(&pool, NULL, abortfunc, allocator);

  context->pool = pool;

  svn_stringbuf_t * str = svn_stringbuf_create_ensure(0, context->pool);
  context->stream = svn_stream_from_stringbuf(str, context->pool);

  svn_revnum_t fetched_rev;
  apr_hash_t * props;

  // parse uri
  const char * end = index(URI, '@');

  const char * path = 0;
  path = strndup(URI, end - path);

  svn_revnum_t revision = atoi(end + 1);

  svn_ra_get_file(global_session, path, revision, context->stream, &fetched_rev, &props, context->pool);

  return context;

}

// read from the URI
int svnRead(void * context, char * buffer, int len) {

  svn_context * ctx = (svn_context *)context;

  apr_size_t length = len;

  svn_error_t * error = svn_stream_read(ctx->stream, buffer, &length);

  if(error) return 0;

  if(length < 0) return 0;

  return length;
}

// close the open file
int svnReadClose(void * context) {

  svn_context * ctx = (svn_context *)context;

  apr_pool_destroy(ctx->pool);

  delete ctx;

  return 1;
}
