// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_input_source_svn.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
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

  virtual const char * get_language(const std::optional<std::string> & path_original, const std::optional<std::string> & path_modified);

  void session_single();
  void session_range();

  virtual void process_file(const std::optional<std::string> & path_original,
                            const std::optional<std::string> & path_modified);
  virtual void process_directory(const std::optional<std::string> & directory_original,
                                 const std::optional<std::string> & directory_modified);
  virtual void process_files_from();

  struct input_context {

    svn_stream_t * stream;
    apr_pool_t * pool;

  };

  input_context * open(const char * uri) const;
  static ssize_t read(void * context, void * buffer, size_t len);
  static int close(void * context);

};

#endif

#endif
