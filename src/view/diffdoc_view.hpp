#ifndef INCLUDED_DIFFDOC_VIEW_HPP
#define INCLUDED_DIFFDOC_VIEW_HPP

#include <view.hpp>

#include <versioned_string.hpp>
#include <srcdiff_summary.hpp>
#include <profile_t.hpp>
#include <unit_profile_t.hpp>

#include <boost/any.hpp>

#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stack>
#include <vector>

struct entity_data {
public:
  std::string type;

  size_t depth;

  std::string indentation;

  size_t line_number_delete;
  size_t line_number_insert;

  bool collect_id;
  versioned_string id;
  std::string signature;

  bool collect_name;
  versioned_string name;

  bool is_changed;
  std::shared_ptr<profile_t> change_profile;

public:
  entity_data(const std::string & type, size_t depth, 
              const std::string & indentation, size_t line_number_delete, size_t line_number_insert);
};

class diffdoc_view : public view_t {

private:

  std::shared_ptr<srcdiff_summary> & summarizer;
  std::shared_ptr<unit_profile_t> unit_profile;

  unsigned int num_open_spans;
  int last_character_operation;

  size_t line_number_delete;
  size_t line_number_insert;

  bool collect_indentation;
  std::string indentation;

  std::stack<std::ostringstream> saved_output;

  std::vector<entity_data> entity_stack;

public:

  diffdoc_view(const std::string & output_filename,
               const std::string & syntax_highlight,
               const std::string & theme,
               std::shared_ptr<srcdiff_summary> & summarizer);
  virtual ~diffdoc_view();

private:

  virtual void transform(const std::string & srcdiff, const std::string & xml_encoding);
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

  srcdiff_type view_op2srcdiff_type(int operation);

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
