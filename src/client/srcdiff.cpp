// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <client_options.hpp>

#include <input_source_manager.hpp>

#include <input_source.hpp>
#include <input_source_local.hpp>
#include <input_source_svn.hpp>
#include <input_source_git.hpp>

#include <srcml.h>

#include <cstdlib>

void srcdiff_libxml_error(void* ctx [[maybe_unused]], const char* msg [[maybe_unused]], ...) {}

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
  } catch(const std::string& s) {
      std::cerr << "Error: " << s << '\n';
  } 

  if(options.input_manager) {

    try {

      options.input_manager->consume();

    } catch(std::exception& e) {

      std::cerr << "Error: " << e.what() << '\n';

    } catch(const std::string & s) {

      std::cerr << "Error: " << s << '\n';

    } catch(...) {

      std::cerr << "Unknown error occurred.\n";

    }

  }

  delete options.input_manager;

  if(!is_view) {
    srcml_archive_close(options.archive);
  }
  srcml_archive_free(options.archive);
  
  return exit_status;

}
