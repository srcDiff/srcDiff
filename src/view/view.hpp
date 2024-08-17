// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file view.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_BASH_VIEW_HPP
#define INCLUDED_BASH_VIEW_HPP

#include <srcSAXHandler.hpp>

#include <theme.hpp>

#include <versioned_string.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>


class view_t : public srcSAXHandler {

public:

  static int UNSET;
  static int COMMON;
  static int DELETE;
  static int INSERT;

protected:

  static const char * const LINE_CODE;

  static const char * const CARRIAGE_RETURN_SYMBOL;

  std::vector<int> diff_stack;
  std::vector<std::string> srcml_stack;

  std::ostream * output;

  bool syntax_highlight;
  theme_t * theme;

  bool in_comment;
  bool in_literal;
  bool in_string;
  bool in_function_name;
  bool in_class_name;
  bool in_call_name;

  bool in_preprocessor_directive;

  bool ignore_all_whitespace;
  bool ignore_whitespace;
  bool ignore_comments;

  bool is_html;

  unsigned int close_num_span;

  bool save_name;
  std::string saved_name_type;
  versioned_string saved_name;

public:

  view_t(const std::string & output_filename,
         const std::string & syntax_highlight,
         const std::string & theme, 
         bool ignore_all_whitespace,
         bool ignore_whitespace,
         bool ignore_comments,
         bool is_html);
  virtual ~view_t();

  virtual void transform(const std::string & srcdiff, const std::string & xml_encoding);
  void reset();

protected:

  void srcml_stack_push(const char * localname, const char * prefix);

  virtual void reset_internal() = 0;

  virtual void start_unit(const std::string & local_name,
                          const char * prefix,
                          const char * URI,
                          int num_namespaces,
                          const struct srcsax_namespace * namespaces,
                          int num_attributes,
                          const struct srcsax_attribute * attributes) = 0;
  virtual void end_unit(const std::string & local_name,
                        const char * prefix,
                        const char * URI) = 0;
  virtual void start_element(const std::string & local_name,
                             const char * prefix,
                             const char * URI, int num_namespaces,
                             const struct srcsax_namespace * namespaces,
                             int num_attributes,
                             const struct srcsax_attribute * attributes) = 0;
  virtual void end_element(const std::string & local_name,
                           const char * prefix,
                           const char * URI) = 0;
  virtual void characters(const char * ch, int len) = 0;

  void end_buffer(std::ostream & out, unsigned int & close_num_span);
  void output_characters_to_buffer(std::ostream & out,
                                   const std::string & ch,
                                   int operation,
                                   int & last_character_operation,
                                   unsigned int & close_num_span);
  void output_character(const char c, int operation);

public:

  virtual void output_characters(const std::string & ch, int operation) = 0;

private:
  std::string change_operation_to_code(int operation);
  std::string close_spans(unsigned int close_num_span);

public:

  /**
   * startDocument
   *
   * SAX handler function for start of document.
   * Overide for desired behaviour.
   */
  virtual void startDocument();

  /**
   * endDocument
   *
   * SAX handler function for end of document.
   * Overide for desired behaviour.
   */
  virtual void endDocument();

  /**
   * startRoot
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   * @param num_namespaces number of namespaces definitions
   * @param namespaces the defined namespaces
   * @param num_attributes the number of attributes on the tag
   * @param attributes list of attributes
   *
   * SAX handler function for start of the root profile.
   * Overide for desired behaviour.
   */
  virtual void startRoot(const char * localname, const char * prefix, const char * URI,
                         int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                         const struct srcsax_attribute * attributes);

  /**
   * startUnit
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   * @param num_namespaces number of namespaces definitions
   * @param namespaces the defined namespaces
   * @param num_attributes the number of attributes on the tag
   * @param attributes list of attributes
   *
   * SAX handler function for start of an unit.
   * Overide for desired behaviour.
   */
  virtual void startUnit(const char * localname, const char * prefix, const char * URI,
                         int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                         const struct srcsax_attribute * attributes);

  /**
   * startElement
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   * @param num_namespaces number of namespaces definitions
   * @param namespaces the defined namespaces
   * @param num_attributes the number of attributes on the tag
   * @param attributes list of attributes
   *
   * SAX handler function for start of an profile.
   * Overide for desired behaviour.
   */
  virtual void startElement(const char * localname, const char * prefix,
                            const char * URI, int num_namespaces,
                            const struct srcsax_namespace * namespaces,
                            int num_attributes,
                            const struct srcsax_attribute * attributes);

  /**
   * endRoot
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   *
   * SAX handler function for end of the root profile.
   * Overide for desired behaviour.
   */
  virtual void endRoot(const char * localname, const char * prefix, const char * URI);

  /**
   * endUnit
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   *
   * SAX handler function for end of an unit.
   * Overide for desired behaviour.
   */
  virtual void endUnit(const char * localname, const char * prefix, const char * URI);

  /**
   * endElement
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   *
   * SAX handler function for end of an profile.
   * Overide for desired behaviour.
   */
  virtual void endElement(const char * localname, const char * prefix, const char * URI);

  /**
   * charactersRoot
   * @param ch the characers
   * @param len number of characters
   *
   * SAX handler function for character handling at the root level.
   * Overide for desired behaviour.
   */
  virtual void charactersRoot(const char * ch, int len);

  /**
   * charactersUnit
   * @param ch the characers
   * @param len number of characters
   *
   * SAX handler function for character handling within a unit.
   * Overide for desired behaviour.
   */
  virtual void charactersUnit(const char * ch, int len);

};

#endif
