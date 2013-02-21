#include <getopt.h>
#include <cerrno>

#include "project.hpp"

#include "srcdiff_options.hpp"
#include "srcmlapps.hpp"
#include "srcmlns.hpp"
#include "Options.hpp"
#include "Language.hpp"
#include <archive.h>
#include "libxml_archive_read.hpp"
#include "libxml_archive_write.hpp"

#include "srcDiffOptions.hpp"

#define PROGRAM_NAME "srcdiff"

using namespace LanguageName;

// setup options and collect info from arguments
int process_args(int argc, char* argv[], process_options & poptions, OPTION_TYPE & options, const char * urisprefix[]) {

  bool cpp_if0 = false;
  bool cpp_else = false;

  int curoption = 0;
  struct option cliargs[] = {
    { HELP_FLAG, no_argument, NULL, HELP_FLAG_SHORT },
    { VERSION_FLAG, no_argument, NULL, VERSION_FLAG_SHORT },
    { OUTPUT_FLAG, required_argument, NULL, OUTPUT_FLAG_SHORT },
    { COMPOUND_FLAG, no_argument, NULL, COMPOUND_FLAG_SHORT },
    { EXPRESSION_MODE_FLAG, no_argument, NULL, EXPRESSION_MODE_FLAG_SHORT },
    { ENCODING_FLAG, required_argument, NULL, ENCODING_FLAG_SHORT },
    { SRC_ENCODING_FLAG, required_argument, NULL, SRC_ENCODING_FLAG_SHORT },
    { COMPRESSED_FLAG, no_argument, NULL, COMPRESSED_FLAG_SHORT },
    { INTERACTIVE_FLAG, no_argument, NULL, INTERACTIVE_FLAG_SHORT },
    { DEBUG_FLAG, no_argument, NULL, DEBUG_FLAG_SHORT },
    { VERBOSE_FLAG, no_argument, NULL, VERBOSE_FLAG_SHORT },
    { LANGUAGE_FLAG, required_argument, NULL, LANGUAGE_FLAG_SHORT },
    { DIRECTORY_FLAG, required_argument, NULL, DIRECTORY_FLAG_SHORT },
    { FILENAME_FLAG, required_argument, NULL, FILENAME_FLAG_SHORT },
    { SRCVERSION_FLAG, required_argument, NULL, SRCVERSION_FLAG_SHORT },
    { SETTINGS_FLAG, no_argument, NULL, SETTINGS_FLAG_CODE },
    { FEATURES_FLAG, no_argument, NULL, FEATURES_FLAG_CODE },
    //    { INPUT_FORMAT_FLAG, required_argument, NULL, INPUT_FORMAT_FLAG_CODE },
    //    { OUTPUT_FORMAT_FLAG, required_argument, NULL, OUTPUT_FORMAT_FLAG_CODE },
    { FILELIST_FLAG, required_argument, NULL, FILELIST_FLAG_CODE },
    //    { FILELIST_FLAG, optional_argument, NULL, FILELIST_FLAG_CODE },
    { REGISTER_EXT_FLAG, required_argument, NULL, REGISTER_EXT_FLAG_CODE },
    { XMLNS_FLAG, required_argument, NULL, XMLNS_FLAG_CODE },
    { RECURSIVE_FLAG, no_argument, NULL, RECURSIVE_FLAG_CODE },
    { REVISION_FLAG, no_argument, NULL, REVISION_FLAG_CODE },
    { METHOD_FLAG, required_argument, NULL, METHOD_FLAG_CODE },
    { NO_THREAD_FLAG, no_argument, NULL, NO_THREAD_FLAG_CODE },
    { VISUALIZE_FLAG, optional_argument, NULL, VISUALIZE_FLAG_CODE },
    { SAME_FLAG, no_argument, NULL, SAME_FLAG_CODE },
    { PURE_FLAG, no_argument, NULL, PURE_FLAG_CODE },
    { CHANGE_FLAG, no_argument, NULL, CHANGE_FLAG_CODE },
    { SRCDIFFONLY_FLAG, no_argument, NULL, SRCDIFFONLY_FLAG_CODE },
    { DIFFONLY_FLAG, no_argument, NULL, DIFFONLY_FLAG_CODE },
    { NO_SAME_FLAG, no_argument, NULL, NO_SAME_FLAG_CODE },
    { NO_PURE_FLAG, no_argument, NULL, NO_PURE_FLAG_CODE },
    { QUIET_FLAG, no_argument, NULL, QUIET_FLAG_SHORT },
    { SVN_FLAG, required_argument, NULL, SVN_FLAG_CODE },
    { SVN_CONTINUOUS_FLAG, no_argument, NULL, SVN_CONTINUOUS_FLAG_CODE },
    { NO_XML_DECLARATION_FLAG, no_argument, &curoption, OPTION_XMLDECL | OPTION_XML },
    { NO_NAMESPACE_DECLARATION_FLAG, no_argument, &curoption, OPTION_NAMESPACEDECL | OPTION_XML },
    { OLD_FILENAME_FLAG, no_argument, NULL, OLD_FILENAME_FLAG_CODE },
    { TABS_FLAG, required_argument, NULL, TABS_FLAG_CODE },
    { POSITION_FLAG, no_argument, &curoption, OPTION_POSITION },
    { LITERAL_FLAG, no_argument, &curoption, OPTION_LITERAL },
    { OPERATOR_FLAG, no_argument, &curoption, OPTION_OPERATOR },
    { MODIFIER_FLAG, no_argument, &curoption, OPTION_MODIFIER },
    { CPP_MARKUP_ELSE_FLAG, no_argument, NULL, CPP_MARKUP_ELSE_FLAG_CODE },
    { CPP_TEXTONLY_ELSE_FLAG, no_argument, NULL, CPP_TEXTONLY_ELSE_FLAG_CODE },
    { CPP_MARKUP_IF0_FLAG, no_argument, NULL, CPP_MARKUP_IF0_FLAG_CODE },
    { CPP_TEXTONLY_IF0_FLAG, no_argument, NULL, CPP_TEXTONLY_IF0_FLAG_CODE },
    { 0, 0, 0, 0 }
  };

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

      poptions.srcdiff_filename = optarg;
      break;

    case FILELIST_FLAG_CODE:

      // check for missing argument confused by an argument that looks like an option
      //      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      options |= OPTION_FILELIST;

      // filelist mode is default nested mode
      options |= OPTION_NESTED;

      poptions.file_list_name = optarg;
      break;

    case SVN_FLAG_CODE:

      // check for missing argument confused by an argument that looks like an option
      //      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);
      {

        options |= OPTION_SVN;

        // filelist mode is default nested mode
        options |= OPTION_NESTED;
        poptions.svn_url = optarg;

        const char * end = index(optarg, '@');
        if(end) {

          poptions.svn_url = strndup(optarg, end - optarg);

          const char * first = index(end + 1, '-');
          const char * temp_revision = strndup(end + 1, first - (end + 1));
          poptions.revision_one = atoi(temp_revision);
          free((void *)temp_revision);

          if(first)
            poptions.revision_two = atoi(first + 1);

        }

      }

      break;

    case SVN_CONTINUOUS_FLAG_CODE:

      options |= OPTION_SVN_CONTINUOUS;

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

        if (!Language::registerUserExt(extension, language)) {

          fprintf(stderr, "%s: language \"%s\" is not supported.\n", PROGRAM_NAME, language);
          fprintf(stderr, "Try '%s %s' for more information.\n", PROGRAM_NAME, HELP_FLAG);
          exit(STATUS_ERROR);
        }
      }

      break;

    case NESTED_FLAG_SHORT:
      options |= OPTION_NESTED;
      break;

    case EXPRESSION_MODE_FLAG_SHORT:
      options |= OPTION_EXPRESSION;
      break;

    case ENCODING_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      options |= OPTION_XML_ENCODING;

      poptions.xml_encoding = optarg;

      // validate xml encoding
