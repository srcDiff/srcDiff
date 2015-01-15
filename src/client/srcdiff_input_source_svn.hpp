/*
  svn_io.hpp

  Functions for reading a file form svn

  Michael J. Decker
  mdecker6@kent.edu
*/

#if SVN

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_SVN_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_SVN_HPP

#include <srcdiff_input_source.hpp>

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

class srcdiff_input_source_svn : public srcdiff_input_source {

protected:

private:

  svn_ra_session_t * session;
  apr_pool_t * pool;

  svn_revnum_t revision_one;
  svn_revnum_t revision_two;

public:

  srcdiff_input_source_svn(const srcdiff_options & options);
  virtual ~srcdiff_input_source_svn();

  virtual void consume();

  void session_single();
  void session_range();

  virtual void file(const boost::optional<std::string> & path_one, const void * context_old,
                    const boost::optional<std::string> & path_two, const void * context_new);
  virtual void directory(const boost::optional<std::string> & directory_old, const void * context_old,
                         const boost::optional<std::string> & directory_new, const void * context_new);
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
