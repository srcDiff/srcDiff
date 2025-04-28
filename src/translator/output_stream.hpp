// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file output_stream.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_OUTPUT_STREAM_HPP
#define INCLUDED_OUTPUT_STREAM_HPP

#include <namespace.hpp>
#include <srcml_nodes.hpp>
#include <client_options.hpp>

#include <methods.hpp>
#include <constants.hpp>
#include <operation.hpp>

#include <optional>

#include <vector>
#include <memory>
#include <string>

namespace srcdiff {

class output_stream {

public:

  struct diff_set {

    enum operation operation;

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

  srcml_archive* archive;
  OPTION_TYPE flags;

  std::shared_ptr<reader_state> rbuf_original;
  std::shared_ptr<reader_state> rbuf_modified;
  std::shared_ptr<writer_state> wstate;

  bool is_initialized;
  bool is_open;

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

  std::shared_ptr<srcML::node> unit_tag;

  static bool delay;
  static enum operation delay_operation;

private:

  static void update_diff_stack(std::vector<diff_set *> & open_diffs, const std::shared_ptr<srcML::node> & node, enum operation operation);
  void update_diff_stacks(const std::shared_ptr<srcML::node> & node, enum operation operation);
  void output_node(const srcML::node & node);
  void output_node_inner(const srcML::node & node);

public:

  output_stream() {}
  output_stream(srcml_archive * archive,
                 const std::string & srcdiff_filename,
                 const OPTION_TYPE & flags,
                 const METHOD_TYPE & method,
                 const client_options::view_options_t & view_options,
                 const std::optional<std::string> & summary_type_str);
  virtual ~output_stream();


  void initialize();

  void prime(int is_original, int is_modified);
  void start_unit(const std::string & language_string, const std::optional<std::string> & unit_filename, const std::optional<std::string> & unit_version);

  std::string end_unit();
  void write_unit();

  void reset();
  void close();

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

  virtual bool is_delay_type(enum operation operation);

  virtual void output_node(const std::shared_ptr<srcML::node> & original_node, 
                           const std::shared_ptr<srcML::node> & modified_node,
                           enum operation operation, bool force_output = false);
  virtual void output_node(const std::shared_ptr<srcML::node> & node, enum operation operation, bool force_output = false);
  virtual void output_text_as_node(const std::string & text, enum operation operation);
  virtual void output_char(char character, enum operation operation);

};

}

#endif
