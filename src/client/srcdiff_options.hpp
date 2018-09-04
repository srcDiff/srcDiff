#ifndef INCLUDED_CLIENT_OPTIONS_HPP
#define INCLUDED_CLIENT_OPTIONS_HPP

#include <methods.hpp>
#include <srcml.h>

#include <boost/optional.hpp>
#include <boost/any.hpp>

#include <string>
#include <vector>

#if SVN
#include <svn_version.h>
#endif

#ifdef __GNUC__
typedef unsigned long long OPTION_TYPE;
#else
typedef unsigned __int64 OPTION_TYPE;
#endif

#ifdef __GNUC__
#define ull(a) a##ULL
#else
#define ull(a) a##i64
#endif

inline bool is_option(OPTION_TYPE options, OPTION_TYPE flag) {
    return (flag & options) > 0;
}

// @srcdiff
const OPTION_TYPE OPTION_VERBOSE               = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_QUIET                 = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_COMPRESS              = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_VISUALIZE             = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SAME                  = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_PURE                  = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_CHANGE                = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SRCDIFFONLY           = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_DIFFONLY              = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SVN_CONTINUOUS        = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SVN                   = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_RECURSIVE             = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_UNIFIED_VIEW          = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SIDE_BY_SIDE_VIEW     = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_DIFFDOC_VIEW          = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_HTML_VIEW             = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_IGNORE_ALL_WHITESPACE = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_IGNORE_WHITESPACE     = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_IGNORE_COMMENTS       = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SUMMARY               = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_BURST                 = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SRCML                 = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_STRING_SPLITTING      = ull(1) << __COUNTER__;

struct srcdiff_options
{

  srcdiff_options() : flags(OPTION_STRING_SPLITTING) {}

  srcml_archive * archive;

  std::vector<std::pair<std::string, std::string>> input_pairs;
  std::string srcdiff_filename;
  boost::optional<std::string> files_from_name;
  boost::optional<std::string> unit_filename;

  OPTION_TYPE flags;
  METHOD_TYPE methods;

  struct view_options_t {
    std::string syntax_highlight;
    std::string theme;
    boost::any unified_view_context;
    int side_by_side_tab_size;
    boost::optional<std::string> srcdiff_filename;
  } view_options;

  boost::optional<std::string> summary_type_str;

#if SVN
  boost::optional<std::string> svn_url;
  svn_revnum_t revision_one;
  svn_revnum_t revision_two;
#endif
  
#if GIT
  boost::optional<std::string> git_url;
  std::string git_revision_one;
  std::string git_revision_two;
  boost::optional<std::string> original_repository;
  boost::optional<std::string> modified_repository;
#endif

};

const srcdiff_options & process_command_line(int argc, char* argv[]);

#endif
