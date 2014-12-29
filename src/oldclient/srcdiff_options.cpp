#include <srcdiff_options.hpp>

#include <boost/program_options.hpp>
#include <libxml/parser.h>

#include <iostream>

srcdiff_options options;

boost::program_options::options_description general("General Options");
boost::program_options::options_description input_ops("Input Options");
boost::program_options::options_description srcml_ops("srcml Options");
boost::program_options::options_description srcdiff_ops("srcdiff Options");
boost::program_options::options_description all("All options");

boost::program_options::positional_options_description input_file;

#define PROGRAM_NAME "srcdiff"
#define EMAIL_ADDRESS "mdecker6@kent.edu"
const char * const SRCDIFF_HELP_HEADER =
"Usage: " PROGRAM_NAME " [options] <old_src_infile new_src_infile>... [-o <srcDiff_outfile>]\n\n"

"Translates C, C++, and Java source code into the XML source-code representation srcDiff.\n"
"Multiple files are stored in a srcDiff archive.\n\n";

const char * const SRCDIFF_HELP_FOOTER =
"Report bugs to " EMAIL_ADDRESS "\n";


void option_help(const bool & on) {

  if(!on) return;

  std::cout << SRCDIFF_HELP_HEADER;
  std::cout << general << '\n';
  std::cout << input_ops<< '\n';
  std::cout << srcml_ops << '\n';
  std::cout << srcdiff_ops << '\n';
  std::cout << SRCDIFF_HELP_FOOTER;

  exit(0);

}

void option_version(const bool & on) {

  if(!on) return;

  printf("%s Version 1\n", PROGRAM_NAME);

  printf("Using: libsrcml %s\n", srcml_version_string());

  printf("Using: ");
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
    options.input_pairs.push_back(std::pair<std::string, std::string>(arg[pos], arg[pos + 1]));

  if(options.input_pairs.size() > 1) srcml_archive_enable_option(options.archive, SRCML_OPTION_ARCHIVE);

}

template<boost::optional<std::string> srcdiff_options::*field>
void option_field(const std::string & arg) { options.*field = arg; }

template<int srcdiff_options::*field>
void option_field(const int & arg) { options.*field = arg; }

template<>
void option_field<&srcdiff_options::number_context_lines>(const int & arg) {

  options.number_context_lines = arg;
  options.flags |= OPTION_BASH_VIEW;

}

enum srcml_int_field { TABSTOP };

template<srcml_int_field field>
void option_srcml_field(const int & arg) {}

template<>
void option_srcml_field<TABSTOP>(const int & arg) {

  srcml_archive_set_tabstop(options.archive, arg);

}

enum srcml_string_field { SRC_ENCODING, XML_ENCODING, LANGUAGE, DIRECTORY, FILENAME, SRC_VERSION, REGISTER_EXT, XMLNS_DEFAULT, XMLNS };

template<srcml_string_field field>
void option_srcml_field(const std::string & arg) {}

template<>
void option_srcml_field<SRC_ENCODING>(const std::string & arg) {

  srcml_archive_set_src_encoding(options.archive, arg.c_str());

}

template<>
void option_srcml_field<XML_ENCODING>(const std::string & arg) {

  srcml_archive_set_encoding(options.archive, arg.c_str());

}

template<>
void option_srcml_field<LANGUAGE>(const std::string & arg) {

  srcml_archive_set_language(options.archive, arg.c_str());

}

template<>
void option_srcml_field<DIRECTORY>(const std::string & arg) {

  srcml_archive_set_directory(options.archive, arg.c_str());

}

