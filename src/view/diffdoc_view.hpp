#ifndef INCLUDED_DIFFDOC_VIEW_HPP
#define INCLUDED_DIFFDOC_VIEW_HPP

#include <view.hpp>

#include <boost/any.hpp>

#include <vector>
#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>

class diffdoc_view : public view_t {

private:
  unsigned int num_open_spans;
  int last_character_operation;

  size_t line_number_delete;
  size_t line_number_insert;

public:

  diffdoc_view(const std::string & output_filename,
               const std::string & syntax_highlight,
               const std::string & theme);
  virtual ~diffdoc_view();

private:

  void start_line();
  void end_line();

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
  
  virtual void output_characters(const std::string & ch, int operation);
  void output_characters(const std::string & str);
};

#endif
