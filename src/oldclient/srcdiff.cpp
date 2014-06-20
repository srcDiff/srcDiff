/*
  srcdiff.cpp

  Copyright (C) 2011-2011  SDML (www.sdml.info)

  This file is part of the srcDiff translator.

  The srcDiff translator is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcDiff translator is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcDiff translator; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Main program to run the srcDiff translator.
*/

#include <cstring>
#ifdef __GNUG__
#include <sys/stat.h>
#endif
#include <sys/types.h>
#include <dirent.h>
#include <srcmlapps.hpp>
#include <srcmlns.hpp>
#include <Options.hpp>
#include <Methods.hpp>
#include <URIStream.hpp>
#include <getopt.h>
#include <dirent.h>
#include <algorithm>
#include <archive.h>
//#include <libxml_archive_read.hpp>
//#include <libxml_archive_write.hpp>
#include <srcDiffTranslator.hpp>

#include <srcml.h>

#include <cstring>
#include <clocale>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#ifdef SVN
#include <svn_io.hpp>
#endif

#include <srcdiff_options.hpp>
#include <srcdiff_uri.hpp>

#define PROGRAM_NAME "srcdiff"

void output_version(const char* name);

struct stringequal {
  const char *const lhs;

  stringequal(const char *l) : lhs(l) {}
};

bool operator==(const char* lhs, const stringequal& r) {
  return std::strcmp(lhs, r.lhs) == 0;
}

const char* const EXAMPLE_TEXT_FILENAME="foo.cpp";
const char* const EXAMPLE_XML_FILENAME="foo.cpp.xml";

const char FILELIST_COMMENT = '#';

void libxml_error(void *ctx, const char *msg, ...) {}

int option_error_status(int optopt);

const char* urisprefix[] = {

  SRCML_SRC_NS_PREFIX_DEFAULT,
  SRCML_CPP_NS_PREFIX_DEFAULT,
  SRCML_ERR_NS_PREFIX_DEFAULT,
  SRCML_EXT_LITERAL_NS_PREFIX_DEFAULT,
  SRCML_EXT_OPERATOR_NS_PREFIX_DEFAULT,
  SRCML_EXT_MODIFIER_NS_PREFIX_DEFAULT,
  SRCML_EXT_POSITION_NS_PREFIX_DEFAULT,
  SRCML_DIFF_NS_PREFIX_DEFAULT,
};

OPTION_TYPE options = OPTION_THREAD | OPTION_OUTPUTSAME | OPTION_OUTPUTPURE;

#ifdef __GNUG__
extern "C" void verbose_handler(int);

extern "C" void terminate_handler(int);
#endif

process_options* gpoptions = 0;

void srcdiff_archive(srcDiffTranslator& translator, const char* path, const char* dir, const char* root_filename, const char* version, int tabsize, int& count, int & skipped, int & error, bool & showinput, bool shownumber);
void srcdiff_dir_top(srcDiffTranslator& translator, const char * directory_old, const char * directory_new, process_options& poptions, int& count, int & skipped, int & error, bool & showinput, bool shownumber);
void srcdiff_dir(srcDiffTranslator& translator, const char * directory_old, int directory_length_old, const char * directory_new, int directory_length_new, process_options& poptions, int& count, int & skipped, int & error, bool & showinput, bool shownumber, const struct stat& outstat);
void srcdiff_filelist(srcDiffTranslator& translator, process_options& poptions, int& count, int & skipped, int & error, bool & showinput, bool & shownumber);

// translate a file, maybe an archive
void srcdiff_file(srcDiffTranslator& translator, const char* path_one, const char* path_two,
                  int& count, int & skipped, int & error, bool & showinput, bool shownumber = false);

void srcdiff_text(srcDiffTranslator& translator, const char* path_one, const char* path_two,
                  int directory_length_old, int directory_length_new,
                  int& count, int & skipped, int & error, bool & showinput, bool shownumber);

// setup options and collect info from arguments

#define LITERALPLUSSIZE(s) BAD_CAST s, sizeof(s) - 1

