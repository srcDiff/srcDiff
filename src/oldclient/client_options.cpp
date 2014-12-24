#include <getopt.h>
#include <cerrno>

#include <srcml.h>

#include <client_options.hpp>
#include <srcmlapps.hpp>
#include <srcmlns.hpp>
#include <Options.hpp>
//#include <archive.h>

#include <cstring>

#include <cstdlib>
#include <libxml/parser.h>
//#include <libxml_archive_read.hpp>
//#include <libxml_archive_write.hpp>

#include <srcdiff_options.hpp>

#include <boost/program_options.hpp>

srcdiff_options options;

boost::program_options::options_description general("General options");
boost::program_options::options_description input_ops("Input options");
boost::program_options::options_description srcml_ops("srcML options");
boost::program_options::options_description srcdiff_ops("srcDiff options");

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

enum srcml_string_field { SRC_ENCODING, XML_ENCODING, LANGUAGE, DIRECTORY, FILENAME, SRC_VERSION, REGISTER_EXT };

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
      exit(STATUS_INVALID_ARGUMENT);    
  }

}

srcdiff_options process_cmdline(int argc, char* argv[]) {

  options.archive = srcml_create_archive();


  boost::program_options::options_description cmdline("srcdiff command-line options");
  general.add_options()
    ("help,h", "Output srcdiff help message")
    ("version,V", "Output srcdiff version")
    ("output,o", boost::program_options::value<std::string>()->notifier(option_field<&srcdiff_options::srcdiff_filename>)->default_value("-"), "Specify output filename")
    ("compress,z", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_COMPRESS>), "Compress the output")
    ("verbose,v", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_VERBOSE>), "Verbose messaging")
    ("quiet,q", boost::program_options::bool_switch()->notifier(option_flag_enable<OPTION_QUIET>), "Silence messaging")
  ;

  input_ops.add_options()
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
    ("xmlns", "Set default namespace")
    ("xmlns:", "Set namesapce for given prefix or create a new one")
    ("position", boost::program_options::bool_switch()->notifier(option_srcml_flag_enable<SRCML_OPTION_POSITION>), "Output additional position information on the srcML elements")
    ("tabs", boost::program_options::value<int>()->notifier(option_srcml_field<TABSTOP>)->default_value(3), "Tabstop size")
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
    ("bash", boost::program_options::value<int>()->notifier(option_field<&srcdiff_options::number_context_lines>)->default_value(3), "Output as colorized bash text")
  ;

  return options;

}

#define PROGRAM_NAME "srcdiff"
#if 0
using namespace LanguageName;

