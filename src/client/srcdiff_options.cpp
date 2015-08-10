#include <srcdiff_options.hpp>

#include <srcdiff_constants.hpp>

#include <boost/program_options.hpp>
#include <libxml/parser.h>

#include <iostream>

srcdiff_options options;

const unsigned LINE_LENGTH = 150;

boost::program_options::options_description general("General Options", LINE_LENGTH);
boost::program_options::options_description input_file_op("Input file Option", LINE_LENGTH);
boost::program_options::options_description input_ops("Input Options", LINE_LENGTH);
boost::program_options::options_description srcml_ops("srcml Options", LINE_LENGTH);
boost::program_options::options_description srcdiff_ops("srcdiff Options", LINE_LENGTH);
boost::program_options::options_description all("All options");

boost::program_options::positional_options_description input_file;

#define PROGRAM_NAME "srcdiff"
#define EMAIL_ADDRESS "mdecker6@kent.edu"
const char * const SRCDIFF_HELP_HEADER =
"Usage: " PROGRAM_NAME " [options] <original_src_infile modified_src_infile>... [-o <srcDiff_outfile>]\n\n"

"Translates C, C++, and Java source code into the XML source-code representation srcDiff.\n"
"Multiple files are stored in a srcDiff archive.\n\n";

const char * const SRCDIFF_HELP_FOOTER =
"Report bugs to " EMAIL_ADDRESS "\n";


void option_help(const bool & on) {

  if(!on) return;

  std::cout << SRCDIFF_HELP_HEADER;
  std::cout << general     << '\n';
  std::cout << input_ops   << '\n';
  std::cout << srcml_ops   << '\n';
  std::cout << srcdiff_ops << '\n';
  std::cout << SRCDIFF_HELP_FOOTER;

  exit(0);

}

void option_version(const bool & on) {

  if(!on) return;

  printf("%s Version 1\n", PROGRAM_NAME);

  printf("Using:\n");
  printf("libsrcml %s\n", srcml_version_string());

  if(atoi(xmlParserVersion) == LIBXML_VERSION)
    printf("libxml %d\n", LIBXML_VERSION);
  else
    printf("libxml %s (Compiled %d)\n", xmlParserVersion, LIBXML_VERSION);

  exit(0);

}

void option_input_file(const std::vector<std::string> & arg) {

  if(arg.size() % 2 != 0) {

    std::cout << "Odd number of input files.\n";
    exit(1);

  }

  options.input_pairs.reserve(arg.size() / 2);

  for(std::vector<std::string>::size_type pos = 0; pos + 1 < arg.size(); pos += 2)
    options.input_pairs.push_back(std::make_pair(arg[pos], arg[pos + 1]));

  if(options.input_pairs.size() > 1) srcml_archive_enable_full_archive(options.archive);

}

template<std::string srcdiff_options::*field>
void option_field(const std::string & arg) { options.*field = arg; }

template<boost::optional<std::string> srcdiff_options::*field>
void option_field(const std::string & arg) { options.*field = arg; }

#ifndef _MSC_BUILD
template<>
void option_field<&srcdiff_options::files_from_name>(const std::string & arg) {

    options.files_from_name = arg;
    srcml_archive_enable_full_archive(options.archive);

}

template<>
void option_field<&srcdiff_options::summary_type_str>(const std::string & arg) {

  options.summary_type_str = arg;

  options.flags |= OPTION_SUMMARY;

}

#endif