template<>
void option_srcml_field<FILENAME>(const std::string & arg) {

  srcml_archive_set_filename(options.archive, arg.c_str());

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
void option_srcml_field<XMLNS_DEFAULT>(const std::string & arg) {

  srcml_archive_register_namespace(options.archive, "", arg.c_str());

}

template<>
void option_srcml_field<XMLNS>(const std::string & arg) {

  std::string::size_type pos = arg.find('=');
  srcml_archive_register_namespace(options.archive, arg.substr(0, pos).c_str(), arg.substr(pos + 1, std::string::npos).c_str());

}

template<OPTION_TYPE flag>
void option_flag_enable(bool on) {

  if(on) options.flags |= flag;

}

template<OPTION_TYPE flag>
void option_flag_disable(bool on) {

  if(on) options.flags |= flag;

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

  if(arg == COLLECT_METHOD) options.methods &= ~METHOD_RAW;
  else if(arg == RAW_METHOD) options.methods |= METHOD_RAW;
  else if(arg == NO_GROUP_DIFF_METHOD) options.methods &= ~METHOD_GROUP;
  else if(arg == GROUP_DIFF_METHOD) options.methods |= METHOD_GROUP;
  else {

      fprintf(stderr, "Invalid argument to --method: '%s'\n", arg.c_str());
      exit(/*STATUS_INVALID_ARGUMENT*/1);    
  }

}

srcdiff_options process_command_line(int argc, char* argv[]) {

  options.archive = srcml_create_archive();
  srcml_archive_disable_option(options.archive, SRCML_OPTION_ARCHIVE);
  srcml_archive_enable_option(options.archive, SRCML_OPTION_NAMESPACE_DECL | SRCML_OPTION_XML_DECL | SRCML_OPTION_HASH | SRCML_OPTION_TERNARY);
  srcml_archive_register_namespace(options.archive, "diff", "http://www.sdml.info/srcDiff");

  general.add_options()
    ("help,h", boost::program_options::bool_switch()->notifier(&option_help), "Output srcdiff help message")
    ("version,V", boost::program_options::bool_switch()->notifier(&option_version), "Output srcdiff version")
    ("output,o", boost::program_options::value<std::string>()->notifier(option_field<&srcdiff_options::srcdiff_filename>)->default_value("-"), "Specify output filename")
    ("compress,z", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_COMPRESS>), "Compress the output")
    ("verbose,v", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_VERBOSE>), "Verbose messaging")
    ("quiet,q", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_QUIET>), "Silence messaging")
  ;

  input_ops.add_options()
    ("input", boost::program_options::value<std::vector<std::string>>()->notifier(option_input_file), "Set the input to be a list of file pairs from the provided file")
    ("files-from", boost::program_options::value<std::string>()->notifier(option_field<&srcdiff_options::files_from_name>), "Set the input to be a list of file pairs from the provided file")
    ("svn", boost::program_options::value<std::string>()->notifier(option_field<&srcdiff_options::svn_url>), "Input from a Subversion repository")
    ("svn-continuous", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_SVN_CONTINUOUS>), "Continue from base revision") // this may have been where needed revision
  ;

  srcml_ops.add_options()
    ("archive,n", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_ARCHIVE>), "Output srcDiff as an archive")
    ("src-encoding,t", boost::program_options::value<std::string>()->notifier(option_srcml_field<SRC_ENCODING>)->default_value("ISO-8859-1"), "Set the input source encoding")
    ("xml-encoding,x", boost::program_options::value<std::string>()->notifier(option_srcml_field<XML_ENCODING>)->default_value("UTF-8"), "Set the output XML encoding") // may want this to be encoding instead of xml-encoding
    ("language,l", boost::program_options::value<std::string>()->notifier(option_srcml_field<LANGUAGE>)->default_value("C++"), "Set the input programming source language")
    ("register-ext", boost::program_options::value<std::string>()->notifier(option_srcml_field<REGISTER_EXT>), "Register an extension to language pair to be used during parsing")
    ("directory,d", boost::program_options::value<std::string>()->notifier(option_srcml_field<DIRECTORY>), "Set the root directory attribute")
    ("filename,f", boost::program_options::value<std::string>()->notifier(option_srcml_field<FILENAME>), "Set the root filename attribute")
    ("src-version,s", boost::program_options::value<std::string>()->notifier(option_srcml_field<SRC_VERSION>), "Set the root version attribute")
    ("xmlns", boost::program_options::value<std::string>()->notifier(option_srcml_field<XMLNS_DEFAULT>), "Set default namespace")
    ("xmlns:", boost::program_options::value<std::string>()->notifier(option_srcml_field<XMLNS>), "Set namesapce for given prefix or create a new one")
    ("position", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_POSITION>), "Output additional position information on the srcML elements")
    ("tabs", boost::program_options::value<int>()->notifier(option_srcml_field<TABSTOP>)->default_value(8), "Tabstop size")
    ("no-xml-decl", boost::program_options::bool_switch()->notifier(option_srcml_flag_disable<SRCML_OPTION_NAMESPACE_DECL>), "Do not output the xml declaration")
    ("no-namespace-decl", boost::program_options::bool_switch()->notifier(option_srcml_flag_disable<SRCML_OPTION_XML_DECL>), "Do not output any namespace declarations")
    ("cpp-markup-else", boost::program_options::bool_switch()->notifier(option_srcml_flag_disable<SRCML_OPTION_CPP_TEXT_ELSE>), "Markup up #else contents")
    ("cpp-text-else", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_CPP_TEXT_ELSE>), "Do not markup #else contents")
    ("cpp-markup-if0", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_CPP_MARKUP_IF0>), "Markup up #if 0 contents")
    ("cpp-text-if0", boost::program_options::bool_switch()->notifier(option_srcml_flag_disable<SRCML_OPTION_CPP_MARKUP_IF0>), "Do not markup #if 0 contents")
  ;

  srcdiff_ops.add_options()
    ("method,m",  boost::program_options::value<std::string>()->notifier(option_method), "Set srcdiff parsing method")
    ("recursive", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_RECURSIVE>), "I need to double check this one, but maybe recursive svn read")
    ("visualization", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_VISUALIZE>), "Output a visualization instead of xml")
    ("same", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_SAME>), "Output files that are the same")
    ("pure", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_PURE>), "Output files that are the purely added/deleted")
    ("change", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_CHANGE>), "Output files that where changed")
    ("no-same", boost::program_options::bool_switch()->notifier(option_flag_disable<OPTION_SAME>), "Do not output files that are the same")
    ("no-pure", boost::program_options::bool_switch()->notifier(option_flag_disable<OPTION_PURE>), "Do not ouptut files that are purely added/deleted")
    ("srcdiff-only", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_SRCDIFFONLY>), "Output files that only srcdiff, but not diff says are changed")
    ("diff-only", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_DIFFONLY>), "Output files that only diff, but not srcdiff says are changed")
    ("bash", boost::program_options::value<int>()->implicit_value(3)->notifier(option_field<&srcdiff_options::number_context_lines>), "Output as colorized bash text")
  ;

  input_file.add("input", -1);
  all.add(general).add(input_ops).add(srcml_ops).add(srcdiff_ops);

  try {

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(all).positional(input_file).run(), var_map);
    boost::program_options::notify(var_map);

  } catch(boost::program_options::error e) {

    std::cerr << "Exception: " << e.what() << '\n';
    exit(1);

  }

  return options;

}