int main(int argc, char* argv[]) {

  int exit_status = EXIT_SUCCESS;

  LIBXML_TEST_VERSION

    xmlGenericErrorFunc handler = (xmlGenericErrorFunc) libxml_error;
  initGenericErrorDefaultFunc(&handler);

  /* signal handling */

#if defined(__GNUG__) && !defined(__MINGW32__)
  // signal to toggle verbose flag
  signal(SIGUSR1, verbose_handler);
#endif

  //Language::register_standard_file_extensions();
  srcml_archive * archive = srcml_create_archive();
  srcml_archive_disable_option(archive, SRCML_OPTION_ARCHIVE);
  srcml_archive_enable_option(archive, SRCML_OPTION_NAMESPACE_DECL | SRCML_OPTION_XML_DECL | SRCML_OPTION_HASH | SRCML_OPTION_TERNARY);
  srcml_archive_register_namespace(archive, "diff", "http://www.sdml.info/srcDiff");
  
  process_options poptions =
    {
      0,
      0,
      0,
      0,
      0, //DEFAULT_TEXT_ENCODING,

      METHOD_GROUP,
      std::string(),
      archive, 
      0,
#ifdef SVN
      SVN_INVALID_REVNUM,
      SVN_INVALID_REVNUM
#endif
    };

  gpoptions = &poptions;

  // process command-line arguments
  int curarg = process_args(argc, argv, poptions, options, urisprefix);

  /* Special checks for illegal combinations */

  // first command line parameter after options are the input filenames
  int input_arg_start = curarg;
  int input_arg_end = argc - 1;
  int input_arg_count = input_arg_end - input_arg_start + 1;

  // no output specified, so use stdout
  if (!poptions.srcdiff_filename)
    poptions.srcdiff_filename = "-";

  // if more than one input filename assume nested
  // a single input filename which is an archive is detected during archive processing
  if (input_arg_count / 2 > 1)
    srcml_archive_enable_option(poptions.archive, SRCML_OPTION_ARCHIVE);

#if defined(__GNUC__) && !defined(__MINGW32__)
  /*
  // verify that all input filenames exist and are nice and clean
  for (int i = input_arg_start; i <= input_arg_end; ++i) {

  if (strcmp(argv[i], "-") == 0)
  continue;

  // may be remote
  struct stat instat = { 0 };
  if (stat(argv[i], &instat) == -1) {
  continue;
  fprintf(stderr, "%s: %s '%s'\n", PROGRAM_NAME, strerror(errno), argv[i]);
  exit(STATUS_INPUTFILE_PROBLEM);
  }
  }
  */

  // verify that only one input pipe is STDIN
  struct stat stdiostat = { 0 };
  if (fstat(STDIN_FILENO, &stdiostat) == -1) {
    fprintf(stderr, "%s: %s '%s'\n", PROGRAM_NAME, strerror(errno), "stdin");
    exit(STATUS_INPUTFILE_PROBLEM);
  }

  int stdiocount = 0;
  for (int i = input_arg_start; i <= input_arg_end; ++i) {

    if (strcmp(argv[i], "-") == 0) {
      ++stdiocount;
      continue;
    }

    // may not exist due to race condition, so check again
    struct stat instat = { 0 };
    if (stat(argv[i], &instat) == -1)
      continue;

    if(instat.st_ino == stdiostat.st_ino)
      ++stdiocount;

    if (stdiocount > 1) {
      fprintf(stderr, "%s: Multiple input files are from standard input.\n", PROGRAM_NAME);
      exit(STATUS_INPUTFILE_PROBLEM);
    }
  }

  // verify that the output filename is not the same as any of the input filenames
  struct stat outstat = { 0 };
  stat(poptions.srcdiff_filename, &outstat);
  for (int i = input_arg_start; i <= input_arg_end; ++i) {

    if (strcmp(argv[i], "-") == 0)
      continue;

    // may not exist due to race condition, so check again
    struct stat instat = { 0 };
    if (stat(argv[i], &instat) == -1)
      continue;

    if (instat.st_ino == outstat.st_ino && instat.st_dev == outstat.st_dev) {
      fprintf(stderr, "%s: Input file '%s' is the same as the output file '%s'\n",
              PROGRAM_NAME, argv[i], poptions.srcdiff_filename);
      exit(STATUS_INPUTFILE_PROBLEM);
    }
  }
#endif

  // make sure user did not specify duplicate prefixes as an option
  for (int i = 0; i < num_prefixes - 1; ++i) {
    const char** presult = std::find(&urisprefix[i + 1], &urisprefix[num_prefixes], stringequal(urisprefix[i]));
    if (presult != (urisprefix + num_prefixes)) {

      fprintf(stderr, "%s: Namespace conflict for ", PROGRAM_NAME);
      if (urisprefix[i] == '\0') {
        fprintf(stderr, "default prefix\n");
      } else {
        fprintf(stderr, "prefix \'%s\'\n", urisprefix[i]);
      }
      fprintf(stderr, "Prefix URI conflicts:\n  %s\n  %s\n", uris[i].uri,
              uris[presult - &urisprefix[0]].uri);

      exit(STATUS_INVALID_OPTION_COMBINATION);
    }
  }

#if defined(__GNUG__) && !defined(__MINGW32__)
  // automatic interactive use from stdin (not on redirect or pipe)
  if (input_arg_count == 0 || strcmp(argv[input_arg_start], STDIN) == 0) {

  }
#endif

  /*
  // all input is through libarchive
  if (!isoption(options, OPTION_FILELIST)) {
  if (xmlRegisterInputCallbacks(archiveReadMatch, archiveReadOpen, archiveRead, archiveReadClose) < 0) {
  fprintf(stderr, "%s: failed to register archive handler\n", PROGRAM_NAME);
  exit(1);
  }
  }
  */

  bool showinput = false;
  bool shownumber = false;
  // output source encoding
  if (isoption(options, OPTION_VERBOSE)) {
    fprintf(stderr, "Source encoding:  %s\n", srcml_archive_get_src_encoding(poptions.archive));
    fprintf(stderr, "XML encoding:  %s\n", srcml_archive_get_encoding(poptions.archive));
    showinput = false;
    shownumber = true;
  }

  // filecount
  int count = 0;

  // files skipped
  int skipped = 0;

  // file errors
  int error = 0;

#ifdef SVN

  if(isoption(options, OPTION_SVN) && isoption(options, OPTION_SVN_CONTINUOUS)) {

    if (xmlRegisterInputCallbacks(svnReadMatch, svnReadOpen, svnRead, svnReadClose) < 0) {
      fprintf(stderr, "%s: failed to register archive handler\n", PROGRAM_NAME);
      exit(1);
    }


    svn_process_session_all(poptions.revision_one, poptions.revision_two, poptions.svn_url, count, skipped, error, showinput,shownumber,
                            poptions.srcdiff_filename,
                            poptions.method,
                            poptions.css_url,
                            poptions.archive,
                            options);

    exit(0);

  }

  if(isoption(options, OPTION_SVN) && isoption(options, OPTION_FILELIST)) {

    if (xmlRegisterInputCallbacks(svnReadMatch, svnReadOpen, svnRead, svnReadClose) < 0) {
      fprintf(stderr, "%s: failed to register archive handler\n", PROGRAM_NAME);
      exit(1);
    }


    svn_process_session_file(poptions.file_list_name, poptions.revision_one, poptions.revision_two, poptions.svn_url, count, skipped, error, showinput,shownumber,
                            poptions.srcdiff_filename,
                            poptions.method,
                            poptions.css_url,
                            poptions.archive,
                            options);

    exit(0);

  }
#endif
  try {

    // translator from input to output using determined language
    srcDiffTranslator translator(poptions.srcdiff_filename,
                                 poptions.method,
                                 poptions.css_url,
                                 poptions.archive,
                                 options);



#ifdef __GNUG__
    // setup so we can gracefully stop after a file at a time
    signal(SIGINT, terminate_handler);
#endif

    // translate input filenames from list in file
    if (isoption(options, OPTION_FILELIST)) {

      // if we don't have a filelist yet, get it from the first argument
      if (!poptions.file_list_name && input_arg_count > 0)
        poptions.file_list_name = argv[input_arg_start];

      // still no filelist? use stdin
      if (!poptions.file_list_name)
        poptions.file_list_name = STDIN;

      // so process the filelist
      srcdiff_filelist(translator, poptions, count, skipped, error, showinput, shownumber);

#ifdef SVN
      // translate from standard input
    } else if(isoption(options, OPTION_SVN)) {

      if (xmlRegisterInputCallbacks(svnReadMatch, svnReadOpen, svnRead, svnReadClose) < 0) {
        fprintf(stderr, "%s: failed to register archive handler\n", PROGRAM_NAME);
        exit(1);
      }

      svn_process_session(poptions.revision_one, poptions.revision_two, translator, poptions.svn_url, options, count, skipped, error, showinput,shownumber);
#endif
    } else if (input_arg_count == 0) {

      // translate from standard input using any directory, filename and version given on the command line
      srcdiff_file(translator, STDIN, STDIN,
                   count, skipped, error, showinput, shownumber);

      // translate filenames from the command line
    } else {

      // translate in batch the input files on the command line extracting the directory and filename attributes
      // from the full path
      for (int i = input_arg_start; (i  + 1) <= input_arg_end; i += 2) {

        srcdiff_file(translator, argv[i], argv[i + 1], count, skipped, error, showinput, shownumber);

        /*
        // process this command line argument
        srcdiff_file(translator, argv[i], options,
        input_arg_count == 1 ? poptions.given_directory : 0,
        poptions.language,
        poptions.tabsize,
        count, skipped, error, showinput, shownumber);
        */
      }

    }

    if (count == 0)
      exit(STATUS_INPUTFILE_PROBLEM);

    else if (showinput && isoption(options, SRCML_OPTION_ARCHIVE) && !isoption(options, OPTION_QUIET)) {
      fprintf(stderr, "\n"
              "Translated: %d\t"
              "Skipped: %d\t"
              "Error: %d\t"
              "Total: %d\n", count, skipped, error, count + skipped + error);

    }

  } catch (const std::exception & e) {

    fprintf(stderr, "%s: Error Executing command. %s\n", PROGRAM_NAME, e.what());

  }

  srcml_free_archive(poptions.archive);
  
  return exit_status;

}



