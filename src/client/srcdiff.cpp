/*
  srcdiff.cpp

  Copyright (C) 2011-2014  SDML (www.sdml.info)

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

#include <srcdiff_translator.hpp>
#include <srcdiff_options.hpp>

#include <srcdiff_local_input.hpp>
#include <srcdiff_svn_input.hpp>

#include <srcml.h>

#include <cstdlib>

void srcdiff_libxml_error(void *ctx, const char *msg, ...) {}

#ifdef __GNUG__
extern "C" void terminate_handler(int);
#endif

int main(int argc, char* argv[]) {

  int exit_status = EXIT_SUCCESS;

  LIBXML_TEST_VERSION

  xmlGenericErrorFunc handler = (xmlGenericErrorFunc) srcdiff_libxml_error;
  initGenericErrorDefaultFunc(&handler);

  // process command-line arguments
  srcdiff_options options = process_command_line(argc, argv);

 srcdiff_source_input * input = 0;
#if SVN

  if(options.svn_url) {

    try {

      input = new srcdiff_svn_input(options);

    } catch(...) {

      std::cerr << "Problem with input url " << *options.svn_url << "for revisions " << options.revision_one << " and " << options.revision_two << '\n';

    }

  } else {
#endif

    input = new srcdiff_local_input(options);

#if SVN
  }
#endif

  if(input) {

    input->consume();
    delete input;

  }

  srcml_free_archive(options.archive);
  
  return exit_status;

}
