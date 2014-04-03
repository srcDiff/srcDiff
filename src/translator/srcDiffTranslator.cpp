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

// constructor
srcDiffTranslator::srcDiffTranslator(const char* srcdiff_filename,
                                     METHOD_TYPE method,
                                     std::string css,
                                     srcml_archive * archive)
  : method(method), archive(archive), rbuf_old(SESDELETE), rbuf_new(SESINSERT), colordiff(NULL)
{
  diff.prefix = srcml_archive_get_prefix_from_uri(archive, diff.href);

  srcml_write_open_filename(archive, srcdiff_filename);
  srcml_archive_register_namespace(archive, diff.prefix, diff.href);

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
  wstate.writer = NULL;

  // writer state
  if(isoption(srcml_archive_get_options(archive), OPTION_VISUALIZE)) {

    std::string dir = "";
    if(srcml_archive_get_directory(archive) != NULL)
      dir = srcml_archive_get_directory(archive);

    std::string ver = "";
    if(srcml_archive_get_version(archive) != NULL)
      ver = srcml_archive_get_version(archive);

    colordiff = new ColorDiff(xmlBufferCreate(), srcdiff_filename, dir, ver, css, srcml_archive_get_options(archive));

  }

  wstate.method = method;


}

// Translate from input stream to output stream
void srcDiffTranslator::translate(const char* path_one, const char* path_two,
                                  const char* unit_directory, const char* unit_filename, const char* unit_version) {

  LineDiffRange line_diff_range(path_one, path_two);

  line_diff_range.create_line_diff();

  if(!isoption(srcml_archive_get_options(archive), OPTION_OUTPUTSAME) && line_diff_range.get_line_diff() == NULL)
    return;

  // create the reader for the old file
  xNodePtr unit_old = 0;
  NodeSets node_set_old;

  int is_old = 0;
  create_nodes_args args_old = { path_one, unit_directory, unit_filename, unit_version, archive
                                 , rbuf_old.mutex
                                 , rbuf_old.nodes, &unit_old, is_old, rbuf_old.stream_source };
  pthread_t thread_old;
  if(pthread_create(&thread_old, NULL, create_nodes_from_srcML_thread, (void *)&args_old)) {

    is_old = -2;

  }

  if(!isoption(srcml_archive_get_options(archive), OPTION_THREAD) && is_old != -2 && pthread_join(thread_old, NULL)) {

    is_old = -2;

  }

  /*

    Input for file two

  */

  xNodePtr unit_new = 0;
  NodeSets node_set_new;

  int is_new = 0;
  create_nodes_args args_new = { path_two, unit_directory, unit_filename, unit_version, archive
                                 , rbuf_new.mutex
                                 , rbuf_new.nodes, &unit_new, is_new, rbuf_new.stream_source };

  pthread_t thread_new;
  if(pthread_create(&thread_new, NULL, create_nodes_from_srcML_thread, (void *)&args_new)) {

    is_new = -2;

  }

  if(isoption(srcml_archive_get_options(archive), OPTION_THREAD) && is_old != -2 && pthread_join(thread_old, NULL)) {

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

    update_diff_stack(rbuf_old.open_diff, unit_old, SESCOMMON);
    update_diff_stack(rbuf_new.open_diff, unit_new, SESCOMMON);
    update_diff_stack(wstate.output_diff, unit_old, SESCOMMON);

  } else if(rbuf_old.nodes.empty() && rbuf_new.nodes.empty()) {

    update_diff_stack(rbuf_old.open_diff, &diff_common_start, SESCOMMON);
    update_diff_stack(rbuf_new.open_diff, &diff_common_start, SESCOMMON);
    update_diff_stack(wstate.output_diff, &diff_common_start, SESCOMMON);

    if(is_old <= -1 && is_new <= -1) {

      fprintf(stderr, "Error with file '%s' and file '%s'\n", path_one, path_two);

      exit(STATUS_INPUTFILE_PROBLEM);

    }

  } else if(rbuf_old.nodes.empty()) {

    if(!isoption(srcml_archive_get_options(archive), OPTION_OUTPUTPURE)) {

      is_old = 0;
      is_new = 0;

    }

    update_diff_stack(rbuf_old.open_diff, &diff_common_start, SESCOMMON);
    update_diff_stack(rbuf_new.open_diff, unit_new, SESCOMMON);
    update_diff_stack(wstate.output_diff, unit_new, SESCOMMON);

  } else {

    if(!isoption(srcml_archive_get_options(archive), OPTION_OUTPUTPURE)) {

      is_old = 0;
      is_new = 0;

    }

    update_diff_stack(rbuf_old.open_diff, unit_old, SESCOMMON);
    update_diff_stack(rbuf_new.open_diff, &diff_common_start, SESCOMMON);
    update_diff_stack(wstate.output_diff, unit_old, SESCOMMON);

  }


  // run on file level
  if(is_old || is_new) {

    if(!isoption(srcml_archive_get_options(archive), OPTION_VISUALIZE)) {

      wstate.buffer = xmlBufferCreate();
      wstate.writer = xmlNewTextWriterMemory(wstate.buffer, 0);

    }

/*
     else {

      if(!line_diff_range.is_no_white_space_diff())
        return;

      wstate.writer = xmlNewTextWriterMemory(colordiff->getsrcDiffBuffer(), 0);

    }
*/


    if (wstate.writer == NULL) {

        fprintf(stderr, "Unable to open file '%s' for XML\n", path_one);

        exit(1);

    }

    output_node(rbuf_old, rbuf_new, unit_old, SESCOMMON, wstate);

    output_diffs(rbuf_old, &node_set_old, rbuf_new, &node_set_new, wstate);

    // output remaining whitespace
    output_white_space_all(rbuf_old, rbuf_new, wstate);

    // output srcdiff unit ending tag
    //if(is_old && is_new)
    //output_node(rbuf_old, rbuf_new, unit_end, SESCOMMON, wstate);

    output_node(rbuf_old, rbuf_new, &flush, SESCOMMON, wstate);

    xmlTextWriterEndDocument(wstate.writer);
    xmlFreeTextWriter(wstate.writer);

    srcml_unit * unit = srcml_create_unit(archive);
    if(wstate.buffer->use) wstate.buffer->content[wstate.buffer->use - 1] = '\0';
    srcml_unit_set_xml(unit, (const char *)wstate.buffer->content);
    srcml_write_unit(archive, unit);

    // }

/*
    if(!isoption(global_options, OPTION_VISUALIZE) && isoption(global_options, OPTION_ARCHIVE)) {

      xmlTextWriterEndElement(wstate.writer);
      xmlTextWriterWriteRawLen(wstate.writer, BAD_CAST "\n\n", 2);

    }
*/

  }

  free_node_sets(node_set_old);
  free_node_sets(node_set_new);

  if(unit_old && unit_old->free)
    freeXNode(unit_old);

  if(unit_new && unit_new->free)
    freeXNode(unit_new);

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

  if(isoption(srcml_archive_get_options(archive), OPTION_VISUALIZE)) {

    xmlTextWriterEndElement(wstate.writer);

    // cleanup writer
    xmlTextWriterEndDocument(wstate.writer);
    xmlFreeTextWriter(wstate.writer);
    wstate.writer = NULL;

    colordiff->colorize(line_diff_range);

    xmlBufferEmpty(colordiff->getsrcDiffBuffer());

  }

}

// destructor
srcDiffTranslator::~srcDiffTranslator() {

  if(!isoption(srcml_archive_get_options(archive), OPTION_VISUALIZE)) {

    srcml_close_archive(archive);

  } else {

    xmlBufferFree(colordiff->getsrcDiffBuffer());
    delete colordiff;

  }

  pthread_mutex_destroy(&mutex);

}

