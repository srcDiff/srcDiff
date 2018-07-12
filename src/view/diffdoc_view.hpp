#ifndef INCLUDED_DIFFDOC_VIEW_HPP
#define INCLUDED_DIFFDOC_VIEW_HPP

#include <view.hpp>

#include <boost/any.hpp>

#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stack>

struct entity_data {
public:
  size_t line_number_delete;
  size_t line_number_insert;
  bool collect_id;
  std::string id;

  bool is_changed;
public:
  entity_data(size_t line_number_delete, size_t line_number_insert);
};

class diffdoc_view : public view_t {

private:
  unsigned int num_open_spans;
  int last_character_operation;

  size_t line_number_delete;
  size_t line_number_insert;

  std::stack<std::ostringstream> saved_output;
  std::stack<entity_data> entity_stack;

public:

  diffdoc_view(const std::string & output_filename,
               const std::string & syntax_highlight,
               const std::string & theme);
  virtual ~diffdoc_view();

private:

  virtual void reset_internal();

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


public:

  std::ostream * get_output_stream();
  void add_saved_output();
  std::string remove_saved_output();

  std::string form_line_str(size_t original_line, size_t modified_line);
  void start_line();
  void end_line();
  void end_spans();
  void output_raw_str(const std::string & str);

  void output_characters(const std::string & str);
  virtual void output_characters(const std::string & str, int operation);

};

#endif