// setup options and collect info from arguments
int process_args(int argc, char* argv[], srcdiff_options & soptions, OPTION_TYPE & options, const char * urisprefix[]) {

  bool cpp_if0 = false;
  bool cpp_else = false;

  // process all command line options
  while (1) {
    int lastoptind = optind;
    curoption = 0;
    int option_index = 0;
    bool special = optind < argc && !strncmp(argv[optind], "--xmlns:", 8);
    opterr = !special ? 1 : 0;
    int c = getopt_long(argc, argv, "hVo:nex:t:zcgvl:d:f:s:q", cliargs, &option_index);

    if (c == -1)
      break;

    if (curoption) {

      if(curoption & (SRCML_OPTION_POSITION | SRCML_OPTION_LITERAL | SRCML_OPTION_OPERATOR | SRCML_OPTION_MODIFIER))
        srcml_archive_enable_option(soptions.archive, curoption);
      else
        options |= curoption;

      continue;

    }

    // treat --xmlns:prefix=url as --xmlns=url for processing
    if (special && c == '?') {
      c = XMLNS_FLAG_CODE;
    }

    // missing or extra option argument
    if (c == '?') {
      fprintf(stderr, "Try '%s --%s' for more information.\n", PROGRAM_NAME, HELP_FLAG);
      exit(option_error_status(optopt));
    }

    switch(c) {

    case HELP_FLAG_SHORT:
      output_help(PROGRAM_NAME);
      exit(STATUS_SUCCESS);

      break;

    case VERSION_FLAG_SHORT:
      output_version(PROGRAM_NAME);
      exit(STATUS_SUCCESS);
      break;

    case OUTPUT_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      soptions.srcdiff_filename = optarg;
      break;

    case FILELIST_FLAG_CODE:

      // check for missing argument confused by an argument that looks like an option
      //      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      options |= OPTION_FILELIST;

      // filelist mode is default nested mode
      if(!isoption(options, OPTION_VISUALIZE))
        srcml_archive_enable_option(soptions.archive,SRCML_OPTION_ARCHIVE);

      soptions.file_list_name = optarg;
  
      break;

    case VERBOSE_FLAG_SHORT :

        options |= OPTION_VERBOSE;

      break;

#ifdef SVN
    case SVN_FLAG_CODE:

      // check for missing argument confused by an argument that looks like an option
      //      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);
      {

        options |= OPTION_SVN;

        // filelist mode is default nested mode
        if(!isoption(options, OPTION_VISUALIZE))
          srcml_archive_enable_option(soptions.archive,SRCML_OPTION_ARCHIVE);
  
        soptions.svn_url = optarg;

        const char * end = index(optarg, '@');
        if(end) {

          soptions.svn_url = strndup(optarg, end - optarg);

          const char * first = index(end + 1, '-');
          const char * temp_revision = strndup(end + 1, first - (end + 1));
          soptions.revision_one = atoi(temp_revision);
          free((void *)temp_revision);

          if(first)
            soptions.revision_two = atoi(first + 1);

        }

      }

      break;

    case SVN_CONTINUOUS_FLAG_CODE:

      options |= OPTION_SVN_CONTINUOUS;

      break;
#endif

    case BASH_VIEW_FLAG_CODE:

      options |= OPTION_BASH_VIEW;

      if(optarg)
        soptions.number_context_lines = atoi(optarg);

      break;

    case REGISTER_EXT_FLAG_CODE:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      // check all the registered extensions.  There may be many (CSV)
      for (char* cursub = strtok(optarg, ","); cursub; cursub = strtok(NULL, ",")) {
        char* language = cursub;
        const char* extension = strsep(&language, "=");

        if(language == NULL) {

          fprintf(stderr, "Incomplete argument: %s\n", optarg);
          fprintf(stderr, "Try '%s %s' for more information.\n", PROGRAM_NAME, HELP_FLAG);
          exit(STATUS_ERROR);
        }

        srcml_archive_register_file_extension(soptions.archive, extension, language);
/*
        if (!Language::registerUserExt(extension, language)) {

          fprintf(stderr, "%s: language \"%s\" is not supported.\n", PROGRAM_NAME, language);
          fprintf(stderr, "Try '%s %s' for more information.\n", PROGRAM_NAME, HELP_FLAG);
          exit(STATUS_ERROR);
        }
*/
      }

      break;

    case NESTED_FLAG_SHORT:
      options |= SRCML_OPTION_ARCHIVE;
      break;

    case EXPRESSION_MODE_FLAG_SHORT:
      options |= OPTION_EXPRESSION;
      break;

    case ENCODING_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      srcml_archive_set_encoding(soptions.archive, optarg);

      break;

    case SRC_ENCODING_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      srcml_archive_set_src_encoding(soptions.archive, optarg);

      // validate source encoding

      break;

    case XMLNS_FLAG_CODE:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      {
        // find the start of the embedded uri (if it is in there)
        char* ns_uri = argv[optind - 1];
        strsep(&ns_uri, "=");

        // now find the prefix in what is left
        char* ns_prefix = argv[optind - 1];
        strsep(&ns_prefix, ":");

        // if no uri, look in the next argument
        if (!ns_uri[0]) {
          if (!(optind < argc && argv[optind][0] != '-')) {
            fprintf(stderr, "%s: xmlns option selected but not specified.\n", PROGRAM_NAME);
            //exit(STATUS_LANGUAGE_MISSING);

      	    exit(1);

          }

          ns_uri = argv[optind++];
        }

        srcml_archive_register_namespace(soptions.archive, ns_prefix, ns_uri);

      }
      break;

    case RECURSIVE_FLAG_CODE:
      options |= OPTION_RECURSIVE;
      break;

    case REVISION_FLAG_CODE:
      options |= OPTION_REVISION;
      break;

    case METHOD_FLAG_CODE:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      process_method(optarg, soptions);

      break;

    case VISUALIZE_FLAG_CODE:

      soptions.css_url = "";
      if(optarg != NULL)
        soptions.css_url = optarg;

      options |= OPTION_VISUALIZE;

      srcml_archive_disable_option(soptions.archive,SRCML_OPTION_ARCHIVE);

      break;

    case SAME_FLAG_CODE:

      options |= OPTION_OUTPUTSAME;

      break;

    case PURE_FLAG_CODE:

      options |= OPTION_OUTPUTPURE;

      break;

    case CHANGE_FLAG_CODE:

      options |= OPTION_CHANGE;
      options &= ~OPTION_OUTPUTSAME;
      options &= ~OPTION_OUTPUTPURE;

      break;

    case SRCDIFFONLY_FLAG_CODE:

      options |= OPTION_SRCDIFFONLY;

      break;

    case DIFFONLY_FLAG_CODE:

      options |= OPTION_DIFFONLY;

      break;

    case NO_SAME_FLAG_CODE:

      options &= ~OPTION_OUTPUTSAME;

      break;

    case NO_PURE_FLAG_CODE:

      options &= ~OPTION_OUTPUTPURE;

      break;

    case DEBUG_FLAG_SHORT:
      options |= SRCML_OPTION_DEBUG;
      break;


    case LANGUAGE_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      srcml_archive_set_language(soptions.archive, optarg);

      break;

    case DIRECTORY_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);


      srcml_archive_set_directory(soptions.archive, optarg);

      break;

    case FILENAME_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      srcml_archive_set_filename(soptions.archive, optarg);
      
      break;

    case SRCVERSION_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      srcml_archive_set_version(soptions.archive, optarg);

      break;

      /*
        case INPUT_FORMAT_FLAG_CODE:

        // check for missing argument confused by an argument that looks like an option
        checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

        options |= OPTION_INPUT_FORMAT;

        soptions.input_format = optarg;
        break;
      */
      /*
        case OUTPUT_FORMAT_FLAG_CODE:

        // check for missing argument confused by an argument that looks like an option
        checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

        options |= OPTION_OUTPUT_FORMAT;

        soptions.output_format = optarg;
        break;
      */

    case TABS_FLAG_CODE :
      /*
      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);
      */

      options |= SRCML_OPTION_POSITION;

      char * end;
      srcml_archive_set_tabstop(soptions.archive, strtol(optarg, &end, 10));

      break;

    case CPP_MARKUP_ELSE_FLAG_CODE:
      if (cpp_else) {
        fprintf(stderr, "%s: Conflicting options %s and %s selected.\n",
                PROGRAM_NAME, CPP_MARKUP_ELSE_FLAG, CPP_TEXTONLY_ELSE_FLAG);
        exit(STATUS_INVALID_OPTION_COMBINATION);
      }

      srcml_archive_enable_option(soptions.archive, SRCML_OPTION_CPP_TEXT_ELSE);
      cpp_else = true;

      break;

    case CPP_TEXTONLY_ELSE_FLAG_CODE:
      if (cpp_else) {
        fprintf(stderr, "%s: Conflicting options %s and %s selected.\n",
                PROGRAM_NAME, CPP_MARKUP_ELSE_FLAG, CPP_TEXTONLY_ELSE_FLAG);
        exit(STATUS_INVALID_OPTION_COMBINATION);
      }

      srcml_archive_disable_option(soptions.archive, SRCML_OPTION_CPP_TEXT_ELSE);
      cpp_else = true;

      break;

    case CPP_MARKUP_IF0_FLAG_CODE:
      if (cpp_if0) {
        fprintf(stderr, "%s: Conflicting options %s and %s selected.\n",
                PROGRAM_NAME, CPP_MARKUP_IF0_FLAG, CPP_TEXTONLY_IF0_FLAG);
        exit(STATUS_INVALID_OPTION_COMBINATION);
      }

      srcml_archive_enable_option(soptions.archive, SRCML_OPTION_CPP_MARKUP_IF0);
      cpp_if0 = true;

      break;

    case CPP_TEXTONLY_IF0_FLAG_CODE:
      if (cpp_if0) {
        fprintf(stderr, "%s: Conflicting options %s and %s selected.\n",
                PROGRAM_NAME, CPP_MARKUP_IF0_FLAG, CPP_TEXTONLY_IF0_FLAG);
        exit(STATUS_INVALID_OPTION_COMBINATION);
      }

      srcml_archive_disable_option(soptions.archive, SRCML_OPTION_CPP_MARKUP_IF0);
      cpp_if0 = true;

      break;

    default:
      exit(STATUS_UNKNOWN_OPTION);
      break;
    };
  }

  return optind;
}

