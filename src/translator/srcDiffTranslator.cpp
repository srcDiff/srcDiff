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
#include <srcmlns.hpp>
#include <srcmlapps.hpp>
#include <shortest_edit_script.h>
#include <srcDiffConstants.hpp>
#include <srcDiffUtility.hpp>
#include <srcDiffWhiteSpace.hpp>
#include <srcDiffCommon.hpp>
#include <srcDiffChange.hpp>
#include <srcDiffOutput.hpp>
#include <srcDiffDiff.hpp>
#include <srcMLUtility.hpp>
#include <LineDiffRange.hpp>

#include <xmlrw.hpp>

#include <strings.h>
#include <cstring>

// diff nodes
xNode diff_common_start;
xNode diff_common_end;
xNode diff_old_start;
xNode diff_old_end;
xNode diff_new_start;
xNode diff_new_end;

xNs diff = {"http://www.sdml.info/srcDiff", "diff"};

// diff attribute
xAttr diff_type = { 0 };

xNode unit_tag = { (xmlElementType)XML_READER_TYPE_ELEMENT, "unit", 0, 0, 0, 0, 0, false, false, 0, 0 };

// constructor
srcDiffTranslator::srcDiffTranslator(const char* srcdiff_filename,
                                     METHOD_TYPE method,
                                     std::string css,
                                     srcml_archive * archive,
                                     const char * url,
                                     OPTION_TYPE & options,
                                     unsigned long number_context_lines)
  : method(method), archive(archive), rbuf_old(SESDELETE), rbuf_new(SESINSERT), colordiff(NULL), bashview(NULL), url(url), options(options)
{
  diff.prefix = srcml_archive_get_prefix_from_uri(archive, diff.href);

  if(!isoption(options, OPTION_VISUALIZE) && !isoption(options, OPTION_BASH_VIEW))
    srcml_write_open_filename(archive, srcdiff_filename);

  // diff tags
  diff_common_start.name = DIFF_SESCOMMON;
  diff_common_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_common_start.ns = &diff;
  diff_common_start.extra = 0;

  diff_common_end.name = DIFF_SESCOMMON;
  diff_common_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_common_end.ns = &diff;
  diff_common_end.extra = 0;

  diff_old_start.name = DIFF_OLD;
  diff_old_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_old_start.ns = &diff;
  diff_old_start.extra = 0;

  diff_old_end.name = DIFF_OLD;
  diff_old_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_old_end.ns = &diff;
  diff_old_end.extra = 0;

  diff_new_start.name = DIFF_NEW;
  diff_new_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_new_start.ns = &diff;
  diff_new_start.extra = 0;

  diff_new_end.name = DIFF_NEW;
  diff_new_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_new_end.ns = &diff;
  diff_new_end.extra = 0;

  diff_type.name = DIFF_TYPE;
  //diff_type.type = (xmlElementType)XML_ATTRIBUTE_NODE;

  pthread_mutex_init(&mutex, 0);

  rbuf_old.mutex = &mutex;
  rbuf_new.mutex = &mutex;

  wstate.filename = srcdiff_filename;

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

  wstate.method = method;


}