#ifdef FIX
      if (!srcMLOutput::checkEncoding(poptions.xml_encoding)) {
        fprintf(stderr, "%s: xml encoding \"%s\" is not supported.\n", PROGRAM_NAME, poptions.xml_encoding);
        fprintf(stderr, "Try '%s %s' for more information.\n", PROGRAM_NAME, HELP_FLAG);
        exit(STATUS_UNKNOWN_ENCODING);
      }
#endif
      break;

    case SRC_ENCODING_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      options |= OPTION_SRC_ENCODING;

      poptions.src_encoding = optarg;

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
            exit(STATUS_LANGUAGE_MISSING);
          }

          ns_uri = argv[optind++];
        }

        // check uri to turn on specific option
        bool found = false;
        for (int i = 0; i < num_prefixes; ++i)
          if (strcmp(ns_uri, uris[i].uri) == 0) {

            options |= uris[i].option;

            urisprefix[i] = ns_prefix ? ns_prefix : "";
            poptions.prefixchange[i] = true;
            found = true;
            break;
          }

        if (!found) {
          fprintf(stderr, "%s: invalid namespace \"%s\"\n\n"
                  "Namespace URI must be on of the following:  \n", PROGRAM_NAME, ns_uri);
          for (int i = 0; i < num_prefixes; ++i)
            fprintf(stderr, "  %-35s %s\n", uris[i].uri, uris[i].description);

          exit(STATUS_INVALID_LANGUAGE);
        }
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

      process_method(optarg, poptions);

      break;

    case NO_THREAD_FLAG_CODE:

      options &= ~OPTION_THREAD;

      break;

    case VISUALIZE_FLAG_CODE:

      poptions.css_url = "";
      if(optarg != NULL)
        poptions.css_url = optarg;

      options |= OPTION_VISUALIZE;

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

    case QUIET_FLAG_SHORT:
      options |= OPTION_QUIET;
      break;

    case COMPRESSED_FLAG_SHORT:
      options |= OPTION_COMPRESSED;
      break;

    case INTERACTIVE_FLAG_SHORT:
      options |= OPTION_INTERACTIVE;
      break;

    case DEBUG_FLAG_SHORT:
      options |= OPTION_DEBUG;
      break;

    case VERBOSE_FLAG_SHORT:
      options |= OPTION_VERBOSE;
      break;

    case LANGUAGE_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      options |= OPTION_LANGUAGE;

      // validate language selected
      poptions.language = Language::getLanguage(optarg);
      if (poptions.language == 0) {
        fprintf(stderr, "%s: invalid option -- Language flag must one of the following values:  "
                "%s %s %s %s\n", PROGRAM_NAME, LANGUAGE_C, LANGUAGE_CXX, LANGUAGE_JAVA, LANGUAGE_ASPECTJ);

        exit(STATUS_INVALID_LANGUAGE);
      }
      break;

    case DIRECTORY_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      options |= OPTION_DIRECTORY;

      poptions.given_directory = optarg;
      break;

    case FILENAME_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      options |= OPTION_FILENAME;

      poptions.given_filename = optarg;
      break;

    case SRCVERSION_FLAG_SHORT:

      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

      options |= OPTION_VERSION;

      poptions.given_version = optarg;
      break;

    case SETTINGS_FLAG_CODE :
      output_settings(PROGRAM_NAME);
      exit(STATUS_SUCCESS);
      break;

    case FEATURES_FLAG_CODE :
      output_features(PROGRAM_NAME);
      exit(STATUS_SUCCESS);
      break;
      /*
        case INPUT_FORMAT_FLAG_CODE:

        // check for missing argument confused by an argument that looks like an option
        checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

        options |= OPTION_INPUT_FORMAT;

        poptions.input_format = optarg;
        break;
      */
      /*
        case OUTPUT_FORMAT_FLAG_CODE:

        // check for missing argument confused by an argument that looks like an option
        checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);

        options |= OPTION_OUTPUT_FORMAT;

        poptions.output_format = optarg;
        break;
      */
    case OLD_FILENAME_FLAG_CODE :
      options |= OPTION_OLD_FILENAME;
      break;

    case TABS_FLAG_CODE :
      /*
      // check for missing argument confused by an argument that looks like an option
      checkargisoption(PROGRAM_NAME, argv[lastoptind], optarg, optind, lastoptind);
      */

      options |= OPTION_POSITION;

      char * end;
      poptions.tabsize = pstd::strtol(optarg, &end, 10);

      // validate type of tabsize number
      if (errno == EINVAL || strlen(end) == strlen(optarg)) {
        fprintf(stderr, "%s: unit option value \"%s\" must be numeric.\n", PROGRAM_NAME, optarg);
        exit(STATUS_UNIT_INVALID);
      }

      // validate range of unit number
      if (poptions.tabsize <= 0) {
        fprintf(stderr, "%s: unit option value \"%d\" must be > 0.\n", PROGRAM_NAME, poptions.tabsize);
        exit(STATUS_UNIT_INVALID);
      }

      break;

    case CPP_MARKUP_ELSE_FLAG_CODE:
      if (cpp_else) {
        fprintf(stderr, "%s: Conflicting options %s and %s selected.\n",
                PROGRAM_NAME, CPP_MARKUP_ELSE_FLAG, CPP_TEXTONLY_ELSE_FLAG);
        exit(STATUS_INVALID_OPTION_COMBINATION);
      }

      options |= OPTION_CPP_MARKUP_ELSE;
      cpp_else = true;

      break;

    case CPP_TEXTONLY_ELSE_FLAG_CODE:
      if (cpp_else) {
        fprintf(stderr, "%s: Conflicting options %s and %s selected.\n",
                PROGRAM_NAME, CPP_MARKUP_ELSE_FLAG, CPP_TEXTONLY_ELSE_FLAG);
        exit(STATUS_INVALID_OPTION_COMBINATION);
      }

      options &= ~OPTION_CPP_MARKUP_ELSE;
      cpp_else = true;

      break;

    case CPP_MARKUP_IF0_FLAG_CODE:
      if (cpp_if0) {
        fprintf(stderr, "%s: Conflicting options %s and %s selected.\n",
                PROGRAM_NAME, CPP_MARKUP_IF0_FLAG, CPP_TEXTONLY_IF0_FLAG);
        exit(STATUS_INVALID_OPTION_COMBINATION);
      }

      options |= OPTION_CPP_MARKUP_IF0;
      cpp_if0 = true;

      break;

    case CPP_TEXTONLY_IF0_FLAG_CODE:
      if (cpp_if0) {
        fprintf(stderr, "%s: Conflicting options %s and %s selected.\n",
                PROGRAM_NAME, CPP_MARKUP_IF0_FLAG, CPP_TEXTONLY_IF0_FLAG);
        exit(STATUS_INVALID_OPTION_COMBINATION);
      }

      options &= ~OPTION_CPP_MARKUP_IF0;
      cpp_if0 = true;

      break;

    default:
      exit(STATUS_UNKNOWN_OPTION);
      break;
    };
  }

  return optind;
}