#if SVN
template<>
void option_field<&srcdiff_options::svn_url>(const std::string & arg) {

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
template<>
void option_field<&srcdiff_options::git_url>(const std::string & arg) {

  std::string::size_type atsign = arg.find('@');
  options.git_url = arg.substr(0, atsign);
  std::string::size_type dash = arg.find('-', atsign + 1);
  options.git_revision_one = arg.substr(atsign + 1, dash - (atsign + 1));
  options.git_revision_two = arg.substr(dash + 1);
  srcml_archive_enable_full_archive(options.archive);

}
#endif

template<boost::any srcdiff_options::*field>
void option_field(const std::string & arg) { options.*field = arg; }

#ifndef _MSC_BUILD
template<>
void option_field<&srcdiff_options::bash_view_context>(const std::string & arg) {

  try {

    options.bash_view_context = (size_t)std::stoll(arg);

  } catch(std::invalid_argument) {

    options.bash_view_context = arg;

  }

  options.flags |= OPTION_BASH_VIEW;

}

#endif

enum srcml_bool_field { ARCHIVE };

template<srcml_bool_field field>
void option_srcml_field(bool on) {}

template<>
void option_srcml_field<ARCHIVE>(bool on) {

  if(on)
    srcml_archive_enable_full_archive(options.archive);
  else
    srcml_archive_disable_full_archive(options.archive);

}

enum srcml_int_field { TABSTOP };

template<srcml_int_field field>
void option_srcml_field(const int & arg) {}

template<>
void option_srcml_field<TABSTOP>(const int & arg) {

  srcml_archive_set_tabstop(options.archive, arg);

}

enum srcml_string_field { SRC_ENCODING, XML_ENCODING, LANGUAGE, URL, SRC_VERSION, REGISTER_EXT, XMLNS };

template<srcml_string_field field>
void option_srcml_field(const std::string & arg) {}

template<>
void option_srcml_field<SRC_ENCODING>(const std::string & arg) {

  srcml_archive_set_src_encoding(options.archive, arg.c_str());

}

template<>
void option_srcml_field<XML_ENCODING>(const std::string & arg) {

  srcml_archive_set_xml_encoding(options.archive, arg.c_str());

}

template<>
void option_srcml_field<LANGUAGE>(const std::string & arg) {

  srcml_archive_set_language(options.archive, arg.c_str());

}

template<>
void option_srcml_field<URL>(const std::string & arg) {

  srcml_archive_set_url(options.archive, arg.c_str());

}

template<>
void option_srcml_field<SRC_VERSION>(const std::string & arg) {

  srcml_archive_set_version(options.archive, arg.c_str());

}

template<>
void option_srcml_field<REGISTER_EXT>(const std::string & arg) {

  std::string::size_type pos = arg.find('=');
  srcml_archive_register_file_extension(options.archive, arg.substr(0, pos).c_str(), arg.substr(pos + 1, std::string::npos).c_str());

}


template<>
void option_srcml_field<XMLNS>(const std::string & arg) {

  std::string::size_type pos = arg.find('=');
  if(pos == std::string::npos)
    srcml_archive_register_namespace(options.archive, "", arg.c_str());
  else
    srcml_archive_register_namespace(options.archive, arg.substr(0, pos).c_str(), arg.substr(pos + 1, std::string::npos).c_str());

}

template<OPTION_TYPE flag>
void option_flag_enable(bool on) {

  if(on) options.flags |= flag;

}

template<OPTION_TYPE flag>
void option_flag_disable(bool on) {

  if(on) options.flags &= ~flag;

}

template<int op>
void option_srcml_flag_enable(bool on) {

  if(on) srcml_archive_enable_option(options.archive, op);

}

template<int op>
void option_srcml_flag_disable(bool on) {

  if(on) srcml_archive_disable_option(options.archive, op);

}

void option_method(const std::string & arg) {

  std::vector<std::string> methods;
  std::string::size_type last_pos = 0;
  std::string::size_type pos = 0;
  while((pos = arg.find(',', last_pos)) != std::string::npos) {

    methods.push_back(arg.substr(last_pos, pos));
    last_pos = pos + 1;

  }

  methods.push_back(arg.substr(last_pos));

  for(std::string method : methods) {

    if(method == COLLECT_METHOD) options.methods &= ~METHOD_RAW;
    else if(method == RAW_METHOD) options.methods |= METHOD_RAW;
    else if(method == NO_GROUP_DIFF_METHOD) options.methods &= ~METHOD_GROUP;
    else if(method == GROUP_DIFF_METHOD) options.methods |= METHOD_GROUP;
    else {

        fprintf(stderr, "Invalid argument to --method: '%s'\n", method.c_str());
        exit(/*STATUS_INVALID_ARGUMENT*/1);    
    }

  }

}

std::pair<std::string, std::string> parse_xmlns(const std::string & arg) {

  if(arg.find("xmlns:") != std::string::npos) {

    std::string::size_type pos = arg.find(':');
    std::string name, value;
    if(pos == std::string::npos)
      name = arg.substr(2);
    else {

      name = arg.substr(2, pos - 2);
      value = arg.substr(pos + 1);

    }

    return std::make_pair(name, value);

  }

  return std::make_pair(std::string(), std::string());

}

const srcdiff_options & process_command_line(int argc, char* argv[]) {

  options.archive = srcml_archive_create();
  srcml_archive_disable_full_archive(options.archive);
  srcml_archive_disable_hash(options.archive);
  srcml_archive_enable_option(options.archive, SRCML_OPTION_XML_DECL);
  srcml_archive_register_namespace(options.archive, "diff", SRCDIFF_DEFAULT_NAMESPACE_HREF.c_str());

  general.add_options()
    ("help,h", boost::program_options::bool_switch()->notifier(&option_help), "Output srcdiff help message")
    ("version,V", boost::program_options::bool_switch()->notifier(&option_version), "Output srcdiff version")
    ("output,o", boost::program_options::value<std::string>()->notifier(option_field<&srcdiff_options::srcdiff_filename>)->default_value("-"), "Specify output filename")
    ("compress,z", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_COMPRESS>), "Compress the output")
    ("verbose,v", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_VERBOSE>), "Verbose messaging")
    ("quiet,q", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_QUIET>), "Silence messaging")
  ;

  input_file_op.add_options()
    ("input", boost::program_options::value<std::vector<std::string>>()->notifier(option_input_file), "Set the input to be a list of file pairs from the provided file")
  ;