#ifdef __GNUG__
extern "C" void verbose_handler(int) {


}

extern "C" void terminate_handler(int) {


  // turn off handler for this signal
  signal(SIGINT, SIG_DFL);
}
#endif

void srcdiff_file(srcDiffTranslator& translator, const char* path_one, const char* path_two,
                  int& count, int & skipped, int & error, bool & showinput, bool shownumber) {

  // handle local directories specially
  struct stat instat = { 0 };
  int stat_status = stat(path_one, &instat);
  if (!stat_status && S_ISDIR(instat.st_mode)) {
    srcdiff_dir_top(translator, path_one, path_two, *gpoptions, count, skipped, error, showinput, shownumber);
    return;
  }

  srcdiff_text(translator, path_one, path_two, 0, 0, count, skipped,
               error, showinput, shownumber);

}

void srcdiff_text(srcDiffTranslator& translator, const char* path_one, const char* path_two,
                  int directory_length_old, int directory_length_new,
                  int& count, int & skipped, int & error, bool & showinput, bool shownumber) {

  std::string filename = path_one[0] ? path_one + directory_length_old : path_one;
  if(path_two[0] == 0 || strcmp(path_one + directory_length_old, path_two + directory_length_new) != 0) {

    filename += "|";
    filename += path_two[0] ? path_two + directory_length_new : path_two;

  }

  // Remove eventually
/*  int real_language = language ? language : Language::getLanguageFromFilename(path_one);

  real_language = real_language ? real_language : Language::getLanguageFromFilename(path_two);

  if (!(real_language == Language::LANGUAGE_JAVA || real_language == Language::LANGUAGE_ASPECTJ))
    local_options |= OPTION_CPP;

  if (!real_language && !isoption(options, OPTION_QUIET)) {
    fprintf(stderr, !shownumber ? "Skipped '%s|%s':  Unregistered extension\n" :
            "    - '%s|%s'\tSkipped: Unregistered extension\n",
            path_one, path_two);

    ++skipped;

    return;
  }
*/
  ++count;

  if(showinput && !isoption(srcml_archive_get_options(gpoptions->archive), OPTION_QUIET))
    fprintf(stderr, "%5d '%s|%s'\n", count, path_one, path_two);

  translator.translate(path_one, path_two, srcml_archive_get_directory(gpoptions->archive), filename.c_str(), 0);

  /*
  // single file archive (tar, zip, cpio, etc.) is listed as a single file
  // but is much, much more
  OPTION_TYPE save_options = options;

  try {

  bool foundfilename = true;

  std::string unit_filename = path;
  if (root_filename)
  unit_filename = root_filename;
  else if (strcmp(path, STDIN))
  unit_filename = path;
  else
  foundfilename = false;

  // language based on extension, if not specified

  // 1) language may have been explicitly specified
  int reallanguage = language;

  // 2) try from the filename (basically the extension)
  if (!reallanguage)
  reallanguage = Language::getLanguageFromFilename(unit_filename.c_str());

  // error if can't find a language
  if (!reallanguage) {

  if (!isoption(options, OPTION_QUIET)) {
  if (unit_filename == "-")
  fprintf(stderr, "Skipped:  Must specify language for standard input.\n" );
  else
  fprintf(stderr, !shownumber ? "Skipped '%s':  Unregistered extension.\n" :
  "    - %s\tSkipped: Unregistered extension.\n",
  unit_filename.c_str() ? unit_filename.c_str() : "standard input");
  }

  ++skipped;

  return;
  }

  // turnon cpp namespace for non Java-based languages
  if (!(reallanguage == Language::LANGUAGE_JAVA || reallanguage == Language::LANGUAGE_ASPECTJ))
  options |= OPTION_CPP;

  // open up the file
  void* context = translator.setInput(path);

  // check if file is bad
  if (!context || archiveReadStatus(context) < 0 ) {
  fprintf(stderr, "%s: Unable to open file %s\n", PROGRAM_NAME, path);

  options = save_options;
  ++error;

  return;
  }
  /
  // another file
  ++count;


  const char* c_filename = clean_filename(unit_filename.c_str());

  // output the currently processed filename
  if (!isoption(options, OPTION_QUIET) && shownumber)
  fprintf(stderr, "%5d %s\n", count, c_filename);

  // translate the file
  translator.translate(path, dir,
  foundfilename ? c_filename : 0,
  version, reallanguage);

  } catch (FileError) {

  // output tracing information about the input file
  if (showinput && !isoption(options, OPTION_QUIET)) {

  // output the currently processed filename
  fprintf(stderr, "Path: %s", strcmp(path, STDIN) == 0 ? "standard input" : path);
  fprintf(stderr, "\tError: Unable to open file.\n");

  } else {

  if (dir)
  fprintf(stderr, "%s: Unable to open file %s/%s\n", PROGRAM_NAME, dir, path);
  else
  fprintf(stderr, "%s: Unable to open file %s\n", PROGRAM_NAME, path);
  }

  ++error;
  }

  options = save_options;
  */
}
#if 0
void srcdiff_archive(srcDiffTranslator& translator, const char* path, OPTION_TYPE& options, const char* dir, const char* root_filename, const char* version, int tabsize, int& count, int & skipped, int & error, bool & showinput, bool shownumber) {

  // single file archive (tar, zip, cpio, etc.) is listed as a single file
  // but is much, much more
  OPTION_TYPE save_options = options;

  // process the individual file (once), or an archive as many times as it takes
  void* context = 0;
  bool isarchive = false;
  //bool firstopen = true;
  do {

    // start with the original options
    options = save_options;
    std::string unit_filename;

    try {

      /*      // open up the file
              if (firstopen)
              context = translator.setInput(path);
              else*/
      //context = getContext();

      // check if file is bad
      if (!context || archiveReadStatus(context) < 0 ) {
        fprintf(stderr, "%s: Unable to open file %s\n", PROGRAM_NAME, path);
        ++error;
        return;
      }

      // so, do we have an archive?
      //isarchive = isArchiveRead(context);

      // once any source archive is input, then we have to assume nested not just locally
      if (isarchive) {
        translator.set_nested();
        save_options |= SRCML_OPTION_ARCHIVE;
        showinput = true;
        //        shownumber = true;
      }

      // output tracing information about the input file
      //if (showinput && isArchiveFirst(context) && !isoption(options, OPTION_QUIET)) {
      if (showinput && !isoption(options, OPTION_QUIET)) {

        // output the currently processed filename
        fprintf(stderr, "Path: %s", strcmp(path, STDIN) == 0 ? "standard input" : path);

        // output compression and format (if any)
        if (isarchive)
          fprintf(stderr, "\tFormat: %s", archiveReadFormat(context));

        if (archiveReadCompression(context) && strcmp(archiveReadCompression(context), "none"))
          fprintf(stderr, "\tCompression: %s", archiveReadCompression(context));

        fprintf(stderr, "\n");
      }

      //bool foundfilename = true;
      unit_filename = path;
      if (archiveReadFilename(context))
        unit_filename = archiveReadFilename(context);
      else if (root_filename)
        unit_filename = root_filename;
      else if (strcmp(path, STDIN))
        unit_filename = path;
      //else
      //foundfilename = false;

      // special case:  skip directories (in archives)
      if (archiveIsDir(context)) {

        if (!isoption(options, OPTION_QUIET))
          fprintf(stderr, !shownumber ? "Skipped '%s':  Directory\n" :
                  "    - %s\tSkipped: Directory\n", unit_filename.c_str());

        ++skipped;

        // explicitly close, since we are skipping it
        archiveReadClose(context);

        continue;
      }

      // language (for this item in archive mode) based on extension, if not specified

      // 1) language may have been specified explicitly
      int reallanguage = language;

      // 2) try from the filename (basically the extension)
      if (!reallanguage)
        reallanguage = Language::getLanguageFromFilename(unit_filename.c_str());

      // error if can't find a language
      if (!reallanguage) {

        if (!isoption(options, OPTION_QUIET)) {
          if (unit_filename == "-")
            fprintf(stderr, "Skipped:  Must specify language for standard input.\n" );
          else
            fprintf(stderr, !shownumber ? "Skipped '%s':  Unregistered extension\n" :
                    "    - %s\tSkipped: Unregistered extension\n",
                    unit_filename.c_str() ? unit_filename.c_str() : "standard input");
        }

        ++skipped;

        // close the file that we don't have a language for
        archiveReadClose(context);

        continue;
      }

      // turnon cpp namespace for non Java-based languages
      if (!(reallanguage == Language::LANGUAGE_JAVA || reallanguage == Language::LANGUAGE_ASPECTJ))
        options |= OPTION_CPP;

      // another file
      ++count;

      const char* c_filename = clean_filename(unit_filename.c_str());

      // output the currently processed filename
      if (!isoption(options, OPTION_QUIET) && shownumber)
        fprintf(stderr, "%5d %s\n", count, c_filename);

      // open up the file
      /*      if (!firstopen)
              context = translator.setInput(path);
      */

      // translate the file
      /*      translator.translate(path, dir,
              foundfilename ? c_filename : 0,
              version, reallanguage);
      */
    } catch (... /*FileError*/) {

      // output tracing information about the input file
      if (showinput && !isoption(options, OPTION_QUIET)) {

        // output the currently processed filename
        fprintf(stderr, "Path: %s", strcmp(path, STDIN) == 0 ? "standard input" : path);
        fprintf(stderr, "\tError: Unable to open file.\n");

      } else {

        if (dir)
          fprintf(stderr, "%s: Unable to open file %s/%s\n", PROGRAM_NAME, dir, path);
        else
          fprintf(stderr, "%s: Unable to open file %s\n", PROGRAM_NAME, path);
      }

      ++error;

      return;
      //      exit(STATUS_INPUTFILE_PROBLEM);
    }

    // restore options for next time around
    options = save_options;

    // compound documents are interrupted gracefully
    if (isoption(options, OPTION_TERMINATE))
      return;
    //     return STATUS_TERMINATED;

    //firstopen = false;

  } while (isarchive && isAnythingOpen(context));
}
#endif
void srcdiff_dir_top(srcDiffTranslator& translator, const char * directory_old, const char * directory_new, process_options& poptions, int& count, int & skipped, int & error, bool & showinput, bool shownumber) {

  // by default, all dirs are treated as an archive
  srcml_archive_enable_option(poptions.archive, SRCML_OPTION_ARCHIVE);

  // record the stat info on the output file

  struct stat outstat = { 0 };
  stat(gpoptions->srcdiff_filename, &outstat);

  showinput = true;

  int directory_length_old = strlen(directory_old);

  char * dold = (char *)directory_old;

  char * dnew = (char *)directory_new;

  if(dold[directory_length_old - 1] != '/') {

    ++directory_length_old;

  } else {

    dold[directory_length_old - 1] = '\0';

  }

  int directory_length_new = strlen(directory_new);

  if(dnew[directory_length_new - 1] != '/') {

    ++directory_length_new;

  } else {

    dnew[directory_length_new - 1] = '\0';

  }

  std::string directory = dold;

  if(strcmp(dold, dnew) != 0) {

    directory += "|";
    directory += dnew;

  }

 if(!srcml_archive_get_directory(gpoptions->archive)) srcml_archive_set_directory(gpoptions->archive, directory.c_str());

  srcdiff_dir(translator, dold, directory_length_old, dnew, directory_length_new, *gpoptions, count, skipped, error, showinput, shownumber, outstat);
}