void process_method(char * optarg, process_options & poptions) {

  char * methods = strdup(optarg);
  char * method;
  while((method = strsep(&methods, ",")) != NULL) {

    if(strcmp(method, COLLECT_METHOD) == 0)
      poptions.method &= ~METHOD_RAW;

    else if(strcmp(method, RAW_METHOD) == 0)
      poptions.method |= METHOD_RAW;

    else if(strcmp(method, NO_GROUP_DIFF_METHOD) == 0)
      poptions.method &= ~METHOD_GROUP;

    else if(strcmp(method, GROUP_DIFF_METHOD) == 0)
      poptions.method |= METHOD_GROUP;

    else {

      fprintf(stderr, "Invalid argument to --method: '%s'\n", optarg);
      exit(STATUS_INVALID_ARGUMENT);

    }

  }

  free(methods);

}

int option_error_status(int optopt) {

  switch (optopt) {

  case FILENAME_FLAG_SHORT:
    return STATUS_FILENAME_MISSING;
    break;

  case LANGUAGE_FLAG_SHORT:
    return STATUS_LANGUAGE_MISSING;
    break;

  case DIRECTORY_FLAG_SHORT:
    return STATUS_DIRECTORY_MISSING;
    break;

  case SRCVERSION_FLAG_SHORT:
    return STATUS_VERSION_MISSING;
    break;

  case ENCODING_FLAG_SHORT:
    return STATUS_XMLENCODING_MISSING;
    break;

  case SRC_ENCODING_FLAG_SHORT:
    return STATUS_SRCENCODING_MISSING;
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

  printf("%s Version %s\n%s\n", name, VERSION,COPYRIGHT);

  printf("Using: ");
  if(atoi(xmlParserVersion) == LIBXML_VERSION)
    printf("libxml %d, ", LIBXML_VERSION);
  else
    printf("libxml %s (Compiled %d), ", xmlParserVersion, LIBXML_VERSION);

  if(archive_version_number(), ARCHIVE_VERSION_NUMBER)
    printf("libarchive %d\n", ARCHIVE_VERSION_NUMBER);
  else
    printf("libarchive %d (Compiled %d)\n", archive_version_number(), ARCHIVE_VERSION_NUMBER);
}

void output_settings(const char * name)
{}

void output_features(const char * name)
{}

