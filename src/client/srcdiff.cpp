// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <client_options.hpp>

#include <input_source.hpp>
#include <input_source_local.hpp>
#include <input_source_svn.hpp>
#include <input_source_git.hpp>

#include <srcml.h>

#include <cstdlib>

void srcdiff_libxml_error(void *ctx [[maybe_unused]], const char *msg [[maybe_unused]], ...) {}

srcdiff::input_source * next_input_source(const srcdiff::client_options & options);

int main(int argc, char* argv[]) {

  int exit_status = EXIT_SUCCESS;

  LIBXML_TEST_VERSION

  xmlGenericErrorFunc handler = (xmlGenericErrorFunc) srcdiff_libxml_error;
  initGenericErrorDefaultFunc(&handler);

  // process command-line arguments
  const srcdiff::client_options& options = srcdiff::process_command_line(argc, argv);
  srcML::name_spaces::namespace_registry.init(options.archive);

  bool is_view = options.is_option(srcdiff::OPTION_UNIFIED_VIEW | srcdiff::OPTION_SIDE_BY_SIDE_VIEW);
  try {
    if(!is_view && srcml_archive_write_open_filename(options.archive, options.output_filename.c_str()) != SRCML_STATUS_OK) {
      throw std::string("Output source '" + options.output_filename + "' could not be opened");
    }
  } catch(const std::string & s) {
      std::cerr << "Error: " << s << '\n';
  } 

  srcdiff::input_source * input = next_input_source(options);

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

  if(!is_view) {
    srcml_archive_close(options.archive);
  }
  srcml_archive_free(options.archive);
  
  return exit_status;

}

srcdiff::input_source * next_input_source(const srcdiff::client_options & options) {

  srcdiff::input_source * input = nullptr;

#if SVN

  if(options.svn_url) {

    try {

      input = new input_source_svn(options);

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

      input = new input_source_git(options);

    } catch(const std::string & error) {

      std::cerr << "Error: " << error << '\n';

    } catch(...) {

      std::cerr << "Problem with input url " << *options.git_url << " for revisions " << options.git_revision_one << " and " << options.git_revision_two << '\n';

    }

  } else {
#endif

    try {

     input = new srcdiff::input_source_local(options);

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
