/*
  svn_io.hpp

  Functions for reading a file form svn

  Michael J. Decker
  mdecker6@kent.edu
*/


#include <apr.h>
#include <svn_fs.h>
#include <svn_repos.h>
#include <svn_ra.h>
#include <svn_client.h>
#include <svn_types.h>
#include <svn_dirent_uri.h>
#include <svn_auth.h>
#include <svn_cmdline.h>
#include <svn_string.h>
#include <svn_subst.h>

#include <srcdiff_translator.hpp>
#include <Options.hpp>

#ifndef INCLUDED_SRCDIFF_SVN_INPUT_HPP
#define INCLUDED_SRCDIFF_SVN_INPUT_HPP

class srcdiff_svn_input {

protected:

  svn_ra_session_t * session;
  apr_pool_t pool;

private:

public:

  srcdiff_svn_input(std::string url);
  ~srcdiff_svn_input();

  struct svn_context {

    svn_stream_t * stream;
    apr_pool_t * pool;

  };

  static int svnReadMatch(const char * uri);
  static void * svnReadOpen(const char * uri);
  static int svnRead(void * context, char * buffer, int len);
  static int svnReadClose(void * context);

};

void svn_process_dir(svn_ra_session_t * session, svn_revnum_t revision_one, svn_revnum_t revision_two, apr_pool_t * pool, srcdiff_translator& translator, OPTION_TYPE options,
  const char * directory_old, int directory_length_old, const char * directory_new, int directory_length_new);

void svn_process_file(svn_ra_session_t * session,  svn_revnum_t revision_one, svn_revnum_t revision_two, apr_pool_t * pool, srcdiff_translator& translator,
  const char * path_one, const char * path_two, int directory_length_old, int directory_length_new, const char * svn_url, OPTION_TYPE options,
);

void svn_process_session(svn_revnum_t revision_one, svn_revnum_t revision_two, srcdiff_translator & translator, OPTION_TYPE options);

void svn_process_session_all(svn_revnum_t start_rev, svn_revnum_t end_rev, OPTION_TYPE options);

void svn_process_session_file(const char * list, svn_revnum_t revision_one, svn_revnum_t revision_two, OPTION_TYPE options);

void svn_session_destroy(svn_ra_session_t * session, apr_pool_t * pool);


#endif
