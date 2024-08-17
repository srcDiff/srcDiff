// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_output.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_OUTPUT_HPP
#define INCLUDED_SRCDIFF_OUTPUT_HPP

#include <namespace.hpp>
#include <srcml_nodes.hpp>
#include <srcdiff_options.hpp>

#include <view.hpp>
#include <unified_view.hpp>
#include <side_by_side_view.hpp>

#include <methods.hpp>
#include <srcdiff_constants.hpp>
#include <shortest_edit_script.h>

#include <optional>

#include <vector>
#include <memory>
#include <string>

class srcdiff_output {

public:

  struct diff_set {

    int operation;

    srcml_nodes open_tags;

  };

  // stores information on state of a single file
  class reader_state {

  public:

    reader_state(int source) 
      : stream_source(source), last_output(0) { }

    void clear() {

      last_output = 0;
      nodes.clear();

      while(!open_diff.empty()) {

        delete open_diff.back();
        open_diff.pop_back();

      }

    }

    int stream_source;
    unsigned int last_output;

    srcml_nodes nodes;

    std::vector<diff_set *> open_diff;

  };

  // stores information during xml Text Writer processing
  class writer_state {

  public:

    writer_state(const METHOD_TYPE & method) : method(method), approximate(false) {}

    void clear() {

      while(!output_diff.empty()) {

        delete output_diff.back();
        output_diff.pop_back();

      }

      approximate = false;

    }

    std::string filename;
    srcml_unit * unit;

    const METHOD_TYPE & method;
    bool approximate;
    
    std::vector<diff_set *> output_diff;

  };

protected:

  bool output_srcdiff;
  srcml_archive * archive;
  OPTION_TYPE flags;

  std::shared_ptr<reader_state> rbuf_original;
  std::shared_ptr<reader_state> rbuf_modified;
  std::shared_ptr<writer_state> wstate;

  std::shared_ptr<view_t> view;

public:

  // diff nodes
  std::shared_ptr<srcML::node> diff_common_start;
  std::shared_ptr<srcML::node> diff_common_end;
  std::shared_ptr<srcML::node> diff_original_start;
  std::shared_ptr<srcML::node> diff_original_end;
  std::shared_ptr<srcML::node> diff_modified_start;
  std::shared_ptr<srcML::node> diff_modified_end;

  std::shared_ptr<srcML::node> diff_ws_start;
  std::shared_ptr<srcML::node> diff_ws_end;

  std::shared_ptr<srcML::name_space> diff;

  std::shared_ptr<srcML::node> unit_tag;

  static bool delay;
  static int delay_operation;

private:

  static void update_diff_stack(std::vector<diff_set *> & open_diffs, const std::shared_ptr<srcML::node> & node, int operation);
  void update_diff_stacks(const std::shared_ptr<srcML::node> & node, int operation);
  void output_node(const srcML::node & node);
  void output_node_inner(const srcML::node & node);

public:

  srcdiff_output() {}
  srcdiff_output(srcml_archive * archive,
                 const std::string & srcdiff_filename,
                 const OPTION_TYPE & flags,
                 const METHOD_TYPE & method,
                 const srcdiff_options::view_options_t & view_options,
                 const std::optional<std::string> & summary_type_str);
  virtual ~srcdiff_output();

  virtual void initialize(int is_original, int is_modified);
  virtual void start_unit(const std::string & language_string, const std::optional<std::string> & unit_filename, const std::optional<std::string> & unit_version);

  void finish();
  virtual void reset();
  virtual void close();


  virtual const std::string & srcdiff_filename() const;
  virtual const srcml_nodes & nodes_original() const;
  virtual const srcml_nodes & nodes_modified() const;
  virtual srcml_nodes & nodes_original();
  virtual srcml_nodes & nodes_modified();
  unsigned int last_output_original() const;
  unsigned int last_output_modified() const;
  virtual unsigned int & last_output_original();
  virtual unsigned int & last_output_modified();
  virtual int output_state() const;
  METHOD_TYPE method() const;

  void approximate(bool is_approximate);

  virtual bool is_delay_type(int operation);

  virtual void output_node(const std::shared_ptr<srcML::node> & original_node, 
                           const std::shared_ptr<srcML::node> & modified_node,
                           int operation, bool force_output = false);
  virtual void output_node(const std::shared_ptr<srcML::node> & node, int operation, bool force_output = false);
  virtual void output_text_as_node(const std::string & text, int operation);
  virtual void output_char(char character, int operation);

};

#endif