// file/directory names to ignore when processing a directory
// const/non-const versions for linux/bsd different declarations
int dir_filter(const struct dirent* d) {

    return d->d_name[0] != '.';// && !archiveReadMatchExtension(d->d_name);
}

int dir_filter(struct dirent* d) {

  return dir_filter((const struct dirent*)d);
}

int is_dir(struct dirent * file, const char * filename) {

#ifdef _DIRENT_HAVE_D_TYPE
  if (file->d_type == DT_DIR)
    return 1;
#endif

  // path with current filename
  // handle directories later after all the filenames
  struct stat instat = { 0 };

  int stat_status = stat(filename, &instat);

  if(stat_status)
    return stat_status;

#ifndef _DIRENT_HAVE_D_TYPE
  if(S_ISDIR(instat.st_mode))
    return 1;
#endif

  return 0;

}

int is_output_file(const char * filename, const struct stat & outstat) {

  struct stat instat = { 0 };

  int stat_status = stat(filename, &instat);

  if(stat_status)
    return stat_status;

  if(instat.st_ino == outstat.st_ino && instat.st_dev == outstat.st_dev)
    return 1;

  return 0;

}

void noteSkipped(bool shownumber, const process_options& options) {

  fprintf(stderr, !shownumber ? "Skipped '%s':  Output file.\n" :
          "    - %s\tSkipped: Output file.\n", options.srcdiff_filename);
}

