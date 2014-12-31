/*
  svn_io.hpp

  Functions for reading a file form svn

  Michael J. Decker
  mdecker6@kent.edu
*/

#ifndef INCLUDED_SRCDIFF_SVN_INPUT_HPP
#define INCLUDED_SRCDIFF_SVN_INPUT_HPP

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

#include <srcdiff_options.hpp>
#include <srcdiff_translator.hpp>

class srcdiff_svn_input {

protected:

  srcdiff_options & options;
  srcdiff_translator * translator;

private:

  svn_ra_session_t * session;
  apr_pool_t * pool;

  svn_revnum_t revision_one;
  svn_revnum_t revision_two;

public:

  srcdiff_svn_input(srcdiff_options & options);
  ~srcdiff_svn_input();

  void session_single(svn_revnum_t revision_one, svn_revnum_t revision_two);

  void session_files_from(svn_revnum_t revision_one, svn_revnum_t revision_two, const char * list);

  void session_range(svn_revnum_t start_revision, svn_revnum_t end_revision);

  void file(const char * path_one, const char * path_two, int directory_length_old, int directory_length_new);

  void directory(const char * directory_old, int directory_length_old, const char * directory_new, int directory_length_new);

  struct svn_context {

    svn_stream_t * stream;
    apr_pool_t * pool;

  };

  svn_context * open(const char * uri) const;

  static int read(void * context, char * buffer, int len);
  static int close(void * context);

};

#endif
