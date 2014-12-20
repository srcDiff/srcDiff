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
  : method(method), archive(archive), output(srcdiff_filename, method, srcml_archive_get_prefix_from_uri(archive, SRCDIFF_DEFAULT_NAMESPACE_HREF)),
    colordiff(NULL), bashview(NULL), url(url), options(options)
{

  if(!isoption(options, OPTION_VISUALIZE) && !isoption(options, OPTION_BASH_VIEW))
    srcml_write_open_filename(archive, srcdiff_filename);

  // writer state
  if(isoption(options, OPTION_VISUALIZE)) {

    std::string dir = "";
    if(srcml_archive_get_directory(archive) != NULL)
      dir = srcml_archive_get_directory(archive);

    std::string ver = "";
    if(srcml_archive_get_version(archive) != NULL)
      ver = srcml_archive_get_version(archive);

    colordiff = new ColorDiff(srcdiff_filename, dir, ver, css, options);

  } else if(isoption(options, OPTION_BASH_VIEW))
      bashview = new bash_view(srcdiff_filename, number_context_lines);

}

// Translate from input stream to output stream
void srcDiffTranslator::translate(const char* path_one, const char* path_two,
                                  const char* unit_directory, const char* unit_filename, const char* unit_version) {

  LineDiffRange line_diff_range(path_one, path_two, url, options);

  line_diff_range.create_line_diff();

  if(!isoption(options, OPTION_OUTPUTSAME) && line_diff_range.get_line_diff() == NULL)
    return;

  int is_old = 0;
  std::vector<xNodePtr> nodes_old;
  srcdiff_input input_old(archive, options);
  std::thread thread_old(input_old, path_one, SESDELETE, nodes_old, is_old);

  int is_new = 0;
  std::vector<xNodePtr> nodes_new;
  srcdiff_input input_new(archive, options);
  std::thread thread_new(input_nodes, path_one, SESINSERT, nodes_new, is_new);

  /*

    Setup readers and writer.

  */

  diff_set old_diff;
  old_diff.operation = SESCOMMON;
  output.get_rbuf_old().open_diff.push_back(&old_diff);

  diff_set new_diff;
  new_diff.operation = SESCOMMON;
  output.get_rbuf_new().open_diff.push_back(&new_diff);

  diff_set output_diff;
  output_diff.operation = SESCOMMON;
  output.get_wstate().output_diff.push_back(&output_diff);

  /*

    Output srcDiff

  */

  // output srcdiff unit
  if(!output.get_rbuf_old().nodes.empty() && !output.get_rbuf_new().nodes.empty()) {

    srcdiff_output::update_diff_stack(output.get_rbuf_old().open_diff, output.unit_tag.get(), SESCOMMON);
    srcdiff_output::update_diff_stack(output.get_rbuf_new().open_diff, output.unit_tag.get(), SESCOMMON);
    srcdiff_output::update_diff_stack(output.get_wstate().output_diff, output.unit_tag.get(), SESCOMMON);

  } else if(output.get_rbuf_old().nodes.empty() && output.get_rbuf_new().nodes.empty()) {

    srcdiff_output::update_diff_stack(output.get_rbuf_old().open_diff, output.diff_common_start.get(), SESCOMMON);
    srcdiff_output::update_diff_stack(output.get_rbuf_new().open_diff, output.diff_common_start.get(), SESCOMMON);
    srcdiff_output::update_diff_stack(output.get_wstate().output_diff, output.diff_common_start.get(), SESCOMMON);

    if(is_old <= -1 && is_new <= -1) {

      fprintf(stderr, "Error with file '%s' and file '%s'\n", path_one, path_two);

      exit(STATUS_INPUTFILE_PROBLEM);

    }

  } else if(output.get_rbuf_old().nodes.empty()) {

    if(!isoption(options, OPTION_OUTPUTPURE)) {

      is_old = 0;
      is_new = 0;

    }

    srcdiff_output::update_diff_stack(output.get_rbuf_old().open_diff, output.diff_common_start.get(), SESCOMMON);
    srcdiff_output::update_diff_stack(output.get_rbuf_new().open_diff, output.unit_tag.get(), SESCOMMON);
    srcdiff_output::update_diff_stack(output.get_wstate().output_diff, output.unit_tag.get(), SESCOMMON);

  } else {

    if(!isoption(options, OPTION_OUTPUTPURE)) {

      is_old = 0;
      is_new = 0;

    }

    srcdiff_output::update_diff_stack(output.get_rbuf_old().open_diff, output.unit_tag.get(), SESCOMMON);
    srcdiff_output::update_diff_stack(output.get_rbuf_new().open_diff, output.diff_common_start.get(), SESCOMMON);
    srcdiff_output::update_diff_stack(output.get_wstate().output_diff, output.unit_tag.get(), SESCOMMON);

  }

  srcml_unit * srcdiff_unit = srcml_create_unit(archive);

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

  srcml_unit_set_language(srcdiff_unit, language_string);


  srcml_archive_get_filename(archive) ? srcml_unit_set_filename(srcdiff_unit, srcml_archive_get_filename(archive)) : srcml_unit_set_filename(srcdiff_unit, unit_filename);
  srcml_unit_set_directory(srcdiff_unit, unit_directory);
  srcml_unit_set_version(srcdiff_unit, unit_version);

  // run on file level
  if(is_old || is_new) {

    output.get_wstate().unit = srcdiff_unit;

    /** @todo when output non-archive additional namespaces not appended, because not collected 
      However this is correct when output is to archive */
    srcml_write_start_unit(srcdiff_unit);

    srcdiff_diff diff(output, &set_old, &set_new);
    diff.output();

    // output remaining whitespace
    srcdiff_whitespace whitespace(output);
    whitespace.output_all();

    output.flush();

    srcml_write_end_unit(srcdiff_unit);

    if(!isoption(options, OPTION_VISUALIZE)) {

      srcml_write_unit(archive, srcdiff_unit);

    }

  }

  output.reset();

  if(isoption(options, OPTION_VISUALIZE)) {

    if(is_old || is_new)
      colordiff->colorize(srcml_unit_get_xml(srcdiff_unit), line_diff_range);

  } else if(isoption(options, OPTION_BASH_VIEW)) {

    bashview->transform(srcml_unit_get_xml(srcdiff_unit));
  }

  srcml_free_unit(srcdiff_unit);

}

srcml_archive * srcDiffTranslator::get_archive() {

  return archive;

}

// destructor
srcDiffTranslator::~srcDiffTranslator() {

  if(!isoption(options, OPTION_VISUALIZE) && !isoption(options, OPTION_BASH_VIEW)) {

    srcml_close_archive(archive);

  } else {

    if(colordiff) delete colordiff;

    if(bashview) delete bashview;

  }

}