void srcdiff_dir(srcDiffTranslator& translator, const char * directory_old, int directory_length_old, const char * directory_new, int directory_length_new,
                 process_options& poptions, int& count, int & skipped, int & error, bool & showinput, bool shownumber, const struct stat& outstat) {

#if defined(__GNUC__) && !defined(__MINGW32__)

  // collect the filenames in alphabetical order
  struct dirent **namelist_old;
  struct dirent **namelist_new;

  int n = scandir(directory_old, &namelist_old, dir_filter, alphasort);
  int m = scandir(directory_new, &namelist_new, dir_filter, alphasort);
  // TODO:  Fix error handling.  What if one is in error?
  if (n < 0 && m < 0) {
    return;
  }

  // start of path from directory name
  std::string filename_old = directory_old;
  if (filename_old != "" && !filename_old.empty() && filename_old[filename_old.size() - 1] != PATH_SEPARATOR)
    filename_old += PATH_SEPARATOR;
  int basesize_old = filename_old.length();

  std::string filename_new = directory_new;
  if (filename_new != "" && !filename_new.empty() && filename_new[filename_new.size() - 1] != PATH_SEPARATOR)
    filename_new += PATH_SEPARATOR;
  int basesize_new = filename_new.length();

  // process all non-directory files
  int i = 0;
  int j = 0;
  while (i < n && j < m) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, namelist_old[i]->d_name);
    filename_new.replace(basesize_new, std::string::npos, namelist_new[j]->d_name);

    // skip directories
    if(is_dir(namelist_old[i], filename_old.c_str()) != 0) {
      ++i;
      continue;
    }
    if(is_dir(namelist_new[j], filename_new.c_str()) != 0) {
      ++j;
      continue;
    }

    // skip over output file
    if (is_output_file(filename_old.c_str(), outstat) == 1) {
      noteSkipped(shownumber, poptions);
      ++i;
      ++skipped;
      continue;
    }
    if (is_output_file(filename_new.c_str(), outstat) == 1) {
      noteSkipped(shownumber, poptions);
      ++j;
      ++skipped;
      continue;
    }

    // is this a common, inserted, or deleted file?
    int comparison = strcoll(namelist_old[i]->d_name, namelist_new[j]->d_name);

    // translate the file listed in the input file using the directory and filename extracted from the path
    srcdiff_text(translator,
                 comparison <= 0 ? (++i, filename_old.c_str()) : "",
                 comparison >= 0 ? (++j, filename_new.c_str()) : "",
                 directory_length_old,
                 directory_length_new,
                 count, skipped, error, showinput, shownumber);
  }

  // process all non-directory files that are remaining in the old version
  for ( ; i < n; ++i) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, namelist_old[i]->d_name);

    // skip directories
    if(is_dir(namelist_old[i], filename_old.c_str()) != 0)
      continue;

    // skip over output file
    if (is_output_file(filename_old.c_str(), outstat) != 0) {
      noteSkipped(shownumber, poptions);
      ++skipped;
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
    srcdiff_text(translator,
                 filename_old.c_str(),
                 "",
                 directory_length_old,
                 directory_length_new,
                 count, skipped, error, showinput, shownumber);
  }

  // process all non-directory files that are remaining in the new version
  for ( ; j < m; ++j) {

    // form the full path
    filename_new.replace(basesize_new, std::string::npos, namelist_new[j]->d_name);

    // skip directories
    if(is_dir(namelist_new[j], filename_new.c_str()) != 0)
      continue;

    // skip over output file
    if (is_output_file(filename_new.c_str(), outstat) != 0) {
      noteSkipped(shownumber, poptions);
      ++skipped;
      continue;
    }

    // translate the file listed in the input file using the directory and filename extracted from the path
    srcdiff_text(translator,
                 "",
                 filename_new.c_str(),
                 directory_length_old,
                 directory_length_new,
                 count, skipped, error, showinput, shownumber);
  }

  // no need to handle subdirectories, unless recursive
  //  if (!isoption(options, OPTION_RECURSIVE))
  //    return;

  // process all directories
  i = 0;
  j = 0;
  while (i < n && j < m) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, namelist_old[i]->d_name);
    filename_new.replace(basesize_new, std::string::npos, namelist_new[j]->d_name);

    // skip non-directories
    if(is_dir(namelist_old[i], filename_old.c_str()) != 1) {
      ++i;
      continue;
    }
    if(is_dir(namelist_new[j], filename_new.c_str()) != 1) {
      ++j;
      continue;
    }

    // is this a common, inserted, or deleted directory?
    int comparison = strcoll(namelist_old[i]->d_name, namelist_new[j]->d_name);

    // process these directories
    srcdiff_dir(translator,
                comparison <= 0 ? (++i, filename_old.c_str()) : "",
                directory_length_old,
                comparison >= 0 ? (++j, filename_new.c_str()) : "",
                directory_length_new,
                poptions,
                count, skipped, error, showinput, shownumber, outstat);
  }

  // process all directories that remain in the old version
  for ( ; i < n; ++i) {

    // form the full path
    filename_old.replace(basesize_old, std::string::npos, namelist_old[i]->d_name);

    // skip non-directories
    if(is_dir(namelist_old[i], filename_old.c_str()) != 1)
      continue;

    // skip over output file
    if (is_output_file(filename_old.c_str(), outstat) == 1) {
      noteSkipped(shownumber, poptions);
      ++skipped;
      continue;
    }

    // process this directory
    srcdiff_dir(translator,
                filename_old.c_str(),
                directory_length_old,
                "",
                directory_length_new,
                poptions,
                count, skipped, error, showinput, shownumber, outstat);
  }

  // process all directories that remain in the new version
  for ( ; j < m; ++j) {

    // form the full path
    filename_new.replace(basesize_new, std::string::npos, namelist_new[j]->d_name);

    // skip non-directories
    if(is_dir(namelist_new[j], filename_new.c_str()) != 1)
      continue;

    // skip over output file
    if (is_output_file(filename_new.c_str(), outstat) == 1) {
      noteSkipped(shownumber, poptions);
      ++skipped;
      continue;
    }

    // process this directory
    srcdiff_dir(translator,
                "",
                directory_length_old,
                filename_new.c_str(),
                directory_length_new,
                poptions,
                count, skipped, error, showinput, shownumber, outstat);
  }

  // all done with this directory
  if (n > 0) {
    for (int i = 0; i < n; ++i)
      free(namelist_old[i]);
    free(namelist_old);
  }

  if (m > 0) {
    for (int j = 0; j < m; ++j)
      free(namelist_new[j]);
    free(namelist_new);
  }