// Translate from input stream to output stream
void srcDiffTranslator::translate(const char* path_one, const char* path_two,
                                  const char* unit_directory, const char* unit_filename, const char* unit_version) {

  LineDiffRange line_diff_range(path_one, path_two, url, options);

  line_diff_range.create_line_diff();

  if(!isoption(options, OPTION_OUTPUTSAME) && line_diff_range.get_line_diff() == NULL)
    return;


  // create the reader for the old file
  NodeSets node_set_old;

  int is_old = 0;
  create_nodes_args args_old = { path_one, archive
                                , srcml_archive_check_extension(archive, path_one ? path_one : path_two)
                                , unit_filename
                                , unit_directory
                                , unit_version
                                , rbuf_old.mutex
                                , rbuf_old.nodes
                                , is_old
                                , rbuf_old.stream_source
                                , options };
  pthread_t thread_old;
  if(pthread_create(&thread_old, NULL, create_nodes_from_srcML_thread, (void *)&args_old)) {

    is_old = -2;

  }

  if(!isoption(options, OPTION_THREAD) && is_old != -2 && pthread_join(thread_old, NULL)) {

    is_old = -2;

  }

  /*

    Input for file two

  */

  NodeSets node_set_new;

  int is_new = 0;
  create_nodes_args args_new = { path_two, archive
                                , srcml_archive_check_extension(archive, path_one ? path_one : path_two)
                                , unit_filename
                                , unit_directory
                                , unit_version
                                , rbuf_new.mutex
                                , rbuf_new.nodes
                                , is_new
                                , rbuf_new.stream_source
                                , options };


  pthread_t thread_new;
  if(pthread_create(&thread_new, NULL, create_nodes_from_srcML_thread, (void *)&args_new)) {

    is_new = -2;

  }

  if(isoption(options, OPTION_THREAD) && is_old != -2 && pthread_join(thread_old, NULL)) {

    is_old = -2;

  }

  if(is_new != -2 && pthread_join(thread_new, NULL)) {

    is_new = -2;

  }

  if(is_old && is_old > -1)
    node_set_old = create_node_set(rbuf_old.nodes, 0, rbuf_old.nodes.size());


  if(is_new && is_new > -1)
    node_set_new = create_node_set(rbuf_new.nodes, 0, rbuf_new.nodes.size());

  /*

    Setup readers and writer.

  */

  diff_set old_diff;
  old_diff.operation = SESCOMMON;
  rbuf_old.open_diff.push_back(&old_diff);

  diff_set new_diff;
  new_diff.operation = SESCOMMON;
  rbuf_new.open_diff.push_back(&new_diff);

  diff_set output_diff;
  output_diff.operation = SESCOMMON;
  wstate.output_diff.push_back(&output_diff);

  /*

    Output srcDiff

  */

  // output srcdiff unit
  if(!rbuf_old.nodes.empty() && !rbuf_new.nodes.empty()) {

    update_diff_stack(rbuf_old.open_diff, &unit_tag, SESCOMMON);
    update_diff_stack(rbuf_new.open_diff, &unit_tag, SESCOMMON);
    update_diff_stack(wstate.output_diff, &unit_tag, SESCOMMON);

  } else if(rbuf_old.nodes.empty() && rbuf_new.nodes.empty()) {

    update_diff_stack(rbuf_old.open_diff, &diff_common_start, SESCOMMON);
    update_diff_stack(rbuf_new.open_diff, &diff_common_start, SESCOMMON);
    update_diff_stack(wstate.output_diff, &diff_common_start, SESCOMMON);

    if(is_old <= -1 && is_new <= -1) {

      fprintf(stderr, "Error with file '%s' and file '%s'\n", path_one, path_two);

      exit(STATUS_INPUTFILE_PROBLEM);

    }

  } else if(rbuf_old.nodes.empty()) {

    if(!isoption(options, OPTION_OUTPUTPURE)) {

      is_old = 0;
      is_new = 0;

    }

    update_diff_stack(rbuf_old.open_diff, &diff_common_start, SESCOMMON);
    update_diff_stack(rbuf_new.open_diff, &unit_tag, SESCOMMON);
    update_diff_stack(wstate.output_diff, &unit_tag, SESCOMMON);

  } else {

    if(!isoption(options, OPTION_OUTPUTPURE)) {

      is_old = 0;
      is_new = 0;

    }

    update_diff_stack(rbuf_old.open_diff, &unit_tag, SESCOMMON);
    update_diff_stack(rbuf_new.open_diff, &diff_common_start, SESCOMMON);
    update_diff_stack(wstate.output_diff, &unit_tag, SESCOMMON);

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

    wstate.unit = srcdiff_unit;

    /** @todo when output non-archive additional namespaces not appended, because not collected 
      However this is correct when output is to archive */
    srcml_write_start_unit(srcdiff_unit);

    srcdiff_diff diff(rbuf_old, rbuf_new, wstate, &node_set_old, &node_set_new);
    diff.output();

    // output remaining whitespace
    output_white_space_all(rbuf_old, rbuf_new, wstate);

    output_node(rbuf_old, rbuf_new, (xNodePtr)&flush, SESCOMMON, wstate);

    srcml_write_end_unit(srcdiff_unit);

    if(!isoption(options, OPTION_VISUALIZE)) {

      srcml_write_unit(archive, srcdiff_unit);

    }

  }

  free_node_sets(node_set_old);
  free_node_sets(node_set_new);

  // Because of grouping need to output a common to end grouping need to deallocate as well
  for(unsigned int i = 0; i < rbuf_old.nodes.size(); ++i) {

    if(rbuf_old.nodes.at(i)->free) {

      freeXNode(rbuf_old.nodes.at(i));

    }
  }

  for(unsigned int i = 0; i < rbuf_new.nodes.size(); ++i) {

    if(rbuf_new.nodes.at(i)->free) {

      freeXNode(rbuf_new.nodes.at(i));

    }
  }

  rbuf_old.clear();
  rbuf_new.clear();
  wstate.clear();

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

  pthread_mutex_destroy(&mutex);

}

