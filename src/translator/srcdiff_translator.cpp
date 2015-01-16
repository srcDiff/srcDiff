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
#include <shortest_edit_script.h>
#include <srcdiff_constants.hpp>

#include <LineDiffRange.hpp>

#include <thread>

#include <strings.h>
#include <cstring>

// constructor
srcdiff_translator::srcdiff_translator(const std::string & srcdiff_filename,
                                       const OPTION_TYPE & flags, const METHOD_TYPE & method,
                                       srcml_archive * archive,
                                       unsigned long number_context_lines)
  : archive(archive), flags(flags), output(archive, srcdiff_filename, flags, method, number_context_lines) {}

// Translate from input stream to output stream
void srcdiff_translator::translate(const srcdiff_input & input_original, const srcdiff_input & input_modified,
                                  LineDiffRange & line_diff_range, const std::string & language,
                                  const boost::optional<std::string> & unit_directory, const boost::optional<std::string> & unit_filename,
                                  const boost::optional<std::string> & unit_version) {

  line_diff_range.create_line_diff();

  if(!isoption(flags, OPTION_SAME) && line_diff_range.get_line_diff() == NULL)
    return;

  int is_original = 0;
  std::thread thread_original(std::ref(input_original), SESDELETE, std::ref(output.get_nodes_original()), std::ref(is_original));

  int is_modified = 0;
  std::thread thread_modified(std::ref(input_modified), SESINSERT, std::ref(output.get_nodes_modified()), std::ref(is_modified));

  thread_original.join();
  thread_modified.join();

  node_sets set_original(output.get_nodes_original(), 0, output.get_nodes_original().size());
  node_sets set_modified(output.get_nodes_modified(), 0, output.get_nodes_modified().size());

  output.initialize(is_original, is_modified);

  // run on file level
  if(is_original || is_modified) {

    output.start_unit(language, unit_directory, unit_filename, unit_version);

    srcdiff_diff diff(output, set_original, set_modified);
    diff.output();

    // output remaining whitespace
    srcdiff_whitespace whitespace(output);
    whitespace.output_all();

    output.finish(is_original, is_modified, line_diff_range);

  }

  output.reset();

}

// destructor
srcdiff_translator::~srcdiff_translator() {

  output.close();

}

