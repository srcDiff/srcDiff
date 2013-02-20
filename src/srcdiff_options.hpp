#ifndef INCLUDED_SRCDIFF_OPTIONS_HPP
#define INCLUDED_SRCDIFF_OPTIONS_HPP

#include "srcdiff_uri.hpp"
#include "Methods.hpp"

const char* const DEBUG_FLAG = "debug";
const char DEBUG_FLAG_SHORT = 'g';

const char* const LITERAL_FLAG = "literal";

const char* const OPERATOR_FLAG = "operator";

const char* const MODIFIER_FLAG = "modifier";

const char* const POSITION_FLAG = "position";
const char* const TABS_FLAG = "tabs";
const int TABS_FLAG_CODE = 256 + 0;
const char* const TABS_FLAG_FULL = "tabs=NUMBER";
const int DEFAULT_TABSIZE = 8;

const char* const INTERACTIVE_FLAG = "interactive";
const char INTERACTIVE_FLAG_SHORT = 'c';

const char* const CPP_MARKUP_ELSE_FLAG = "cpp-markup-else";
const int CPP_MARKUP_ELSE_FLAG_CODE = 256 + 1;

const char* const CPP_TEXTONLY_ELSE_FLAG = "cpp-text-else";
const int CPP_TEXTONLY_ELSE_FLAG_CODE = 256 + 2;

const char* const CPP_MARKUP_IF0_FLAG = "cpp-markup-if0";
const int CPP_MARKUP_IF0_FLAG_CODE = 256 + 3;

const char* const CPP_TEXTONLY_IF0_FLAG = "cpp-text-if0";
const int CPP_TEXTONLY_IF0_FLAG_CODE = 256 + 4;

const char* const EXPRESSION_MODE_FLAG = "expression";
const char EXPRESSION_MODE_FLAG_SHORT = 'e';

const char* const SELF_VERSION_FLAG = "self-version";

const char* const FILELIST_FLAG = "files-from";
const int FILELIST_FLAG_CODE = 256 + 5;
const char* const FILELIST_FLAG_FULL = "files-from=INPUT";

const char* const XMLNS_FLAG = "xmlns";
const int XMLNS_FLAG_CODE = 256 + 6;
const char* const XMLNS_DEFAULT_FLAG_FULL = "xmlns=URI";
const char* const XMLNS_FLAG_FULL = "xmlns:PREFIX=URI";

const char* const REGISTER_EXT_FLAG = "register-ext";
const char* const REGISTER_EXT_FLAG_FULL = "register-ext EXT=LANG";
const int REGISTER_EXT_FLAG_CODE = 256 + 7;

const char* const OLD_FILENAME_FLAG = "old-filename";
const int OLD_FILENAME_FLAG_CODE = 256 + 8;

const char* const RECURSIVE_FLAG = "recursive";
const int RECURSIVE_FLAG_CODE = 256 + 9;

const char* const REVISION_FLAG = "revision";
const int REVISION_FLAG_CODE = 256 + 10;

const char* const METHOD_FLAG = "method";
const int METHOD_FLAG_CODE = 256 + 11;

const char* const NO_THREAD_FLAG = "no-thread";
const int NO_THREAD_FLAG_CODE = 256 + 12;

const char* const VISUALIZE_FLAG = "visualize";
const int VISUALIZE_FLAG_CODE = 256 + 13;

const char* const SAME_FLAG = "same";
const int SAME_FLAG_CODE = 256 + 14;

const char* const PURE_FLAG = "pure";
const int PURE_FLAG_CODE = 256 + 15;

const char* const CHANGE_FLAG = "change";
const int CHANGE_FLAG_CODE = 256 + 16;

const char* const SRCDIFFONLY_FLAG = "srcdiff-only";
const int SRCDIFFONLY_FLAG_CODE = 256 + 17;

const char* const DIFFONLY_FLAG = "diff-only";
const int DIFFONLY_FLAG_CODE = 256 + 18;

const char* const NO_SAME_FLAG = "no-same";
const int NO_SAME_FLAG_CODE = 256 + 19;

const char* const NO_PURE_FLAG = "no-pure";
const int NO_PURE_FLAG_CODE = 256 + 20;

const char * const SVN_FLAG = "svn";
const int SVN_FLAG_CODE = 256 + 21;

const char * const SVN_START_FLAG = "svn-start";
const int SVN_START_FLAG_CODE = 256 + 22;

struct process_options
{
  // options
  // output filename
  const char* srcdiff_filename;
  const char* file_list_name;
  const char* input_format;
  const char* output_format;
  int language;
  const char* src_encoding;
  const char* xml_encoding;
  const char* given_directory;
  const char* given_filename;
  const char* given_version;
  int tabsize;
  bool prefixchange[num_prefixes];
  METHOD_TYPE method;
  std::string css_url;

  const char * svn_url;
  int revision_one;
  int revision_two;
};

int process_args(int argc, char* argv[], process_options & poptions, OPTION_TYPE & options, const char * urisprefix[]);
void process_method(char * optarg, process_options & poptions);

int option_error_status(int optopt);

void output_settings(const char * name);

void output_features(const char * name);

void output_help(const char* name);

void output_version(const char* name);

#endif
