#ifndef INCLUDED_UNIFIED_VIEW_HPP
#define INCLUDED_UNIFIED_VIEW_HPP

#include <bash_view.hpp>

#include <boost/any.hpp>

#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <iostream>

class unified_view : public bash_view {

private:

  typedef unsigned long context_mode;

  static const context_mode LINE     = 1 << 0;
  static const context_mode FUNCTION = 1 << 1;
  static const context_mode ALL      = 1 << 2;

  context_mode modes;

  int last_character_operation;

  size_t line_number_delete;
  size_t line_number_insert;

  std::string context;

  size_t number_context_lines;

  bool is_after_change;
  bool wait_change;
  std::vector<bool> in_function;

  boost::any context_type;
  std::list<std::string>::size_type length;
  std::list<std::string> additional_context;

  bool is_after_additional;
  size_t after_edit_count;
  size_t last_context_line;

  bool change_starting_line;
  std::string change_ending_space;
  int change_ending_operation;

public:

  unified_view(const std::string & output_filename, bool ignore_all_whitespace,
               bool ignore_whitespace, bool ignore_comments, boost::any context_type);
  virtual ~unified_view();

private:

  virtual void reset_internal();

  bool in_mode(context_mode mode);

  void output_additional_context();

  virtual void start_element(const std::string & local_name, const char * prefix, const char * URI,
                            int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                            const struct srcsax_attribute * attributes);
  virtual void end_element(const std::string & local_name, const char * prefix,
                           const char * URI);
  virtual void characters(const char * ch, int len);

  context_mode context_string_to_id(const std::string & context_type_str) const;

  virtual void output_characters(const std::string ch, int operation);

public:

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
   * endUnit
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   *
   * SAX handler function for end of an unit.
   * Overide for desired behaviour.
   */
  virtual void endUnit(const char * localname, const char * prefix, const char * URI);

};

#endif
