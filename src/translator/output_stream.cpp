// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file output_stream.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <output_stream.hpp>

#include <constants.hpp>
#include <shortest_edit_script.h>
#include <methods.hpp>

#include <list>

#include <cstring>

namespace srcdiff {

bool output_stream::delay = false;
enum operation output_stream::delay_operation = NONE;

// summary_type_str is unused here
output_stream::output_stream(srcml_archive * archive, 
                               const std::string & srcdiff_filename,
                               const OPTION_TYPE & flags,
                               const METHOD_TYPE & method,
                               const client_options::view_options_t & view_options,
                               const std::optional<std::string> & summary_type_str [[maybe_unused]])
 : archive(archive), flags(flags),
   rbuf_original(std::make_shared<reader_state>(DELETE)), rbuf_modified(std::make_shared<reader_state>(INSERT)), wstate(std::make_shared<writer_state>(method)),
   diff(std::make_shared<srcML::name_space>()),
   is_initialized(false), is_open(false) {

  wstate->filename = srcdiff_filename;

}

void output_stream::initialize() {

  diff->set_prefix(srcml_archive_get_prefix_from_uri(archive, SRCDIFF_DEFAULT_NAMESPACE_HREF.c_str()));
  diff->set_uri(SRCDIFF_DEFAULT_NAMESPACE_HREF);

  unit_tag            = std::make_shared<srcML::node>(srcML::node_type::START, std::string("unit"));
  diff_common_start   = std::make_shared<srcML::node>(srcML::node_type::START, DIFF_COMMON, srcML::name_space::DIFF_NAMESPACE);
  diff_common_end     = std::make_shared<srcML::node>(srcML::node_type::END, DIFF_COMMON, srcML::name_space::DIFF_NAMESPACE);
  diff_original_start = std::make_shared<srcML::node>(srcML::node_type::START, DIFF_ORIGINAL, srcML::name_space::DIFF_NAMESPACE);
  diff_original_end   = std::make_shared<srcML::node>(srcML::node_type::END, DIFF_ORIGINAL, srcML::name_space::DIFF_NAMESPACE);
  diff_modified_start = std::make_shared<srcML::node>(srcML::node_type::START, DIFF_MODIFIED, srcML::name_space::DIFF_NAMESPACE);
  diff_modified_end   = std::make_shared<srcML::node>(srcML::node_type::END, DIFF_MODIFIED, srcML::name_space::DIFF_NAMESPACE);
  
  diff_ws_start = std::make_shared<srcML::node>(srcML::node_type::START, DIFF_WHITESPACE, srcML::name_space::DIFF_NAMESPACE);
  diff_ws_end   = std::make_shared<srcML::node>(srcML::node_type::END, DIFF_WHITESPACE, srcML::name_space::DIFF_NAMESPACE);

  is_initialized = true;
}

output_stream::~output_stream() {}

void output_stream::prime(int is_original, int is_modified) {

  if(!is_initialized) initialize();

  diff_set * original_diff = new diff_set();
  original_diff->operation = COMMON;
  rbuf_original->open_diff.push_back(original_diff);

  diff_set * modified_diff = new diff_set();
  modified_diff->operation = COMMON;
  rbuf_modified->open_diff.push_back(modified_diff);

  diff_set * output_diff = new diff_set();
  output_diff->operation = COMMON;
  wstate->output_diff.push_back(output_diff);

  if(!rbuf_original->nodes.empty() && !rbuf_modified->nodes.empty()) {

    update_diff_stack(rbuf_original->open_diff, unit_tag, COMMON);
    update_diff_stack(rbuf_modified->open_diff, unit_tag, COMMON);
    update_diff_stack(wstate->output_diff, unit_tag, COMMON);

  } else if(rbuf_original->nodes.empty() && rbuf_modified->nodes.empty()) {

    update_diff_stack(rbuf_original->open_diff, diff_common_start, COMMON);
    update_diff_stack(rbuf_modified->open_diff, diff_common_start, COMMON);
    update_diff_stack(wstate->output_diff, diff_common_start, COMMON);

    if(is_original <= -1 && is_modified <= -1) {

      fprintf(stderr, "Error with files\n");

      exit(1);

    }

  } else if(rbuf_original->nodes.empty()) {

    update_diff_stack(rbuf_original->open_diff, diff_common_start, COMMON);
    update_diff_stack(rbuf_modified->open_diff, unit_tag, COMMON);
    update_diff_stack(wstate->output_diff, unit_tag, COMMON);

  } else {

    update_diff_stack(rbuf_original->open_diff, unit_tag, COMMON);
    update_diff_stack(rbuf_modified->open_diff, diff_common_start, COMMON);
    update_diff_stack(wstate->output_diff, unit_tag, COMMON);

  }

}

void output_stream::reset() {

  rbuf_original->clear();
  rbuf_modified->clear();
  wstate->clear();

}

void output_stream::start_unit(const std::string & language_string, const std::optional<std::string> & unit_filename, const std::optional<std::string> & unit_version) {

  srcml_unit_free(wstate->unit);
  wstate->unit = srcml_unit_create(archive);
  srcml_unit_set_language(wstate->unit, language_string.c_str());

  srcml_unit_set_filename(wstate->unit, unit_filename ? unit_filename->c_str() : 0);
  srcml_unit_set_version(wstate->unit, unit_version ? unit_version->c_str() : 0);
  /** @todo when output non-archive additional namespaces not appended, because not collected 
    However this is correct when output is to archive */
  srcml_write_start_unit(wstate->unit);

}

std::string output_stream::end_unit() {

  static const std::shared_ptr<srcML::node> flush = std::make_shared<srcML::node>(srcML::node_type::TEXT, "text");
  output_node(flush, COMMON);

  if(wstate->approximate) {
    srcml_write_start_element(wstate->unit, SRCDIFF_DEFAULT_NAMESPACE_PREFIX.c_str(), "approximate", 0);
    srcml_write_end_element(wstate->unit);
  }

  srcml_write_end_unit(wstate->unit);

  std::string srcdiff = srcml_unit_get_srcml(wstate->unit);

  return srcdiff;

}

void output_stream::write_unit() {
  if(!is_open) {
    int ret_status = srcml_archive_write_open_filename(archive, wstate->filename.c_str());
    if(ret_status != SRCML_STATUS_OK) throw std::string("Output source '" + wstate->filename + "' could not be opened");
  }

  is_open = true;
  srcml_archive_write_unit(archive, wstate->unit);
}

void output_stream::close() {
  srcml_unit_free(wstate->unit);
  if(is_open) srcml_archive_close(archive);
}

const std::string & output_stream::srcdiff_filename() const {
  return wstate->filename;
}

const srcml_nodes & output_stream::nodes_original() const {
  return rbuf_original->nodes;
}

const srcml_nodes & output_stream::nodes_modified() const {
  return rbuf_modified->nodes;
}

srcml_nodes & output_stream::nodes_original() {
  return rbuf_original->nodes;
}

srcml_nodes & output_stream::nodes_modified() {
  return rbuf_modified->nodes;
}

unsigned int output_stream::last_output_original() const {
  return rbuf_original->last_output;

}

unsigned int output_stream::last_output_modified() const {
  return rbuf_modified->last_output;
}

unsigned int & output_stream::last_output_original() {
  return rbuf_original->last_output;
}

unsigned int & output_stream::last_output_modified() {
  return rbuf_modified->last_output;
}

int output_stream::output_state() const {
  return wstate->output_diff.back()->operation;
}

METHOD_TYPE output_stream::method() const {
  return wstate->method;
}

void output_stream::approximate(bool is_approximate) {
  wstate->approximate = is_approximate;
}


bool output_stream::is_delay_type(enum operation operation) {
  if(!delay) return false;
  return operation == delay_operation;
}

void output_stream::update_diff_stack(std::vector<diff_set *> & open_diffs, const std::shared_ptr<srcML::node> & node, enum operation operation) {

  // Skip empty node
  if(node->is_empty() || node->is_text())
    return;

  if(open_diffs.back()->operation != operation) {

    diff_set * modified_diff = new diff_set;
    modified_diff->operation = operation;

    open_diffs.push_back(modified_diff);
  }

  if(node->get_type() == srcML::node_type::START) {

    open_diffs.back()->open_tags.push_back(node);
  } else if(node->get_type() == srcML::node_type::END) {

    if(open_diffs.size() == 1 && open_diffs.back()->open_tags.size() == 1)
      return;

    open_diffs.back()->open_tags.pop_back();

  }

  if(open_diffs.back()->open_tags.size() == 0) {

    delete open_diffs.back();
    open_diffs.pop_back();

  }

}

void output_stream::update_diff_stacks(const std::shared_ptr<srcML::node> & node, enum operation operation) {

  if(operation == COMMON) {

    update_diff_stack(rbuf_original->open_diff, node, operation);
    update_diff_stack(rbuf_modified->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  }
  else if(operation == DELETE) {

    update_diff_stack(rbuf_original->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  } else if(operation == INSERT) {

    update_diff_stack(rbuf_modified->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  }

}

void output_stream::output_node(const std::shared_ptr<srcML::node> & original_node, 
                                 const std::shared_ptr<srcML::node> & modified_node,
                                 enum operation operation, bool force_output) {

  if(operation == COMMON && original_node->is_temporary() != modified_node->is_temporary()) {

    if(original_node->get_type() == srcML::node_type::END) {
      output_node(diff_common_end, COMMON);
    }

    if(original_node->is_temporary()) {

      if(modified_node->get_type() == srcML::node_type::START) {
        output_node(diff_modified_start, INSERT);
      }
      output_node(modified_node, INSERT, force_output);
      if(modified_node->get_type() == srcML::node_type::END) {
        output_node(diff_modified_end, INSERT);
      }

    } else {

      if(original_node->get_type() == srcML::node_type::START) {
        output_node(diff_original_start, DELETE);
      }
      output_node(original_node, DELETE, force_output);
      if(original_node->get_type() == srcML::node_type::END) {
        output_node(diff_original_end, DELETE);
      }

    }
    if(original_node->get_type() == srcML::node_type::START) {
      output_node(diff_common_start, COMMON);
    }
  } else {
    output_node(original_node, operation, force_output);
  }

}


void output_stream::output_node(const std::shared_ptr<srcML::node> & node, enum operation operation, bool force_output) {

  // check if delaying DELETE/INSERT/COMMON tag. should only stop if operation is different or not whitespace
  if(delay && (delay_operation != operation)
     && ((delay_operation == DELETE 
          && wstate->output_diff.back()->open_tags.back()->get_name() == diff_original_end->get_name())
         || (delay_operation == INSERT 
             && wstate->output_diff.back()->open_tags.back()->get_name() == diff_modified_end->get_name())
         || (delay_operation == COMMON 
             && wstate->output_diff.back()->open_tags.back()->get_name() == diff_common_end->get_name()))) {

    if(delay_operation == DELETE) {

      output_node(*diff_original_end);

      update_diff_stacks(diff_original_end, delay_operation);

    } else if(delay_operation == INSERT) {

      output_node(*diff_modified_end);

      update_diff_stacks(diff_modified_end, delay_operation);

    } else if(delay_operation == COMMON)  {

      output_node(*diff_common_end);

      update_diff_stacks(diff_common_end, delay_operation);

    }

    delay = false;
    delay_operation = NONE;

  } else if(delay) {

    delay = false;
    delay_operation = NONE;

  }

  if(node->get_type() == srcML::node_type::END) {

    if(node->get_type() == srcML::node_type::END && wstate->output_diff.back()->open_tags.back()->get_name() != node->get_name())
      return;

    // check if ending a DELETE/INSERT/COMMON tag. if so delay.
    if(ismethod(wstate->method, METHOD_GROUP) && !force_output && (*node == *diff_original_end || *node == *diff_modified_end || *node == *diff_common_end)) {


      delay = true;
      delay_operation = wstate->output_diff.back()->operation;
      return;

    } else {

      output_node(*node);

    }

    update_diff_stacks(node, wstate->output_diff.back()->operation);
    return;

  }

  if(node->get_type() == srcML::node_type::START) {

    int current_operation = wstate->output_diff.back()->operation;
    int size = wstate->output_diff.back()->open_tags.size();

    if(!force_output && size > 0 &&
       (   (*node == *diff_original_start && current_operation == DELETE)
        || (*node == *diff_modified_start && current_operation == INSERT)
        || (*node == *diff_common_start && current_operation == COMMON))) {

      return;
    }

  }

  // output non-text node and get next node
  output_node(*node);

  update_diff_stacks(node, operation);

}

void output_stream::output_text_as_node(const std::string & text, enum operation operation) {

  if(text.size() == 0) return;
  std::shared_ptr<srcML::node> node = std::make_shared<srcML::node>(srcML::node_type::TEXT, std::string("text"));
  node->set_content(text);

  output_node(node, operation);

}


void output_stream::output_char(char character, enum operation operation) {

  static char buf[2] = { 0 };
  buf[0] = character;

  output_text_as_node(buf, operation);
}

void output_stream::output_node(const srcML::node & node) {

  static bool delay_ws_end = false;

  if(delay_ws_end) {

    delay_ws_end = false;

    if(node == *diff_ws_start) return;
    else output_node_inner(*diff_ws_end);

  }

  if(node == *diff_ws_end) {

    delay_ws_end = true;
    return;

  }

  output_node_inner(node);

}

// output current XML node in reader
void output_stream::output_node_inner(const srcML::node & node) {

  if(node.is_temporary()) return;

  switch (node.get_type()) {
  case srcML::node_type::START:

    
    // start the element
    srcml_write_start_element(wstate->unit,
      node.get_namespace()->get_prefix() ? node.get_namespace()->get_prefix()->c_str() : 0,
      node.get_name().c_str(),
      node.get_namespace()->get_uri().c_str()
      );

    // copy all the attributes
    {

      for(srcML::attribute_map_cpair attr : node.get_attributes()) {
        srcml_write_attribute(wstate->unit,
                              attr.second.get_ns() && attr.second.get_ns()->get_prefix()? attr.second.get_ns()->get_prefix()->c_str() : 0, 
                              attr.second.get_name().c_str(), 
                              0, 
                              attr.second.get_value()? attr.second.get_value()->c_str() : 0);
      }

    }

    // end now if this is an empty element
    if (node.is_empty()) {
      srcml_write_end_element(wstate->unit);
    }

    break;

  case srcML::node_type::END:
    srcml_write_end_element(wstate->unit);
    break;

  case srcML::node_type::TEXT:

    // output the UTF-8 buffer escaping the characters.  Note that the output encoding
    // is handled by libxml
    srcml_write_string(wstate->unit, node.get_content() ? node.get_content()->c_str() : 0);

    break;

  default:
    break;
  }

}

}