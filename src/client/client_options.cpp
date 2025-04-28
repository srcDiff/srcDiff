// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file options.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <client_options.hpp>
#include <constants.hpp>

#include <libxml/parser.h>
#include <CLI/CLI.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>

namespace srcdiff {

// this is the options object that will store all the cli options once they are
// parsed. it is global so that it never goes out of scope and gets destroyed
client_options options;

#define PROGRAM_NAME "srcdiff"
#define EMAIL_ADDRESS "srcmldev@gmail.com"

// width of each of the two columns of help text that CLI11 displays
const unsigned COLUMN_WIDTH = 50;

// Callback functions that process or respond to flags or options being set:

std::string get_version() {

  std::stringstream out;

  out << PROGRAM_NAME << ' ' << "0.2.0\n"
      << "libsrcml " << srcml_version_string() << "\n";

  if(atoi(xmlParserVersion) == LIBXML_VERSION) {
    out << "libxml " << LIBXML_VERSION;
  } else {
    out << "libxml " << xmlParserVersion << " (Compiled " << LIBXML_VERSION << ")";
  }

  return out.str();
}

// processes input file arguments. this accepts a vector of file names or a
// vector where each element consists of two file names separated by a pipe,
// e.g. "orig.cpp|mod.cpp"
void option_input_file(const std::vector<std::string> & arg) {

  options.input_pairs.reserve(arg.size());

  for(std::vector<std::string>::size_type pos = 0; pos < arg.size(); pos += 1) {

    std::string::size_type sep_pos = arg[pos].find('|');
    std::string::size_type ext_pos = arg[pos].rfind(".");
    if(sep_pos != std::string::npos) {
      std::string path_original = arg[pos].substr(0, sep_pos);
      std::string path_modified = arg[pos].substr(sep_pos + 1);
      options.input_pairs.push_back(std::make_pair(path_original, path_modified));
    } else if(ext_pos != std::string::npos && arg[pos].substr(ext_pos + 1) == "xml") {
      options.flags |= OPTION_VIEW_XML;
      options.input_pairs.push_back(std::make_pair(arg[pos], ""));
    } else {

      if((pos + 1) >= arg.size()) {
        throw CLI::ValidationError("Odd number of input files.");
      }
      options.input_pairs.push_back(std::make_pair(arg[pos], arg[pos + 1]));
      ++pos;
    }

  }

  if(options.input_pairs.size() > 1) {
    srcml_archive_disable_solitary_unit(options.archive);
  }

}

// processes the --files-from cli argument
void option_files_from(const std::string & filename) {

    options.files_from_name = filename;

    std::ifstream input_file(filename);
    if(!input_file) throw CLI::FileError::Missing(filename);

    size_t line_count = 0;
    std::string line;
    while(std::getline(input_file, line)) {

      int white_length = std::strspn(line.c_str(), " \t\f");
      line.erase(0, white_length);
      // skip blank lines or comment lines
      if (line[0] == '\0' || line[0] == '#') {
        continue;
      } else if (
        std::count_if(line.begin(), line.end(), [](char c) { return c == '|'; }) != 1) {
        throw CLI::ValidationError(
          "The following input line did not consist of two filenames separated by '|':\n"+
          line
        );
      }

      ++line_count;
      if(line_count > 1) break;
    }

    if(line_count > 1) {
      srcml_archive_disable_solitary_unit(options.archive);
    } else if (line_count < 1) {
      throw CLI::ValidationError("No input file pairs could be obtained from " + filename);
    }

}

// processing git and svn input arguments:

#if SVN
void option_svn_url(const std::string & arg) {

  std::string::size_type atsign = arg.find('@');
  if(atsign == std::string::npos) {

    options.svn_url = arg;
    options.revision_one = SVN_INVALID_REVNUM;
    options.revision_two = SVN_INVALID_REVNUM;

  } else {

    options.svn_url = arg.substr(0, atsign);
    options.revision_one = std::stoi(arg.substr(atsign + 1));
    std::string::size_type dash = arg.find('-', atsign + 1);
    options.revision_two = std::stoi(arg.substr(dash + 1));

  }

}
#endif

#if GIT
void option_git_url(const std::string & arg) {

  std::string::size_type atsign = arg.find('@');
  options.git_url = arg.substr(0, atsign);
  std::string::size_type dash = arg.find('-', atsign + 1);
  options.git_revision_one = arg.substr(atsign + 1, dash - (atsign + 1));
  options.git_revision_two = arg.substr(dash + 1);

}
#endif

// options that are passed to the srcML archive object (options.archive):

enum srcml_bool_field { ARCHIVE };

// compiler wants a general template function for some reason
template<srcml_bool_field field>
void option_srcml_bool(int) {}

// specialized version of the general template function above
template<>
void option_srcml_bool<ARCHIVE>(int flagged_count) {

  if(flagged_count) {
    srcml_archive_disable_solitary_unit(options.archive);
  }

}

enum srcml_int_field { TABSTOP };

template<srcml_int_field field>
void option_srcml_int(const int & arg) {}

template<>
void option_srcml_int<TABSTOP>(const int & arg) {

  srcml_archive_set_tabstop(options.archive, arg);

}

enum srcml_string_field { SRC_ENCODING, XML_ENCODING, LANGUAGE, URL, SRC_VERSION, REGISTER_EXT, XMLNS };

template<srcml_string_field field>
void option_srcml_string(const std::string & arg) {}

template<>
void option_srcml_string<SRC_ENCODING>(const std::string & arg) {

  srcml_archive_set_src_encoding(options.archive, arg.c_str());

}

template<>
void option_srcml_string<XML_ENCODING>(const std::string & arg) {

  srcml_archive_set_xml_encoding(options.archive, arg.c_str());

}

template<>
void option_srcml_string<LANGUAGE>(const std::string & arg) {

  srcml_archive_set_language(options.archive, arg.c_str());

}

template<>
void option_srcml_string<URL>(const std::string & arg) {
  
  srcml_archive_set_url(options.archive, arg.c_str());

}

template<>
void option_srcml_string<SRC_VERSION>(const std::string & arg) {

  srcml_archive_set_version(options.archive, arg.c_str());

}

template<>
void option_srcml_string<REGISTER_EXT>(const std::string & arg) {

  std::string::size_type pos = arg.find('=');
  srcml_archive_register_file_extension(
    options.archive,
    arg.substr(0, pos).c_str(),
    arg.substr(pos + 1).c_str()
  );

}

// this function is called with a processed version of the xmlns arguments
// that look like "prefix=uri", or just "uri"
template<>
void option_srcml_string<XMLNS>(const std::string & arg) {

  std::string::size_type pos = arg.find('=');
  if(pos == std::string::npos) {
    srcml_archive_register_namespace(options.archive, "", arg.c_str());
  }
  else {
    srcml_archive_register_namespace(
      options.archive,
      arg.substr(0, pos).c_str(),
      arg.substr(pos + 1, std::string::npos).c_str()
    );
  }

}

// general template functions for srcDiff's bit-level flags

template<OPTION_TYPE flag>
void option_flag_enable(int flagged_count) {

  if(flagged_count > 0) options.flags |= flag;

}

template<OPTION_TYPE flag>
void option_flag_disable(int flagged_count) {

  if(flagged_count > 0) options.flags &= ~flag;

}

// general template functions for srcML's bit-level flags
template<int op>
void option_srcml_flag_enable(int flagged_count) {

  if(flagged_count > 0) srcml_archive_enable_option(options.archive, op);

}

// processes the argument to the srcdiff parsing method option
void option_parsing_method(const std::string & arg) {

  std::vector<std::string> methods;
  std::string::size_type last_pos = 0;
  std::string::size_type pos = 0;
  while((pos = arg.find(',', last_pos)) != std::string::npos) {

    methods.push_back(arg.substr(last_pos, pos));
    last_pos = pos + 1;

  }

  methods.push_back(arg.substr(last_pos));

  for(std::string method : methods) {

    if(method == NO_GROUP_DIFF_METHOD)   options.methods &= ~METHOD_GROUP;
    else if(method == GROUP_DIFF_METHOD) options.methods |= METHOD_GROUP;
    else {
      throw CLI::ValidationError(method + " is not a valid parsing method");
    }

  }

}

// this is a special option in the view_options_t struct that can be a string or
// a number via std::any
void view_option_unified_view_context(const std::string & arg) {

  try {

    options.view_options.unified_view_context = std::stoi(arg);

  } catch(std::invalid_argument &) {

    if (arg != "function" && arg != "all") {
      throw CLI::ValidationError(
        "The context for the unified view must be specified as \"all\", \"function\", or an integer number of lines."
      );
    }

    options.view_options.unified_view_context = arg;

  }

}

void view_option_side_by_side_tab_size(const int & arg) {

  options.view_options.side_by_side_tab_size = arg;

  options.flags |= OPTION_SIDE_BY_SIDE_VIEW;

}


// the main interface
const client_options& process_command_line(int argc, char* argv[]) {

  options.archive = srcml_archive_create();
  srcml_archive_enable_solitary_unit(options.archive);
  srcml_archive_disable_hash(options.archive);
  srcml_archive_register_namespace(options.archive,
      srcdiff::SRCDIFF_DEFAULT_NAMESPACE_PREFIX.c_str(),
      srcdiff::SRCDIFF_DEFAULT_NAMESPACE_HREF.c_str()
  );

  CLI::App cli(
    "Translates C, C++, and Java source code into the XML source-code representation srcDiff.\n"
    "Multiple files are stored in a srcDiff archive.",
    PROGRAM_NAME
  );

  cli.usage("USAGE: srcdiff [OPTIONS] <original_src_input modified_src_input>... [-o <srcDiff_outfile>]");
  
  cli.footer("Report bugs to " EMAIL_ADDRESS "\n");

  cli.get_formatter()->column_width(COLUMN_WIDTH);

  cli.add_option_function<std::vector<std::string>>(
    "input",
    option_input_file,
    "Pairs of input source files or directories, separated by spaces.\n"
    "Example: orig1.cpp mod1.cpp orig2.cpp mod2.cpp dir1 dir2 ...]\n"
    "srcDiff file with xml extension, requires --unified or --side-by-side"
  );

  CLI::Option_group * general_group = cli.add_option_group("General");

  general_group->set_version_flag("-V,--version", get_version);
  
  general_group->add_option(
      "-o,--output",
      options.srcdiff_filename,
      "Specify output filename"
    )->default_val("-");
  
  general_group->add_flag(
    "-v,--verbose",
    option_flag_enable<OPTION_VERBOSE>,
    "Verbose messaging"
  );
  
  general_group->add_flag(
    "-q,--quiet",
    option_flag_enable<OPTION_QUIET>,
    "Silence messaging"
  );

  CLI::Option_group * input_group = cli.add_option_group("Input");

  // currently, this takes priority over the positional file input argument if
  // it is present
  input_group->add_option_function<std::string>(
    "--files-from",
    option_files_from,
    "Set the input to be a list of file pairs from the specified file.\n"
    "There should be one pair per line, in the format: original|modified"
  );

  #if SVN
    input_group->add_option_function<std::string>(
      "--svn",
      option_svn_url,
      "Input from a Subversion repository. Example: --svn http://example.org@1-2"
    );

    input_group->add_flag(
      "--svn-continuous",
      option_flag_enable<OPTION_SVN_CONTINUOUS>,
      "Continue from base revision: Treat revisions supplied as as range and srcdiff each version with subsequent"
    ); // this may have been where needed revision
  #endif

  #if GIT
    input_group->add_option_function<std::string>(
      "--git",
      option_git_url,
      "Input from a Git repository. Example: --git http://example.org@HASH-HASH"
    );
  #endif

  CLI::Option_group * srcml_group = cli.add_option_group(
    "srcML",
    "These options control how srcML parses code into an XML AST."
  );

  srcml_group->add_flag_function(
    "-n,--archive",
    option_srcml_bool<ARCHIVE>,
    "Output srcDiff as an archive"
  )->force_callback(true);

  srcml_group->add_option_function<std::string>(
    "-t,--src-encoding",
    option_srcml_string<SRC_ENCODING>,
    "Set the input source encoding"
  )->default_val("ISO-8859-1")->force_callback(true);

  srcml_group->add_option_function<std::string>(
    "-x,--xml-encoding",
    option_srcml_string<XML_ENCODING>,
    "Set the output XML encoding"
  )->default_val("UTF-8")->force_callback(true);

  srcml_group->add_option_function<std::string>(
    "-l,--language",
    option_srcml_string<LANGUAGE>,
    "Set the input source programming language"
  )->default_val("C++");

  // Note: this will override the filename attribute on all output units
  srcml_group->add_option(
    "-f,--filename",
    options.unit_filename,
    "Specify a unit filename attribute that is different from the actual filename"
  );

  srcml_group->add_option_function<std::string>(
    "--register-ext",
     option_srcml_string<REGISTER_EXT>,
    "Register a file extension/language pair to be used during parsing\n"
    "Example: --register-ext cxx=C++"
  );

// TODO: should these attributes be added to the unit(s), or require --archive?
  srcml_group->add_option_function<std::string>(
    "--url",
    option_srcml_string<URL>,
    "Set the url attribute on the root XML element of the archive"
  );

  srcml_group->add_option_function<std::string>(
    "-s,--src-version",
    option_srcml_string<SRC_VERSION>,
    "Set the version attribute on the root XML element of the archive"
  );

  // since the XMLNS options have a format that CLI11 doesn't know how to parse,
  // they're actually pre-processed before the CLI11 parse function is called.
  // So, this option is always going to be unused from CLI11's point of view,
  // and is just added so it shows up in the help text.
  srcml_group->add_option_function<std::string>(
    "--xmlns",
    [](std::string){},  // no-op. should never be called
    "Set the prefix associated with a namespace or register a new one.\n"
    "Use the form --xmlns:prefix=url, or --xmlns=url to set the\ndefault prefix."
  );

  srcml_group->add_flag(
    "--position",
    option_srcml_flag_enable<SRCML_OPTION_POSITION>,
    "Output additional position information on the srcML elements"
  )->force_callback();

  srcml_group->add_option_function<int>(
    "--tabs",
    option_srcml_int<TABSTOP>,
    "Set the tabstop size"
  )->default_val(8)->force_callback();

  srcml_group->add_flag(
    "--no-xml-decl",
    option_srcml_flag_enable<SRCML_OPTION_NO_XML_DECL>,
    "Do not output the XML declaration"
  )->force_callback();

  srcml_group->add_flag(
    "--cpp-markup-if0",
    option_srcml_flag_enable<SRCML_OPTION_CPP_MARKUP_IF0>,
    "Markup #if 0 contents"
  )->force_callback();

  srcml_group->add_flag(
    "--cpp-text-else",
    option_srcml_flag_enable<SRCML_OPTION_CPP_TEXT_ELSE>,
    "Do not markup #else contents"
  )->force_callback();

  // TODO: remove these options, here and elsewhere in the code

  srcml_group->add_option_function<std::string>(
    "-m,--method",
    option_parsing_method,
    "Specify a list of parsing methods, separated by commas.\n"
    "The options are group-diff and no-group-diff"
  )->default_val("group-diff")->force_callback();

  // srcdiff_group->add_flag(
  //   "--disable-string-split",
  //   option_flag_disable<OPTION_STRING_SPLITTING>,
  //   "Disable splitting strings into multiple nodes"
  // );

  CLI::Option_group * view_options = cli.add_option_group("View");
  view_options->description(
    "These options configure the view produced by --unified or --side-by-side."
  );

  CLI::Option * unified = view_options->add_flag(
    "-u,--unified",
    option_flag_enable<OPTION_UNIFIED_VIEW>,
    "Output as a colorized unified diff with provided context"
  );

  // no forced callback to add the default value to the options for this one
  CLI::Option * side_by_side = view_options->add_option_function<int>(
    "-y,--side-by-side",
    view_option_side_by_side_tab_size,
    "Output as colorized side-by-side diff. Provide the tabstop size as\n"
    "the argument to this option."
  )->default_val(8)->excludes(unified);

  view_options->add_option_function<std::string>(
    "-C,--context",
    view_option_unified_view_context,
    "Specify the amount of context to show around an edit in the unified/side-by-side view.\n"
    "Either give a number of lines, or use \"all\" or -1 to see the entire file,\n"
    "or use \"function\" to see the encompassing function."
  )->default_val("3")->needs(unified);

  // TODO: document what a custom theme file should look like somewhere
  view_options->add_option(
    "--theme",
    options.view_options.theme,
    "Select theme for syntax highlighting.\n"
    "Options: \"default\", \"monokai\", or the filename of a custom theme."
  )->default_val("default");

  view_options->add_flag(
    "--html",
    option_flag_enable<OPTION_HTML_VIEW>,
    "Output the unified/side-by-side view as an HTML file instead of on the terminal"
  );

  // TODO: what is the difference between ignoring whitespace and ignoring *all*
  // whitespace?
  CLI::Option * ignore_space = view_options->add_flag(
    "-w,--ignore-space",
    option_flag_enable<OPTION_IGNORE_WHITESPACE>,
    "Ignore whitespace"
  );

  view_options->add_flag(
    "-W,--ignore-all-space",
    option_flag_enable<OPTION_IGNORE_ALL_WHITESPACE>,
    "Ignore all whitespace"
  )->excludes(ignore_space);

  view_options->add_flag(
    "-c,--ignore-comments",
    option_flag_enable<OPTION_IGNORE_COMMENTS>,
    "Ignore comments"
  );

  view_options->add_option(
    "--highlight",
    options.view_options.syntax_highlight,
    "Set the level of syntax highlighting.\n"
    "Options: none, partial (default), or full"
  )->default_val("partial");

  // our xmlns options have a custom format and need to be parsed in advance as
  // a special case

  std::vector<std::string> arguments;

  std::string xmlns_option = "--xmlns";
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.find(xmlns_option) == 0) {
      // if this is an xmlns argument, cut off the --xmlns= (or, if there is a
      // prefix, the --xmlns:) and pass to the option handler function
      option_srcml_string<XMLNS>(arg.substr(xmlns_option.length() + 1));
    } else {
      arguments.push_back(arg);
    }
  }

  // if you give CLI11 a vector of arguments, it expects it to be reversed 🤷
  std::reverse(arguments.begin(), arguments.end());
  
  try {

    cli.parse(arguments);

    if (!options.files_from_name.has_value() && options.input_pairs.size() < 1) {
      throw CLI::ValidationError("Input files are required.");
    }

    // CLI11 unfortunately does not have a great mechanism for requiring that
    // exactly one out of two options is required if and only if any options
    // from a certain set are present. see discussion here:
    // https://github.com/CLIUtils/CLI11/issues/88. This is a simple workaround
    // for one case
    for (const auto& view_option : view_options->get_options()) {
      if (view_option != unified && view_option != side_by_side) {
        if (!view_option->empty() && (side_by_side->empty() && unified->empty())) {
          throw CLI::ValidationError(
            view_option->get_name(false, true) +
            " requires either --unified or --side-by-side to be set."
          );
        }
      }
    }

    if(   is_option(options.flags, OPTION_VIEW_XML)
      && !is_option(options.flags, OPTION_UNIFIED_VIEW) 
      && !is_option(options.flags, OPTION_SIDE_BY_SIDE_VIEW)) {
          throw CLI::ValidationError("XML input requires either --unified or --side-by-side to be set."
          );

    }

  } catch (const CLI::ParseError &e) {
    // parsing error
    exit(cli.exit(e));
  }

  // on success, we return our options object

  return options;

}

}
