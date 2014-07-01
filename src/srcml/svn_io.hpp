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

#include <srcDiffTranslator.hpp>
#include <Options.hpp>

#ifndef INCLUDDED_SVN_IO_HPP
#define INCLUDDED_SVN_IO_HPP

struct svn_context {

  svn_stream_t * stream;
  apr_pool_t * pool;

};

void svn_process_dir(svn_ra_session_t * session, svn_revnum_t revision_one, svn_revnum_t revision_two, apr_pool_t * pool, srcDiffTranslator& translator, const char * directory_old, int directory_length_old, const char * directory_new, int directory_length_new, OPTION_TYPE options, int& count, int & skipped, int & error, bool & showinput, bool shownumber);

void svn_process_file(svn_ra_session_t * session,  svn_revnum_t revision_one, svn_revnum_t revision_two, apr_pool_t * pool, srcDiffTranslator& translator, const char* path_one, const char* path_two, int directory_length_old, int directory_length_new, OPTION_TYPE options, int& count, int & skipped, int & error, bool & showinput, bool shownumber);

void svn_process_session(svn_revnum_t revision_one, svn_revnum_t revision_two, srcDiffTranslator & translator, const char * url, OPTION_TYPE options, int & count, int & skipped, int & error, bool & showinput, bool shownumber);

void svn_process_session_all(svn_revnum_t start_rev, svn_revnum_t end_rev, const char * url, int& count, int & skipped, int & error, bool & showinput, bool shownumber,
                             const char* srcdiff_filename,  // filename of result srcDiff file
                             METHOD_TYPE method,
                             std::string css,
                             srcml_archive * archive,
                             OPTION_TYPE options);

void svn_process_session_file(const char * list, svn_revnum_t revision_one, svn_revnum_t revision_two, const char * url, int& count, int & skipped, int & error, bool & showinput, bool shownumber,
                             const char* srcdiff_filename,  // filename of result srcDiff file
                             METHOD_TYPE method,
                             std::string css,
                             srcml_archive * archive,
                             OPTION_TYPE options);

// check svn match
int svnReadMatch(const char * URI);

// svn open
void * svnReadOpen(const char * URI);

// read from the URI
int svnRead(void * context, char * buffer, int len);

// close the open file
int svnReadClose(void * context);

#endif