#else

#endif
}

void srcdiff_filelist(srcDiffTranslator& translator, process_options& poptions, int& count, int & skipped, int & error, bool & showinput, bool & shownumber) {
  try {

    // translate all the filenames listed in the named file
    // Use libxml2 routines so that we can handle http:, file:, and gzipped files automagically
    URIStream uriinput(poptions.file_list_name);
    char* file_one;
    /*
      if (xmlRegisterInputCallbacks(archiveReadMatch, archiveReadOpen, archiveRead, archiveReadClose) < 0) {
      fprintf(stderr, "%s: failed to register archive handler\n", PROGRAM_NAME);
      exit(1);
      }
    */
    while ((file_one = uriinput.readline())) {

      // skip over whitespace
      // TODO:  Other types of whitespace?  backspace?
      file_one += strspn(file_one, " \t\f");

      // skip blank lines or comment lines
      if (file_one[0] == '\0' || file_one[0] == FILELIST_COMMENT)
        continue;

      char * separator = strchr(file_one, '|');

      // remove any end whitespace
      // TODO:  Extract function, and use elsewhere
      for (char* p = separator- 1; p != file_one; --p) {
        if (isspace(*p))
          *p = 0;
        else
          break;
      }

      showinput = true;

      *separator = 0;

      char * file_two = separator + 1;

      file_two += strspn(file_two, " \t\f");

      // remove any end whitespace
      // TODO:  Extract function, and use elsewhere
      for (char* p = file_two + strlen(file_two) - 1; p != file_two; --p) {
        if (isspace(*p))
          *p = 0;
        else
          break;
      }

      std::string filename = file_one;
      filename += "|";
      filename += file_two;

      srcdiff_text(translator, file_one, file_two, 0, 0, count, skipped, error, showinput, shownumber);

      *separator = '|';

      if (isoption(options, OPTION_TERMINATE))
        return;

      /*
      // process this command line argument
      srcdiff_file(translator, argv[i], options,
      input_arg_count == 1 ? poptions.given_directory : 0,
      input_arg_count == 1 ? poptions.given_filename : 0,
      input_arg_count == 1 ? poptions.given_version : 0,
      poptions.language,
      poptions.tabsize,
      count, skipped, error, showinput, shownumber);
      */

    }

  } catch (URIStreamFileError) {
    fprintf(stderr, "%s error: file/URI \'%s\' does not exist.\n", PROGRAM_NAME, poptions.file_list_name);
    exit(STATUS_INPUTFILE_PROBLEM);
  }

}
