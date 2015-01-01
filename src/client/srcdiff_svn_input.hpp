/*
  svn_io.hpp

  Functions for reading a file form svn

  Michael J. Decker
  mdecker6@kent.edu
*/

#if SVN

#ifndef INCLUDED_SRCDIFF_SVN_INPUT_HPP
#define INCLUDED_SRCDIFF_SVN_INPUT_HPP

#include <srcdiff_source_input.hpp>

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

class srcdiff_svn_input : public srcdiff_source_input {

protected:

private:

  svn_ra_session_t * session;
  apr_pool_t * pool;

  svn_revnum_t revision_one;
  svn_revnum_t revision_two;

public:

  srcdiff_svn_input(srcdiff_options & options);
  ~srcdiff_svn_input();

  void session_single();
  void session_files_from(const std::string & list);
  void session_range();

  virtual void file(const boost::optional<std::string> & path_one, const boost::optional<std::string> & path_two, int directory_length_old, int directory_length_new);
  virtual void directory(const boost::optional<std::string> & directory_old, int directory_length_old, const boost::optional<std::string> & directory_new, int directory_length_new);
  virtual void files_from();

  struct svn_context {

    svn_stream_t * stream;
    apr_pool_t * pool;

  };

  svn_context * open(const char * uri) const;
  static int read(void * context, char * buffer, int len);
  static int close(void * context);

};

#endif

#endif