int option_error_status(int optopt) {

  switch (optopt) {

  case FILENAME_FLAG_SHORT:
    return 1;//STATUS_FILENAME_MISSING;
    break;

  case LANGUAGE_FLAG_SHORT:
    return 1;//STATUS_LANGUAGE_MISSING;
    break;

  case DIRECTORY_FLAG_SHORT:
    return 1;//STATUS_DIRECTORY_MISSING;
    break;

  case SRCVERSION_FLAG_SHORT:
    return 1;//STATUS_VERSION_MISSING;
    break;

  case ENCODING_FLAG_SHORT:
    return 1;//STATUS_XMLENCODING_MISSING;
    break;

  case SRC_ENCODING_FLAG_SHORT:
    return 1;//STATUS_SRCENCODING_MISSING;
    break;
    /*
      case INPUT_FORMAT_FLAG_CODE:
      return STATUS_ERROR;
      break;
    */
  case OUTPUT_FORMAT_FLAG_CODE:
    return STATUS_ERROR;
    break;

  case TABS_FLAG_CODE:
    return STATUS_ERROR;
    break;

  case REGISTER_EXT_FLAG_CODE:
    return STATUS_ERROR;
    break;

  case '\0':
    return STATUS_UNKNOWN_OPTION;
    break;
  };

  return 0;
}