#ifndef _MSC_BUILD
  input_ops.add_options()
    ("files-from", boost::program_options::value<std::string>()->notifier(option_field<&srcdiff_options::files_from_name>), "Set the input to be a list of file pairs from the provided file.  Pairs are of the format: original|modified")
    #endif

#if SVN
    ("svn", boost::program_options::value<std::string>()->notifier(option_field<&srcdiff_options::svn_url>), "Input from a Subversion repository. Example: --svn http://example.org@1-2")
    ("svn-continuous", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_SVN_CONTINUOUS>), "Continue from base revision: Treat revisions supplied as as range and srcdiff each version with subsequent") // this may have been where needed revision
#endif

#if GIT
    ("git", boost::program_options::value<std::string>()->notifier(option_field<&srcdiff_options::git_url>), "Input from a Subversion repository. Example: --git http://example.org@HASH-HASH")
#endif

  ;

  srcml_ops.add_options()
    ("archive,n", boost::program_options::bool_switch()->notifier(option_srcml_field<ARCHIVE>), "Output srcDiff as an archive")

#ifndef _MSC_BUILD
    ("src-encoding,t", boost::program_options::value<std::string>()->notifier(option_srcml_field<SRC_ENCODING>)->default_value("ISO-8859-1"), "Set the input source encoding")
