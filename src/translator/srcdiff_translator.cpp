/*
  srcdiff_translator.cpp

  Copyright (C) 2011  SDML (www.sdml.info)

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

  Class for straightforward translation from source code to srcDiff
*/

#include <srcdiff_translator.hpp>

#include <srcdiff_output.hpp>
#include <srcdiff_diff.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcml_converter.hpp>

#include <srcmlns.hpp>
#include <srcmlapps.hpp>
#include <shortest_edit_script.h>
#include <srcdiff_constants.hpp>
#include <LineDiffRange.hpp>

#include <xmlrw.hpp>

#include <strings.h>
#include <cstring>

// constructor
srcdiff_translator::srcdiff_translator(const char* srcdiff_filename,
                                     METHOD_TYPE method,
                                     std::string css,
                                     srcml_archive * archive,
                                     OPTION_TYPE & options,
                                     unsigned long number_context_lines)
  : archive(archive), method(method), output(archive, srcdiff_filename, options, method, srcml_archive_get_prefix_from_uri(archive, SRCDIFF_DEFAULT_NAMESPACE_HREF), css, number_context_lines),
    options(options) {}

// Translate from input stream to output stream
void srcdiff_translator::translate(srcdiff_input & input_old, srcdiff_input & input_new, LineDiffRange line_diff_range,
                                  const char * language, const char * unit_directory, const char * unit_filename, const char * unit_version) {

  line_diff_range.create_line_diff();

  if(!isoption(options, OPTION_OUTPUTSAME) && line_diff_range.get_line_diff() == NULL)
    return;

  int is_old = 0;
  std::thread thread_old(std::ref(input_old), SESDELETE, std::ref(output.get_nodes_old()), std::ref(is_old));

  int is_new = 0;
  std::thread thread_new(std::ref(input_new), SESINSERT, std::ref(output.get_nodes_new()), std::ref(is_new));


  thread_old.join();
  thread_new.join();

  node_sets set_old(output.get_nodes_old(), 0, output.get_nodes_old().size());
  node_sets set_new(output.get_nodes_new(), 0, output.get_nodes_new().size());

  output.initialize(is_old, is_new);

  // run on file level
  if(is_old || is_new) {

    output.start_unit(language, unit_directory, unit_filename, unit_version);

    srcdiff_diff diff(output, &set_old, &set_new);
    diff.output();

    // output remaining whitespace
    srcdiff_whitespace whitespace(output);
    whitespace.output_all();

    output.finish(is_old, is_new, line_diff_range);

  }

  output.reset();

}

srcml_archive * srcdiff_translator::get_archive() {

  return archive;

}

// destructor
srcdiff_translator::~srcdiff_translator() {

  output.close();

}

