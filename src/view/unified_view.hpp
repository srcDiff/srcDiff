// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file unified_view.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_UNIFIED_VIEW_HPP
#define INCLUDED_UNIFIED_VIEW_HPP

#include <view.hpp>

#include <any>
#include <vector>
#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>

class unified_view : public view_t {

private:

  typedef unsigned long context_mode;

  static const context_mode LINE     = 1 << 0;
  static const context_mode FUNCTION = 1 << 1;
  static const context_mode ALL      = 1 << 2;

  int last_character_operation;

  context_mode modes;

  size_t line_number_delete;
  size_t line_number_insert;

  std::ostringstream context;

  int number_context_lines;

  bool is_after_change;
  bool wait_change;
  std::vector<bool> in_function;

  std::any context_type;
  std::list<std::string>::size_type length;
  std::list<std::string> additional_context;

  bool is_after_additional;
  size_t after_edit_count;
  size_t last_context_line;

  bool change_starting_line;
  std::string change_ending_space;
  int change_ending_operation;

  unsigned int close_num_spans;

public:

  unified_view(const std::string & output_filename,
               const std::string & syntax_highlight,
               const std::string & theme,
               bool ignore_all_whitespace,
               bool ignore_whitespace,
               bool ignore_comments,
               bool is_html,
               std::any context_type);
  virtual ~unified_view();

private:

  virtual void reset_internal();

  bool in_mode(context_mode mode);

  void output_additional_context();

  virtual void start_unit(const std::string & local_name,
                          const char * prefix,
                          const char * URI,
                          int num_namespaces,
                          const struct srcsax_namespace * namespaces,
                          int num_attributes,
                          const struct srcsax_attribute * attributes);
  virtual void end_unit(const std::string & local_name,
                        const char * prefix,
                        const char * URI);
  virtual void start_element(const std::string & local_name,
                             const char * prefix,
                             const char * URI, int num_namespaces,
                             const struct srcsax_namespace * namespaces,
                             int num_attributes,
                             const struct srcsax_attribute * attributes);
  virtual void end_element(const std::string & local_name,
                           const char * prefix,
                           const char * URI);
  virtual void characters(const char * ch, int len);

  context_mode context_string_to_id(const std::string & context_type_str) const;

public:
  
  virtual void output_characters(const std::string & ch, int operation);

};

#endif
