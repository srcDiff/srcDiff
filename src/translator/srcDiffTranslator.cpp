/*
  srcDiffTranslator.cpp

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

#include <srcDiffTranslator.hpp>

#include <srcdiff_input.hpp>
#include <srcdiff_output.hpp>
#include <srcdiff_diff.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_whitespace.hpp>


#include <srcmlns.hpp>
#include <srcmlapps.hpp>
#include <shortest_edit_script.h>
#include <srcdiff_constants.hpp>
#include <srcDiffUtility.hpp>
#include <srcMLUtility.hpp>
#include <LineDiffRange.hpp>

#include <xmlrw.hpp>

#include <strings.h>
#include <cstring>

// constructor
srcDiffTranslator::srcDiffTranslator(const char* srcdiff_filename,
                                     METHOD_TYPE method,
                                     std::string css,
                                     srcml_archive * archive,
                                     const char * url,
                                     OPTION_TYPE & options,
                                     unsigned long number_context_lines)
  : archive(archive), method(method), output(archive, srcdiff_filename, options, method, srcml_archive_get_prefix_from_uri(archive, SRCDIFF_DEFAULT_NAMESPACE_HREF), css, number_context_lines),
    url(url), options(options) {}

// Translate from input stream to output stream
void srcDiffTranslator::translate(const char * path_one, const char * path_two,
                                  const char * unit_directory, const char * unit_filename, const char * unit_version) {

  LineDiffRange line_diff_range(path_one, path_two, url, options);

  line_diff_range.create_line_diff();

  if(!isoption(options, OPTION_OUTPUTSAME) && line_diff_range.get_line_diff() == NULL)
    return;

  int is_old = 0;
  srcdiff_input input_old(archive, options);
  std::thread thread_old(input_old, path_one, SESDELETE, std::ref(output.get_nodes_old()), std::ref(is_old));

  int is_new = 0;
  srcdiff_input input_new(archive, options);
  std::thread thread_new(input_new, path_one, SESINSERT, std::ref(output.get_nodes_new()), std::ref(is_new));


  thread_old.join();
  thread_new.join();

  const char * path = path_one;
  if(path_one == 0 || path_one[0] == 0 || path_one[0] == '@')
    path = path_two;

  const char * language_string = "";
  if(isoption(options, OPTION_SVN)) {

    const char * end = index(path, '@');
    const char * filename = strndup(path, end - path);
    language_string = srcml_archive_check_extension(archive, filename);
    free((void *)filename);

  } else {

    language_string = srcml_archive_check_extension(archive, path);

  }

  node_sets set_old(output.get_nodes_old(), 0, output.get_nodes_old().size());
  node_sets set_new(output.get_nodes_new(), 0, output.get_nodes_new().size());

  output.initialize(is_old, is_new, language_string, unit_directory, unit_filename, unit_version);

  // run on file level
  if(is_old || is_new) {

    srcdiff_diff diff(output, &set_old, &set_new);
    diff.output();

    // output remaining whitespace
    srcdiff_whitespace whitespace(output);
    whitespace.output_all();

  }

  output.finish(is_old, is_new, line_diff_range);

  output.reset();

}

srcml_archive * srcDiffTranslator::get_archive() {

  return archive;

}

// destructor
srcDiffTranslator::~srcDiffTranslator() {

  output.close();

}

