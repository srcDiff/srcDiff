/**
 * @file srcdiff.cpp
 *
 * Copyright (C) 2011-2014  SDML (www.sdml.info)
 *
 * This file is part of the srcDiff translator.
 *
 * The srcDiff translator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The srcDiff translator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcDiff translator; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Main program to run the srcDiff translator.
 */

#include <srcdiff_translator.hpp>
#include <srcdiff_options.hpp>

#include <srcdiff_input_source.hpp>
#include <srcdiff_input_source_local.hpp>
#include <srcdiff_input_source_svn.hpp>

#include <srcml.h>

#include <cstdlib>

void srcdiff_libxml_error(void *ctx, const char *msg, ...) {}

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

    } catch(...) {

      std::cerr << "Problem with input.\n";

    }

    delete input;

  }

  srcml_free_archive(options.archive);
  
  return exit_status;

}

srcdiff_input_source * next_input_source(const srcdiff_options & options) {

  srcdiff_input_source * input = nullptr;

#if SVN

  if(options.svn_url) {

    try {

      input = new srcdiff_input_source_svn(options);

    } catch(...) {

      std::cerr << "Problem with input url " << *options.svn_url << "for revisions " << options.revision_one << " and " << options.revision_two << '\n';

    }

  } else {
#endif

    input = new srcdiff_input_source_local(options);

#if SVN
  }
#endif

  return input;

}