// output help
void output_help(const char* name) {
  printf( "Usage: %s [options] <old_src_infile new_src_infile>... [-o <srcDiff_outfile>]\n\n"

          "Translates C, C++, and Java source code into the XML source-code representation srcDiff.\n"
          //"Input can be from standard input, a file, a directory, or an archive file, i.e., tar, cpio, and zip.\n"
          "Multiple files are stored in a srcDiff archive.\n\n"

          "The source-code language is based on the file extension.  Additional extensions for a language\n"
          "can be registered, and can be directly set using the --language option.\n\n"

          "By default, output is to stdout.  You can specify a file for output using the --%s or -%c option.\n"
          //"When no filenames are given, input is from stdin and output is to stdout.\n"
          //"An input filename of '-' also reads from stdin.\n\n"

          "Any input file can be a local filename (FILE) or a URI with the protocols http:, ftp:, or file:\n\n"

          "Options:\n", name, OUTPUT_FLAG, OUTPUT_FLAG_SHORT);

  printf("  -%c, --%-17s display this help and exit\n",      HELP_FLAG_SHORT, HELP_FLAG);
  printf("  -%c, --%-17s display version number and exit\n\n", VERSION_FLAG_SHORT, VERSION_FLAG);

  printf("  -%c, --%-17s set the language to %s, %s, or %s\n",
         LANGUAGE_FLAG_SHORT, LANGUAGE_FLAG_FULL,
         LANGUAGE_C, LANGUAGE_CXX, LANGUAGE_JAVA);
  printf("  --%-21s register file extension EXT for source-code language LANG\n\n",
         REGISTER_EXT_FLAG_FULL);

  printf("  -%c, --%-17s write result to OUTPUT which is a FILE or URI\n", OUTPUT_FLAG_SHORT, OUTPUT_FLAG_FULL);
  printf("  --%-21s read list of source file names, either FILE or URI, from INPUT\n"
         "                          to form a srcDiff archive\n",
         FILELIST_FLAG_FULL);
  /*
    printf("  --%-21s ???\n\n", INPUT_FORMAT_FLAG);
    printf("  -%c, --%-17s write result to OUTPUT which is a FILE or URI\n", OUTPUT_FLAG_SHORT, OUTPUT_FLAG_FULL);
  */
  /*
    printf("  --%-21s ???\n\n", OUTPUT_FORMAT_FLAG);
  */
  printf("  -%c, --%-17s store output in a srcDiff archive, default for multiple input files\n",
         COMPOUND_FLAG_SHORT, COMPOUND_FLAG);

  printf("  -%c, --%-17s expression mode for translating a single expression not in a statement\n",
         EXPRESSION_MODE_FLAG_SHORT, EXPRESSION_MODE_FLAG);

  printf("  -%c, --%-17s set the output XML encoding to ENC (default:  %s)\n",
         ENCODING_FLAG_SHORT, ENCODING_FLAG_FULL, DEFAULT_XML_ENCODING);

  printf("  -%c, --%-17s set the input source encoding to ENC (default:  %s)\n\n",
         SRC_ENCODING_FLAG_SHORT, SRC_ENCODING_FLAG_FULL, DEFAULT_TEXT_ENCODING);

  //printf("  -%c, --%-17s output in gzip format\n", COMPRESSED_FLAG_SHORT, COMPRESSED_FLAG);

  //printf("  -%c, --%-17s immediate output while parsing, default for keyboard input\n", INTERACTIVE_FLAG_SHORT, INTERACTIVE_FLAG);

  printf("  -%c, --%-17s markup translation errors, namespace %s\n",
         DEBUG_FLAG_SHORT, DEBUG_FLAG, SRCML_ERR_NS_URI);

  printf("  -%c, --%-17s conversion and status information to stderr\n",
         VERBOSE_FLAG_SHORT, VERBOSE_FLAG);

  printf("  -%c, --%-17s suppresses status messages\n\n",
         QUIET_FLAG_SHORT, QUIET_FLAG);

  printf("  --%-21s do not output the default XML declaration\n", NO_XML_DECLARATION_FLAG);
  printf("  --%-21s do not output any namespace declarations\n\n", NO_NAMESPACE_DECLARATION_FLAG);

  printf("Metadata Options:\n"
         "  -%c, --%-17s set the directory attribute to DIR\n",
         DIRECTORY_FLAG_SHORT, DIRECTORY_FLAG_FULL);

  printf("  -%c, --%-17s set the filename attribute to FILE\n",
         FILENAME_FLAG_SHORT, FILENAME_FLAG_FULL);

  printf("  -%c, --%-17s set the version attribute to VER\n\n",
         SRCVERSION_FLAG_SHORT, SRCVERSION_FLAG_FULL);

  printf("srcML Markup Extensions:\n");

  printf("  --%-21s markup literal values, namespace \"%s\"\n",
         LITERAL_FLAG, SRCML_EXT_LITERAL_NS_URI);

  printf("  --%-21s markup operators, namespace \"%s\"\n",
         OPERATOR_FLAG, SRCML_EXT_OPERATOR_NS_URI);

  printf("  --%-21s markup type modifiers, namespace \"%s\"\n",
         MODIFIER_FLAG, SRCML_EXT_MODIFIER_NS_URI);

  /*
    printf("\nLine/Column Position:\n");

    printf("  --%-21s include line/column attributes, namespace \"%s\"\n",
    POSITION_FLAG, SRCML_EXT_POSITION_NS_URI);

    printf("  --%-21s set tabs NUMBER characters apart.  Default is %d\n",
    TABS_FLAG_FULL, 8);
  */

  printf("\nPrefix Options:\n");

  printf("  --%-21s set the default namespace URI\n", XMLNS_DEFAULT_FLAG_FULL);

  printf("  --%-21s set the namespace PREFIX for the namespace URI\n\n", XMLNS_FLAG_FULL);

  printf("                          Predefined URIs and Prefixes:\n");
  printf("                            xmlns=\"%s\"\n", SRCML_SRC_NS_URI);
  printf("                            xmlns:%s=\"%s\"\n", SRCML_CPP_NS_PREFIX_DEFAULT, SRCML_CPP_NS_URI);
  printf("                            xmlns:%s=\"%s\"\n", SRCML_ERR_NS_PREFIX_DEFAULT, SRCML_ERR_NS_URI);
  printf("                            xmlns:%s=\"%s\"\n", SRCML_DIFF_NS_PREFIX_DEFAULT, SRCML_DIFF_NS_URI);

  printf("\nCPP Markup Options:\n");

  printf("  --%-21s markup cpp #else regions (default)\n", CPP_MARKUP_ELSE_FLAG);
  printf("  --%-21s leave cpp #else regions as text\n\n", CPP_TEXTONLY_ELSE_FLAG);

  printf("  --%-21s markup cpp #if 0 regions\n", CPP_MARKUP_IF0_FLAG);
  printf("  --%-21s leave cpp #if 0 regions as text (default)\n\n", CPP_TEXTONLY_IF0_FLAG);

  printf("srcDiff Method:\n");

  printf("  --%-21s select srcDiff granularity and markup methods\n", METHOD_FLAG);
  printf("                            %s - collect statements/substatements providing coarser granularity (default)\n", COLLECT_METHOD);
  printf("                            %s - markup with fine granularity\n", RAW_METHOD);

  printf("\n");

  printf("srcDiff Visualization:\n");

  printf("  --%-21s Output html visualization of the differences\n", VISUALIZE_FLAG);
  printf("  --%-21s Include files with no changes \n", SAME_FLAG);
  printf("  --%-21s Do not include files with no change \n", NO_SAME_FLAG);
  printf("  --%-21s Include files that were completely added or deleted \n",  PURE_FLAG);
  printf("  --%-21s Do not include files that were completely added or deleted \n", NO_PURE_FLAG);
  printf("  --%-21s Include changes only i.e. lines where diff occurs and sections of srcdiff \n", CHANGE_FLAG);
  printf("  --%-21s Include only the changes that are in srcdiff, but the line is not in diff \n", SRCDIFFONLY_FLAG);
  printf("  --%-21s Include changes that are in diff, but no srcdiff is on that line \n", DIFFONLY_FLAG);

  printf("\n");

  printf("Examples:  \n"
         //"  " PROGRAM_NAME " (read from standard input, write to standard output)\n"
         "  " PROGRAM_NAME " m.cpp n.cpp                 (compute difference of m.cpp and n.cpp, write to standard output)\n"
         "  " PROGRAM_NAME " m.cpp n.cpp -o diff.cpp.xml (compute difference of m.cpp and n.cpp, write to file diff.cpp.xml)\n"
         "\n"
         "  " PROGRAM_NAME " http://www.sdml.info/projects/srcml/ex/main.cpp http://www.sdml.info/projects/srcml/ex/main.cpp (read from URI)\n"
         "\n"
         "  " PROGRAM_NAME " --directory=src --filename=diff.cpp m.cpp n.cpp -o diff.cpp.xml "
         "(element unit attributes dir \"src\", filename \"diff.cpp\")\n"
         "  " PROGRAM_NAME " --src-encoding=UTF-8      m.cpp n.cpp -o diff.cpp.xml    "
         "       (encoding of input text files are UTF-8)\n"
         "  " PROGRAM_NAME " --xml-encoding=ISO-8859-1 m.cpp n.cpp -o diff.cpp.xml    "
         "       (set encoding of srcDiff file to ISO-8859-1)\n\n");

  printf("www.sdml.info\n"
         "Report bugs to %s\n", EMAIL_ADDRESS);
}

// output version message
void output_version(const char* name) {

    printf("%s Version %s\n", name, "1");

    printf("Using: %s\n", srcml_version_string());

  printf("Using: ");
  if(atoi(xmlParserVersion) == LIBXML_VERSION)
    printf("libxml %d, ", LIBXML_VERSION);
  else
    printf("libxml %s (Compiled %d), ", xmlParserVersion, LIBXML_VERSION);


/*
  if(archive_version_number(), ARCHIVE_VERSION_NUMBER)
    printf("libarchive %d\n", ARCHIVE_VERSION_NUMBER);
  else
    printf("libarchive %d (Compiled %d)\n", archive_version_number(), ARCHIVE_VERSION_NUMBER);
*/
}
#endif