#endif

    ("xml-encoding,x", boost::program_options::value<std::string>()->notifier(option_srcml_field<XML_ENCODING>)->default_value("UTF-8"), "Set the output XML encoding") // may want this to be encoding instead of xml-encoding
    ("language,l", boost::program_options::value<std::string>()->notifier(option_srcml_field<LANGUAGE>)->default_value("C++"), "Set the input source programming language")
    ("filename,f", boost::program_options::value<std::string>()->notifier(option_field<&srcdiff_options::unit_filename>), "Override unit filename")
    ("register-ext", boost::program_options::value<std::string>()->notifier(option_srcml_field<REGISTER_EXT>), "Register an extension to language pair to be used during parsing")
    ("url,u", boost::program_options::value<std::string>()->notifier(option_srcml_field<URL>), "Set the root url attribute")
    ("src-version,s", boost::program_options::value<std::string>()->notifier(option_srcml_field<SRC_VERSION>), "Set the root version attribute")
    ("xmlns", boost::program_options::value<std::string>()->notifier(option_srcml_field<XMLNS>), "Set the prefix associationed with a namespace or register a new one. of the form --xmlns:prefix=url or --xmlns=url for default prefix.")
    ("position", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_POSITION>), "Output additional position information on the srcML elements")

#ifndef _MSC_BUILD
    ("tabs", boost::program_options::value<int>()->notifier(option_srcml_field<TABSTOP>)->default_value(8), "Set the tabstop size")
#endif

    ("no-xml-decl", boost::program_options::bool_switch()->notifier(option_srcml_flag_disable<SRCML_OPTION_XML_DECL>), "Do not output the xml declaration")
    ("cpp-markup-else", boost::program_options::bool_switch()->notifier(option_srcml_flag_disable<SRCML_OPTION_CPP_TEXT_ELSE>), "Markup up #else contents (default)")
    ("cpp-text-else", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_CPP_TEXT_ELSE>), "Do not markup #else contents")
    ("cpp-markup-if0", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_CPP_MARKUP_IF0>), "Markup up #if 0 contents")
    ("cpp-text-if0", boost::program_options::bool_switch()->notifier(option_srcml_flag_disable<SRCML_OPTION_CPP_MARKUP_IF0>), "Do not markup #if 0 contents (default)")
  ;

  srcdiff_ops.add_options()
    ("method,m",  boost::program_options::value<std::string>()->notifier(option_method)->default_value("collect,group-diff"), "Set srcdiff parsing method")
    ("visualization", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_VISUALIZE>), "Output a visualization instead of xml")
    ("same", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_SAME>), "Output files that are the same")
    ("pure", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_PURE>)->default_value(true), "Output files that are added/deleted (default)")
    ("change", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_CHANGE>)->default_value(true), "Output files that where changed (default). Used only with visualization option")
    ("no-same", boost::program_options::bool_switch()->notifier(option_flag_disable<OPTION_SAME>)->default_value(true), "Do not output files that are the same (default)")
    ("no-pure", boost::program_options::bool_switch()->notifier(option_flag_disable<OPTION_PURE>), "Do not ouptut files that are added/deleted")
    ("srcdiff-only", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_SRCDIFFONLY>), "Output files that only srcdiff, but not diff says are changed")
    ("diff-only", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_DIFFONLY>), "Output files that only diff, but not srcdiff says are changed")

    ("burst", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_BURST>), "Output each input file to a single srcDiff document.  -o gives output directory")
    ("srcml", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_SRCML>), "Also, output the original and modified srcML of each file when burst enabled")

#ifndef _MSC_BUILD
    ("bash", boost::program_options::value<std::string>()->implicit_value("3")->notifier(option_field<&srcdiff_options::bash_view_context>),
        "Output as colorized bash text with provided context. Number is lines of context, 'all' or -1 for entire file, 'function' for encompasing function (default = 3)")
    ("summary", boost::program_options::value<std::string>()->implicit_value("text")->notifier(option_field<&srcdiff_options::summary_type_str>),
        "Output a summary of the differences.  Options 'text' and/or 'table' summary.   Default 'text'  ")
#endif

  ;

  input_file.add("input", -1);
  all.add(general).add(input_file_op).add(input_ops).add(srcml_ops).add(srcdiff_ops);

  try {

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(all).positional(input_file).extra_parser(parse_xmlns).run(), var_map);
    boost::program_options::notify(var_map);

  } catch(boost::program_options::error e) {

    std::cerr << "Exception: " << e.what() << '\n';
    exit(1);

  }

  return options;

}
