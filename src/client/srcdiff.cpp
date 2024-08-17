// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_translator.hpp>
#include <srcdiff_options.hpp>

#include <srcdiff_input_source.hpp>
#include <srcdiff_input_source_local.hpp>
#include <srcdiff_input_source_svn.hpp>
#include <srcdiff_input_source_git.hpp>

#include <srcml.h>

#include <cstdlib>

void srcdiff_libxml_error(void *ctx [[maybe_unused]], const char *msg [[maybe_unused]], ...) {}

srcdiff_input_source * next_input_source(const srcdiff_options & options);

int main(int argc, char* argv[]) {

  int exit_status = EXIT_SUCCESS;

  LIBXML_TEST_VERSION

  xmlGenericErrorFunc handler = (xmlGenericErrorFunc) srcdiff_libxml_error;
  initGenericErrorDefaultFunc(&handler);

  // process command-line arguments
  const srcdiff_options & options = process_command_line(argc, argv);

  srcdiff_input_source * input = next_input_source(options);

  if(input) {

    try {

      input->consume();

    } catch(std::exception & e) {

      std::cerr << "Error: " << e.what() << '\n';

    } catch(const std::string & s) {

      std::cerr << "Error: " << s << '\n';

    } catch(...) {

      std::cerr << "Unknown error occurred.\n";

    }

    delete input;

  }

  srcml_archive_free(options.archive);
  
  return exit_status;

}

srcdiff_input_source * next_input_source(const srcdiff_options & options) {

  srcdiff_input_source * input = nullptr;

#if SVN

  if(options.svn_url) {

    try {

      input = new srcdiff_input_source_svn(options);

    } catch(const std::string & error) {

      std::cerr << "Error: " << error << '\n';

    } catch(...) {

      std::cerr << "Problem with input url " << *options.svn_url << " for revisions " << options.revision_one << " and " << options.revision_two << '\n';

    }

  } else {
#endif

#if GIT

  if(options.git_url) {

    try {

      input = new srcdiff_input_source_git(options);

    } catch(const std::string & error) {

      std::cerr << "Error: " << error << '\n';

    } catch(...) {

      std::cerr << "Problem with input url " << *options.git_url << " for revisions " << options.git_revision_one << " and " << options.git_revision_two << '\n';

    }

  } else {
#endif

    try {

     input = new srcdiff_input_source_local(options);

    } catch(const std::string & error) {

      std::cerr << "Error: " << error << '\n';

    }

#if SVN
  }
#endif

#if GIT
  }
#endif

  return input;

}
